#ifndef MarketService_h__
#define MarketService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CMarketService : public IService, public CServiceCommon
{
    CMarketService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CMarketService();
    void Destroy();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CMarketService);

public:
    virtual void OnLogicThreadCreate() override;
};

CMarketService* MarketService();
inline auto     EventManager()
{
    return MarketService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return MarketService()->GetNetMsgProcess();
}

#endif // MarketService_h__
