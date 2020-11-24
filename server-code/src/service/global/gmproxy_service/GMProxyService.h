#ifndef GMProxyService_h__
#define GMProxyService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CRPCService;
class CGMProxyService : public IService, public CServiceCommon
{
    CGMProxyService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CGMProxyService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CGMProxyService);

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    void         OnReciveHttp(struct evhttp_request* req);

    void                   AddDelayResponse(uint64_t uid, struct evhttp_request* req);
    struct evhttp_request* FindDelayResponse(uint64_t uid);

private:
    std::map<uint64_t, struct evhttp_request*> m_RequestMap;
    std::unique_ptr<CRPCService>               m_pRPCService;
};

CGMProxyService* GMProxyService();
inline auto      EventManager()
{
    return GMProxyService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return GMProxyService()->GetNetMsgProcess();
}

#endif // GMProxyService_h__
