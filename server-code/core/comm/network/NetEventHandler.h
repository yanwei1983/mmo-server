#ifndef NETEVENTHANDLER_H
#define NETEVENTHANDLER_H

class CNetSocket;
class CNetEventHandler
{
public:
    CNetEventHandler() {}
    virtual ~CNetEventHandler() {}

public:
    // socket new
    virtual void OnBindSocket(CNetSocket*){}
    // socket delete
    virtual void OnUnbindSocket(CNetSocket*){}
    // connect to other server start
    virtual void OnStartConnect(CNetSocket*){}
    // connect to other server succ
    virtual void OnConnected(CNetSocket*){}
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnConnectFailed(CNetSocket*){}
    // lost connect
    virtual void OnDisconnected(CNetSocket*) = 0;
    // closeing
    virtual void OnClosing(CNetSocket*){}
    // accept a new client
    virtual void OnAccepted(CNetSocket*) = 0;
    // receive data
    virtual void OnRecvData(CNetSocket*, byte* pBuffer, size_t len) = 0;
    // recv over time
    virtual void OnRecvTimeout(CNetSocket*) = 0;
    // WaitReconnect
    virtual void OnWaitReconnect(CNetSocket*){};

};


#endif /* NETEVENTHANDLER_H */
