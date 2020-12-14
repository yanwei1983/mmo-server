#include "GuildService.h"

#include <functional>

#include "EventManager.h"
#include "GuildManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "server_msg/server_side.pb.h"

static thread_local CGuildService* tls_pService = nullptr;
CGuildService*                     GuildService()
{
    return tls_pService;
}
void SetGuildServicePtr(CGuildService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CGuildService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CGuildService::CGuildService() {}

CGuildService::~CGuildService() {}

void CGuildService::Release()
{

    Destory();
    delete this;
}

void CGuildService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CGuildService::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
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

    m_UIDFactory.Init(0, GUILD_SERIVE_UID);

    auto pGlobalDB = ConnectGlobalDB();

    CHECKF(pGlobalDB.get());
    m_pGlobalDB.reset(pGlobalDB.release());

    m_pGuildManager.reset(CGuildManager::CreateNew());
    CHECKF(m_pGuildManager.get());

    //注册消息
    RegisterAllMsgProcess<CGuildService>(GetNetMsgProcess());

    if(CreateService(100) == false)
        return false;

    return true;
    __LEAVE_FUNCTION
    return fales;
}

void CGuildService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}
