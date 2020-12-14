#ifndef MessagePort_h__
#define MessagePort_h__

#include "LockfreeQueue.h"
#include "NetEventHandler.h"
#include "NetworkDefine.h"
#include "Noncopyable.h"

class CNetSocket;
using CNetSocketSharedPtr = std::shared_ptr<CNetSocket>;
using CNetSocketWeakPtr   = std::weak_ptr<CNetSocket>;

class CMessageRoute;
class CNetworkMessage;

class CMessagePortEventHandler
{
public:
    CMessagePortEventHandler() {}
    virtual ~CMessagePortEventHandler() {}

public:
    // connect to other server succ
    virtual void OnPortConnected(const CNetSocketSharedPtr&) {}
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnPortConnectFailed(const CNetSocketSharedPtr&) {}
    // lost connect
    virtual void OnPortDisconnected(const CNetSocketSharedPtr&) {}
    // accept a new client
    virtual void OnPortAccepted(const CNetSocketSharedPtr&) {}
    // receive data
    virtual void OnPortRecvData(const CNetworkMessage&) {}
    // recv over time
    virtual void OnPortRecvTimeout(const CNetSocketSharedPtr&) {}
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
    virtual size_t GetPacketSizeMax() const override { return _MAX_MSGSIZE * 10; }
    virtual size_t GetLogWriteHighWateMark() const override { return _DEFAULT_LOGWRITEHIGHWATEMARK; }
    virtual void   OnBindSocket(const CNetSocketSharedPtr& pSocket) override;
    virtual void   OnUnbindSocket(const CNetSocketSharedPtr& pSocket) override;
    virtual void   OnStartConnect(const CNetSocketSharedPtr& pSocket) override;
    virtual void   OnConnected(const CNetSocketSharedPtr& pSocket) override;
    virtual void   OnConnectFailed(const CNetSocketSharedPtr&) override;
    virtual void   OnDisconnected(const CNetSocketSharedPtr&) override;
    virtual void   OnWaitReconnect(const CNetSocketSharedPtr&) override;

    virtual void OnAccepted(const CNetSocketSharedPtr&) override;
    virtual void OnRecvData(const CNetSocketSharedPtr&, byte* pBuffer, size_t len) override;
    virtual void OnRecvTimeout(const CNetSocketSharedPtr&) override;
    virtual void OnClosing(const CNetSocketSharedPtr&) override;

    void                SetPortEventHandler(CMessagePortEventHandler* pHandler) { m_pPortEventHandler = pHandler; }
    void                SetRemoteSocket(const CNetSocketSharedPtr& pRemoteSocket);
    void                DetachRemoteSocket();
    CNetSocketSharedPtr GetRemoteSocket() const;
    const ServerPort&   GetServerPort() const { return m_nServerPort; }
    bool                GetLocalPort() const { return m_bLocalPort; }
    void                SetLocalPort(bool val) { m_bLocalPort = val; }

    bool   TakePortMsg(CNetworkMessage*& msg);
    bool   SendMsgToPort(const CNetworkMessage& msg);
    size_t GetWriteBufferSize();

private:
    void _SendMsgToRemoteSocket(const CNetworkMessage& msg);

private:
    CMessageRoute*              m_pRoute = nullptr;
    MPSCQueue<CNetworkMessage*> m_RecvMsgQueue;

    CNetSocketWeakPtr                      m_pRemoteSocket;
    bool                                   m_bLocalPort = false;
    ServerPort                             m_nServerPort;
    std::atomic<CMessagePortEventHandler*> m_pPortEventHandler = nullptr;

    std::unordered_set<SocketIdx_t> m_SocketIdxList;
};

using CMessagePortSharedPtr = std::shared_ptr<CMessagePort>;
using CMessagePortWeakPtr   = std::weak_ptr<CMessagePort>;

#endif // MessagePort_h__
