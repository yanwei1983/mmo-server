#ifndef MarketService_h__
#define MarketService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CGMManager;
class CAuthManager;
class CMysqlConnection;

class CAuthService : public IService, public CServiceCommon
{
    CAuthService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CAuthService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CAuthService);

public:
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadProc() override;

public:
    CGMManager*   GetGMManager() const { return m_pGMManager.get(); }
    CAuthManager* GetAuthManager() const { return m_pAuthManager.get(); }

private:
    std::unique_ptr<CGMManager>   m_pGMManager;
    std::unique_ptr<CAuthManager> m_pAuthManager;
};

CAuthService* AuthService();
void          SetAuthServicePtr(CAuthService* ptr);

inline auto EventManager()
{
    return AuthService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return AuthService()->GetNetMsgProcess();
}
inline auto GMManager()
{
    return AuthService()->GetGMManager();
}
inline auto AuthManager()
{
    return AuthService()->GetAuthManager();
}

#endif // MarketService_h__
