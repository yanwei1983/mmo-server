#include "AIService.h"

#include <functional>

#include "AIActorManager.h"
#include "AIFuzzyLogic.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AISceneManagr.h"
#include "AISkill.h"
#include "AIType.h"
#include "EventManager.h"
#include "GameEventDef.h"
#include "MapManager.h"
#include "MemoryHelp.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MonsterType.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetworkMessage.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "server_msg/server_side.pb.h"
#include "BehaviorTree.h"

static thread_local CAIService* tls_pService;
CAIService*                     AIService()
{
    return tls_pService;
}
void SetAIServicePtr(CAIService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CreateNewRelease<CAIService>(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CAIService::CAIService()
{
    m_tLastDisplayTime.Startup(60);
}

CAIService::~CAIService() {}

void CAIService::Release()
{

    Destroy();
    delete this;
}

void CAIService::Destroy()
{
    __ENTER_FUNCTION

    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    StopLogicThread();
    if(m_pAISceneManager)
    {
        m_pAISceneManager->Destroy();
        m_pAISceneManager.reset();
    }
    if(m_pAIActorManager)
    {
        m_pAIActorManager->Destroy();
        m_pAIActorManager.reset();
    }
    DestoryServiceCommon();

    __LEAVE_FUNCTION
}

bool CAIService::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    //各种初始化
    scope_guards scope_exit;
    tls_pService = this;
    scope_exit += []() {
        tls_pService = nullptr;
    };

    CServiceCommon::Init(nServerPort, true);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };
    extern void export_to_lua(lua_State*, void*);
    m_pScriptManager.reset(CreateNew<CLUAScriptManager>(std::string("AIScript") + std::to_string(GetServerPort().GetServiceID()),
                                                        &export_to_lua,
                                                        (void*)this,
                                                        "res/script/ai_service",
                                                        "main.lua",
                                                        true));

    m_pMapManager.reset(CreateNew<CMapManager>(GetZoneID()));
    CHECKF(m_pMapManager.get());
    
    DEFINE_CONFIG_LOAD(CTargetFAMSet);
    DEFINE_CONFIG_LOAD(CSkillFAMSet);
    DEFINE_CONFIG_LOAD(CSkillTypeSet);
    DEFINE_CONFIG_LOAD(CAITypeSet);
    DEFINE_CONFIG_LOAD(CMonsterTypeSet);

    std::unordered_set<std::string> all_bt_file;
    AITypeSet()->foreach([&all_bt_file](const auto& k, const auto& ptr)
    {
        if(ptr->GetDataRef().bt_file().empty() == false)
        {
            all_bt_file.emplace(ptr->GetDataRef().bt_file());
        }
    });

    m_pBTManager.reset(CreateNew<BT::BTManager>("res/bttree", ScriptManager()->GetRawPtr(), all_bt_file) );
    CHECKF(m_pBTManager.get());

        

    m_pAISceneManager.reset(CreateNew<CAISceneManager>(GetZoneID()));
    CHECKF(m_pAISceneManager.get());
    m_pAIActorManager.reset(CreateNew<CAIActorManager>());
    CHECKF(m_pAIActorManager.get());

    RegisterAllMsgProcess<CAIService>(GetNetMsgProcess());

    uint32_t FrameCount    = 20;
    uint32_t FrameInterval = 1000 / FrameCount;
    if(CreateService(FrameInterval) == false)
        return false;

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendProtoMsgToScene(msg);

    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CAIService::SendProtoMsgToScene(const proto_msg_t& msg)
{
    return SendProtoMsgToZonePort(GetSceneServerPort(), msg);
}

void CAIService::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    CServiceCommon::OnLogicThreadProc();
    // lua step gc
    m_pScriptManager->OnTimer(TimeGetMonotonic());
    AISceneManager()->OnTimer();

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================") +
                          attempt_format(FMT_STRING("\nMessageProcess:{}\tMem:{}"), GetMessageProcess(), get_thread_memory_allocted());
        buf += attempt_format(FMT_STRING("\nEvent:{}\tActive:{}"), EventManager()->GetEventCount(), EventManager()->GetRunningEventCount());

        for(const auto& [k, v]: EventManager()->GetCountEntryByManagerType())
        {
            auto result = magic_enum::enum_cast<EventManagerType>(k);
            if(result)
            {
                buf += attempt_format(FMT_STRING("\nManagerType:{}\tCount:{}"), magic_enum::enum_name(result.value()), v);
            }
        }
        for(const auto& [k, v]: EventManager()->GetCountEntryByType())
        {
            auto result = magic_enum::enum_cast<GameEventType>(k);
            if(result)
            {
                buf += attempt_format(FMT_STRING("\nEvType:{}\tCount:{}"), magic_enum::enum_name(result.value()), v);
            }
        }

        auto pMessagePort = GetMessageRoute()->QueryMessagePort(GetSceneServerPort(), false);
        if(pMessagePort)
        {
            buf += attempt_format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"), GetZoneID(), pMessagePort->GetWriteBufferSize());
        }
        LOGMONITOR("{}", buf.c_str());
        m_pMonitorMgr->Print();
        SetMessageProcess(0);
    }
    __LEAVE_FUNCTION
}

void CAIService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}
