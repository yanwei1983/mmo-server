#include "AOIService.h"

#include <functional>

#include "AOIActorManager.h"
#include "AOIPlayer.h"
#include "AOISceneManagr.h"
#include "EventManager.h"
#include "MapManager.h"
#include "MemoryHelp.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "NetMSGProcess.h"
#include "MsgProcessRegister.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "server_msg/server_side.pb.h"

static thread_local CAOIService* tls_pService;
CAOIService*                     AOIService()
{
    return tls_pService;
}
void SetAOIServicePtr(CAOIService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CAOIService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CAOIService::CAOIService()
{
    m_tLastDisplayTime.Startup(60);
}

CAOIService::~CAOIService() {}

void CAOIService::Release()
{

    Destory();
    delete this;
}

void CAOIService::Destory()
{
    __ENTER_FUNCTION

    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    StopLogicThread();
    if(m_pAOISceneManager)
    {
        m_pAOISceneManager->Destory();
        m_pAOISceneManager.reset();
    }
    if(m_pAOIActorManager)
    {
        m_pAOIActorManager->Destroy();
        m_pAOIActorManager.reset();
    }
    DestoryServiceCommon();

    __LEAVE_FUNCTION
}

bool CAOIService::Init(const ServerPort& nServerPort)
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

    m_pMapManager.reset(CMapManager::CreateNew(GetZoneID()));
    CHECKF(m_pMapManager.get());

    m_pAOISceneManager.reset(CAOISceneManager::CreateNew(GetZoneID()));
    CHECKF(m_pAOISceneManager.get());
    m_pAOIActorManager.reset(CAOIActorManager::CreateNew());
    CHECKF(m_pAOIActorManager.get());

    RegisterAllMsgProcess<CAOIService>(GetNetMsgProcess());

    constexpr uint32_t FrameCount    = 20;
    constexpr uint32_t FrameInterval = 1000 / FrameCount;
    if(CreateService(FrameInterval) == false)
        return false;

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendProtoMsgToSceneService(msg);

    return true;

    __LEAVE_FUNCTION
    return false;
}

void CAOIService::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    CServiceCommon::OnLogicThreadProc();

    AOISceneManager()->OnTimer();

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

void CAOIService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

bool CAOIService::SendProtoMsgToAIService(const proto_msg_t& msg) const
{
    return SendProtoMsgToZonePort(GetAIServerPort(), msg);
}

bool CAOIService::SendProtoMsgToSceneService(const proto_msg_t& msg) const
{
    return SendProtoMsgToZonePort(GetSceneServerPort(), msg);
}