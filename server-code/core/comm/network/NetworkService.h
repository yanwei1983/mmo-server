#ifndef NetworkService_h__
#define NetworkService_h__

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "BaseCode.h"
#include "LockfreeQueue.h"
#include "NetworkDefine.h"
#include "NetworkMessage.h"
#include "PerSecondCount.h"
#include "Thread.h"

constexpr uint16_t INVALID_SOCKET_IDX = 0xFFFF;
constexpr uint16_t MAX_SOCKET_IDX     = 0x8000;

struct event_base;
struct event;
struct evconnlistener;
struct lws_context;

class CNetSocket;
using CNetSocketSharedPtr = std::shared_ptr<CNetSocket>;
using CNetSocketWeakPtr   = std::weak_ptr<CNetSocket>;
class CNetWebSocket;
class CNetEventHandler;
using CNetEventHandlerSharedPtr = std::shared_ptr<CNetEventHandler>;
using CNetEventHandlerWeakPtr   = std::weak_ptr<CNetEventHandler>;
class CServerSocket;
using CServerSocketSharedPtr = std::shared_ptr<CServerSocket>;
using CServerSocketWeakPtr   = std::weak_ptr<CServerSocket>;
class CClientSocket;
using CClientSocketSharedPtr = std::shared_ptr<CClientSocket>;
using CClientSocketWeakPtr   = std::weak_ptr<CClientSocket>;

class CNetworkService
{
public:
    CNetworkService();
    virtual ~CNetworkService();

    void Destroy();
    //监听
    evconnlistener* Listen(const std::string& addr, int32_t port, const CNetEventHandlerSharedPtr& pEventHandler);
    bool            EnableListener(evconnlistener* listener, bool bEnable = false);
    // http监听
    bool ListenHttpPort(const std::string& addr, int32_t port, std::function<void(struct evhttp_request* req)>&& func);
    //阻塞连接到一个目标地址
    CServerSocketWeakPtr ConnectTo(const std::string& addr, int32_t port, const CNetEventHandlerSharedPtr& pEventHandler, bool bAutoReconnect = false);
    //异步连接到一个目标地址
    CServerSocketWeakPtr AsyncConnectTo(const std::string& addr, int32_t port, const CNetEventHandlerSharedPtr& pEventHandler, bool bAutoReconnect = false);
    bool                 _AsyncReconnect(const CServerSocketSharedPtr& pSocket);

    void BreakLoop();
    //开启独立的IO线程
    void StartIOThread(const std::string&      thread_name,
                       std::function<void()>&& time_out_func = std::function<void()>(),
                       uint32_t                time_out_ms   = 60 * 60,
                       const ServiceID&        idService     = 0);
    void OnIOThreadTimeOut();

    //读取IO一次，如果开启了独立IO线程则不需调用
    void RunOnce();

    virtual CServerSocketSharedPtr CreateServerSocket(const CNetEventHandlerSharedPtr& pHandle, bool bAutoReconnect);
    virtual CClientSocketSharedPtr CreateClientSocket(const CNetEventHandlerSharedPtr& pHandle);
    struct evhttp*                 GetHttpHandle() const { return m_pHttpHandle; }
    bool                           GetIPCheck() const { return m_bIPCheck; }
    void                           SetIPCheck(bool val) { m_bIPCheck = val; }
    size_t                         GetIPCheckNum() const { return m_nIPCheckNum; }
    void                           SetIPCheckNum(size_t val) { m_nIPCheckNum = val; }
    MPSCQueue<CNetworkMessage*>&   _GetMessageQueue() { return m_MessageQueue; }

public:
    static void accept_conn_cb(evconnlistener*, int32_t fd, struct sockaddr* addr, int32_t socklen, void* arg);
    static void accept_error_cb(struct evconnlistener*, void* arg);
    static void http_process_cb(struct evhttp_request* req, void* arg);

    void OnAccept(int32_t fd, struct sockaddr* addr, int32_t, evconnlistener* listener);

public:
    event_base* GetEVBase() const { return m_pBase; }
    size_t      GetSocketAmount();
    // socket广播消息
    void BrocastMsg(const CNetworkMessage& msg, SOCKET execpt_this);
    //直接发送Socket消息
    bool SendSocketMsg(SOCKET _socket, const CNetworkMessage& msg, bool bNeedDuplicate);
    bool SendSocketMsgByIdx(SocketIdx_t nSocketIdx, const CNetworkMessage& msg, bool bNeedDuplicate);
    //主动关闭一个连接
    bool KickSocket(SOCKET _socket);

    void            AddRecvByteCount(size_t len);
    void            AddSendByteCount(size_t len);
    PerSecondCount& GetRecvBPS() { return m_RecvBPS; }
    PerSecondCount& GetSendBPS() { return m_SendBPS; }

public:
    void _AddSocket(const CNetSocketSharedPtr& pSocket);
    void _CloseSocket(uint32_t nSocketIdx);
    void _AddConnectingSocket(const CNetSocketSharedPtr& pSocket);
    void _RemoveSocket(const CNetSocketSharedPtr& pSocket);

public:
    void _AddClosingSocket(const CNetSocketSharedPtr& pSocket);
    void _ReleaseSocket(const CNetSocketSharedPtr& pSocket);

public:
    bool                _AllocSocketIdx(const CNetSocketSharedPtr& pSocket);
    void                _ReleaseSocketIdx(const CNetSocketSharedPtr& pSocket);
    CNetSocketSharedPtr QuerySocketByIdx(uint16_t nSocketIdx);

public:
    void JoinIOThread();

private:
    void                      _ProceseClosingSocket();
    CNetEventHandlerSharedPtr QueryListenerEventHander(evconnlistener* listener);

protected:
    event_base*                                        m_pBase;
    std::map<evconnlistener*, CNetEventHandlerWeakPtr> m_setListener;
    struct evhttp*                                     m_pHttpHandle = nullptr;
    std::function<void(struct evhttp_request* req)>    m_funcOnReciveHttp;
    std::mutex                                         m_mutex;
    std::mutex                                         m_mutexListener;

    std::map<SOCKET, CNetSocketSharedPtr>           m_setSocket;
    std::deque<SocketIdx_t>                         m_SocketIdxPool;
    std::array<CNetSocketSharedPtr, MAX_SOCKET_IDX> m_setSocketByIdx;

    std::unordered_set<CNetSocketSharedPtr> m_setConnectingSocket;

    MPSCQueue<CNetworkMessage*> m_MessageQueue;

    std::unique_ptr<std::thread> m_pIOThread;

    struct event*     m_pIOTimeOutEvent                 = nullptr;
    struct event*     m_pCloseSocketEvent               = nullptr;
    std::atomic<bool> m_bWaitingProcessCloseSocketEvent = false;

    MPSCQueue<CNetSocketSharedPtr> m_setClosingSocket;

    PerSecondCount m_RecvBPS;
    PerSecondCount m_SendBPS;

    std::atomic<bool>   m_bStop = false;
    std::vector<event*> m_setEvTimed;

    uint32_t m_nWorkInterval = 0;

    struct IPInfo
    {
        time_t nextResetTime = 0;
        size_t nCount        = 0;
    };
    std::map<std::string, IPInfo> m_setIP;
    bool                          m_bIPCheck    = false;
    size_t                        m_nIPCheckNum = 400;

    std::function<void()> m_IOThreadTimeOutFunc;
};

#endif // NetworkService_h__
