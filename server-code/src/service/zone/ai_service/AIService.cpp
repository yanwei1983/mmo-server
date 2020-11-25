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
#include "MapManager.h"
#include "MemoryHelp.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MonsterType.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "server_msg/server_side.pb.h"

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
    return CAIService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CAIService::CAIService()
{
    m_tLastDisplayTime.Startup(60);
}

CAIService::~CAIService() {}

void CAIService::Release()
{

    Destory();
    delete this;
}

void CAIService::Destory()
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
        m_pAISceneManager->Destory();
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

    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };
    BaseCode::CreateExtLogDir();
    extern void export_to_lua(lua_State*, void*);
    m_pScriptManager.reset(CLUAScriptManager::CreateNew(std::string("AIScript") + std::to_string(GetServerPort().GetServiceID()),
                                                        &export_to_lua,
                                                        (void*)this,
                                                        "res/script/ai_service",
                                                        true));

    m_pMapManager.reset(CMapManager::CreateNew(GetZoneID()));
    CHECKF(m_pMapManager.get());

    DEFINE_CONFIG_LOAD(CTargetFAMSet);
    DEFINE_CONFIG_LOAD(CSkillFAMSet);
    DEFINE_CONFIG_LOAD(CSkillTypeSet);
    DEFINE_CONFIG_LOAD(CAITypeSet);
    DEFINE_CONFIG_LOAD(CMonsterTypeSet);

    m_pAISceneManager.reset(CAISceneManager::CreateNew(GetZoneID()));
    CHECKF(m_pAISceneManager.get());
    m_pAIActorManager.reset(CAIActorManager::CreateNew());
    CHECKF(m_pAIActorManager.get());

    RegisterAllMsgProcess<CAIService>();

    uint32_t FrameCount    = 20;
    uint32_t FrameInterval = 1000 / FrameCount;
    if(CreateService(FrameInterval) == false)
        return false;
    EventManager()->Pause(true);
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

    AISceneManager()->OnTimer();

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================") +
                          fmt::format(FMT_STRING("\nEvent:{}\tActive:{}\tMem:{}"),
                                      EventManager()->GetEventCount(),
                                      EventManager()->GetRunningEventCount(),
                                      get_thread_memory_allocted());
        auto pMessagePort = GetMessageRoute()->QueryMessagePort(GetSceneServerPort(), false);
        if(pMessagePort)
        {
            buf += fmt::format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"), GetZoneID(), pMessagePort->GetWriteBufferSize());
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

void CAIService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
