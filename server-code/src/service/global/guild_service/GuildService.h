#ifndef GuildService_h__
#define GuildService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CGuildManager;

class CGuildService : public IService, public CServiceCommon
{
    CGuildService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CGuildService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CGuildService);
    export_lua uint64_t CreateUID();

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;


    CGuildManager*    GetGuildManager() const { return m_pGuildManager.get(); }
    CMysqlConnection* GetGlobalDB() const { return m_pGlobalDB.get(); }

public:
    CUIDFactory                       m_UIDFactory;
    std::unique_ptr<CMysqlConnection> m_pGlobalDB;
    std::unique_ptr<CGuildManager>    m_pGuildManager;
};

CGuildService* GuildService();
void           SetGuildServicePtr(CGuildService* ptr);
inline auto    EventManager()
{
    return GuildService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return GuildService()->GetNetMsgProcess();
}
inline auto GuildManager()
{
    return WorldService()->GetGuildManager();
}

#endif // GuildService_h__
