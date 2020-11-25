#include "MarketService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"

#include "server_msg/server_side.pb.h"

static thread_local CMarketService* tls_pService = nullptr;
CMarketService*                     MarketService()
{
    return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CMarketService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CMarketService::CMarketService() {}

CMarketService::~CMarketService() {}

void CMarketService::Release()
{

    Destory();
    delete this;
}

void CMarketService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CMarketService::Init(const ServerPort& nServerPort)
{
    //各种初始化
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };

    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };

    if(CreateService(100) == false)
        return false;

    //注册消息
    RegisterAllMsgProcess<CMarketService>();
    
    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendProtoMsgToZonePort(ServerPort(GetWorldID(), WORLD_SERVICE, 0), msg);
    return true;
}


void CMarketService::OnLogicThreadProc()
{
    CServiceCommon::OnLogicThreadProc();
}

void CMarketService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CMarketService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
