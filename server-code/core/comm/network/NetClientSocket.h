#ifndef NETCLIENTSOCKET_H
#define NETCLIENTSOCKET_H

#include "NetSocket.h"

class CClientSocket : public CNetSocket
{
public:
    CClientSocket(CNetworkService* pService, CNetEventHandler* pEventHandler);
    virtual ~CClientSocket();

    // init by accept
    virtual bool Init(bufferevent* pBufferEvent) override;
    virtual void Interrupt(bool bClearEventHandler) override;

public:
    virtual void OnAccepted();
    virtual void _OnClose(const std::string& what) override;
};

#endif /* NETCLIENTSOCKET_H */
