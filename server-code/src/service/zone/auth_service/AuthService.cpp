#include "AuthService.h"

#include <functional>

#include "AuthManager.h"
#include "EventManager.h"
#include "GMManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MsgProcessRegister.h"
#include "MysqlConnection.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "server_msg/server_side.pb.h"

static thread_local CAuthService* tls_pService = nullptr;
CAuthService*                     AuthService()
{
    return tls_pService;
}
void SetAuthServicePtr(CAuthService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CAuthService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CAuthService::CAuthService() {}

CAuthService::~CAuthService() {}

void CAuthService::Release()
{
    Destory();
    delete this;
}

void CAuthService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CAuthService::Init(const ServerPort& nServerPort)
{
    //各种初始化
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };

    CServiceCommon::Init(nServerPort, false);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };

    auto pGlobalDB = ConnectGlobalDB(GetMessageRoute()->GetServerInfoDB());
    CHECKF(pGlobalDB.get());

    m_pAuthManager.reset(CAuthManager::CreateNew(this));
    CHECKF(m_pAuthManager.get());

    m_pGMManager.reset(CGMManager::CreateNew(pGlobalDB.get()));
    CHECKF(m_pGMManager.get());

    //注册消息
    RegisterAllMsgProcess<CAuthService>(GetNetMsgProcess());

    if(CreateService(100) == false)
        return false;

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendProtoMsgToZonePort(ServerPort(GetWorldID(), SOCKET_SERVICE, GetServiceIdx()), msg);
    return true;
}

void CAuthService::OnLogicThreadProc()
{
    CServiceCommon::OnLogicThreadProc();
    m_pAuthManager->ProcessResult();
}

void CAuthService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}
