#ifndef NETCLIENTSOCKET_H
#define NETCLIENTSOCKET_H

#include "NetSocket.h"

class CClientSocket : public CNetSocket
{
public:
    CClientSocket(CNetworkService* pService, const CNetEventHandlerSharedPtr& pEventHandler);
    virtual ~CClientSocket();

    // init by accept
    virtual bool Init(bufferevent* pBufferEvent) override;
    virtual void Interrupt(bool bClearEventHandler) override;

public:
    virtual void OnAccepted();
    virtual void _OnError(const std::string& what) override;
};
using CClientSocketSharedPtr = std::shared_ptr<CClientSocket>;
using CClientSocketWeakPtr = std::weak_ptr<CClientSocket>;

#endif /* NETCLIENTSOCKET_H */
