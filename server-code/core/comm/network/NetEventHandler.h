#ifndef NETEVENTHANDLER_H
#define NETEVENTHANDLER_H

#include <memory>

class CNetSocket;
using CNetSocketSharedPtr = std::shared_ptr<CNetSocket>;

class CNetEventHandler : public std::enable_shared_from_this<CNetEventHandler>
{
public:
    CNetEventHandler() {}
    virtual ~CNetEventHandler() {}

public:
    // socket new
    virtual void OnBindSocket(const CNetSocketSharedPtr&){}
    // socket delete
    virtual void OnUnbindSocket(const CNetSocketSharedPtr&){}
    // connect to other server start
    virtual void OnStartConnect(const CNetSocketSharedPtr&){}
    // connect to other server succ
    virtual void OnConnected(const CNetSocketSharedPtr&){}
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnConnectFailed(const CNetSocketSharedPtr&){}
    // lost connect
    virtual void OnDisconnected(const CNetSocketSharedPtr&){};
    // closeing
    virtual void OnClosing(const CNetSocketSharedPtr&){}
    // accept a new client
    virtual void OnAccepted(const CNetSocketSharedPtr&){};
    // receive data
    virtual void OnRecvData(const CNetSocketSharedPtr&, byte* pBuffer, size_t len){};
    // recv over time
    virtual void OnRecvTimeout(const CNetSocketSharedPtr&) {};
    // WaitReconnect
    virtual void OnWaitReconnect(const CNetSocketSharedPtr&){};

};
using CNetEventHandlerSharedPtr = std::shared_ptr<CNetEventHandler>;
using CNetEventHandlerWeakPtr = std::weak_ptr<CNetEventHandler>;

#endif /* NETEVENTHANDLER_H */
