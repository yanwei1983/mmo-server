#ifndef MessagePort_h__
#define MessagePort_h__

#include "LockfreeQueue.h"
#include "NetworkDefine.h"
#include "NetEventHandler.h"
#include "Noncopyable.h"

class CNetSocket;
class CMessageRoute;
class CNetworkMessage;

class CMessagePortEventHandler
{
public:
    CMessagePortEventHandler() {}
    virtual ~CMessagePortEventHandler() {}

public:
    // connect to other server succ
    virtual void OnPortConnected(CNetSocket*) {}
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnPortConnectFailed(CNetSocket*) {}
    // lost connect
    virtual void OnPortDisconnected(CNetSocket*) {}
    // accept a new client
    virtual void OnPortAccepted(CNetSocket*) {}
    // receive data
    virtual void OnPortRecvData(const CNetworkMessage&) {}
    // recv over time
    virtual void OnPortRecvTimeout(CNetSocket*) {}
};


class CMessagePort : public CNetEventHandler, public NoncopyableT<CMessagePort>, public CreateNewT<CMessagePort>
{
    friend class CreateNewT<CMessagePort>;
    CMessagePort();

public:
    virtual ~CMessagePort();
    void Destory();

public:
    bool Init(const ServerPort& nServerPort, CMessageRoute* pRoute);

public:
    virtual void OnBindSocket(CNetSocket* pSocket) override;
    virtual void OnUnbindSocket(CNetSocket* pSocket) override;
    virtual void OnStartConnect(CNetSocket* pSocket) override;
    virtual void OnConnected(CNetSocket* pSocket) override;
    virtual void OnConnectFailed(CNetSocket*) override;
    virtual void OnDisconnected(CNetSocket*) override;
    virtual void OnWaitReconnect(CNetSocket*) override;
    
    virtual void OnAccepted(CNetSocket*) override;
    virtual void OnRecvData(CNetSocket*, byte* pBuffer, size_t len) override;
    virtual void OnRecvTimeout(CNetSocket*) override;
    virtual void OnClosing(CNetSocket*)override;

    void              SetPortEventHandler(CMessagePortEventHandler* pHandler) { m_pPortEventHandler = pHandler; }
    void              SetRemoteSocket(uint16_t nRemoteSocketIdx);
    void              DetachRemoteSocket();
    CNetSocket*       GetRemoteSocket() const;
    uint16_t          GetRemoteSocketIdx() const {return m_nRemoteSocketIdx;}
    const ServerPort& GetServerPort() const { return m_nServerPort; }
    bool              GetLocalPort() const { return m_bLocalPort; }
    void              SetLocalPort(bool val) { m_bLocalPort = val; }

    bool   TakePortMsg(CNetworkMessage*& msg);
    bool   SendMsgToPort(const CNetworkMessage& msg);
    size_t GetWriteBufferSize();

private:
    void _SendMsgToRemoteSocket(const CNetworkMessage& msg);
private:
    CMessageRoute*                         m_pRoute = nullptr;
    MPSCQueue<CNetworkMessage*>            m_RecvMsgQueue;
    
    SocketIdx_t                            m_nRemoteSocketIdx;
    bool                                   m_bLocalPort          = false;
    ServerPort                             m_nServerPort;
    std::atomic<CMessagePortEventHandler*> m_pPortEventHandler = nullptr;

    std::unordered_set<SocketIdx_t>        m_SocketIdxList;
};

#endif // MessagePort_h__
