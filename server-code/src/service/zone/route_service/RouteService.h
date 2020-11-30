#ifndef RouteService_h__
#define RouteService_h__

#include "IService.h"

#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CRouteService : public IService, public CServiceCommon
{
    CRouteService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CRouteService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CRouteService);

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;

    void SendServiceReady();
    void SendServiceUnReady();
};

CRouteService* RouteService();
void           SetRouteServicePtr(CRouteService* ptr);
inline auto    EventManager()
{
    return RouteService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return RouteService()->GetNetMsgProcess();
}

#endif // RouteService_h__
