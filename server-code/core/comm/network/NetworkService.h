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

struct event_base;
struct event;
struct evconnlistener;
struct lws_context;

class CNetSocket;
class CNetWebSocket;
class CEventManager;

class CNetEventHandler
{
public:
    CNetEventHandler() {}
    virtual ~CNetEventHandler() {}

public:
    // connect to other server succ
    virtual void OnConnected(CNetSocket*) = 0;
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnConnectFailed(CNetSocket*) = 0;
    // lost connect
    virtual void OnDisconnected(CNetSocket*) = 0;
    // accept a new client
    virtual void OnAccepted(CNetSocket*) = 0;
    // receive data
    virtual void OnRecvData(CNetSocket*, byte* pBuffer, size_t len) = 0;
    // recv over time
    virtual void OnRecvTimeout(CNetSocket*) = 0;
};

class CServerSocket;
class CClientSocket;

class CNetworkService
{
public:
    CNetworkService();
    virtual ~CNetworkService();

    void Destroy();
    //监听
    evconnlistener* Listen(const char* addr, int32_t port, CNetEventHandler* pEventHandler);
    bool            EnableListener(evconnlistener* listener, bool bEnable = false);
    // http监听
    bool ListenHttpPort(const char* addr, int32_t port, std::function<void(struct evhttp_request* req)> func);
    //阻塞连接到一个目标地址
    CServerSocket* ConnectTo(const char* addr, int32_t port, CNetEventHandler* pEventHandler);
    //异步连接到一个目标地址
    CServerSocket* AsyncConnectTo(const char* addr, int32_t port, CNetEventHandler* pEventHandler);
    bool           _Reconnect(CServerSocket* pSocket);
    bool           _AsyncReconnect(CServerSocket* pSocket);

    void Stop();
    //开启独立的IO线程
    void StartIOThread(const std::string&    thread_name,
                       std::function<void()> time_out_func = std::function<void()>(),
                       uint32_t              time_out_ms   = 60 * 60,
                       const ServiceID&      idService     = 0);
    void OnIOThreadTimeOut();

    //读取IO一次，如果开启了独立IO线程则不需调用
    void RunOnce();

    virtual CServerSocket*       CreateServerSocket(CNetEventHandler* pHandle);
    virtual CClientSocket*       CreateClientSocket(CNetEventHandler* pHandle);
    struct evhttp*               GetHttpHandle() const { return m_pHttpHandle; }
    bool                         GetIPCheck() const { return m_bIPCheck; }
    void                         SetIPCheck(bool val) { m_bIPCheck = val; }
    size_t                       GetIPCheckNum() const { return m_nIPCheckNum; }
    void                         SetIPCheckNum(size_t val) { m_nIPCheckNum = val; }
    MPSCQueue<CNetworkMessage*>& _GetMessageQueue() { return m_MessageQueue; }

public:
    static void accept_conn_cb(evconnlistener*, int32_t fd, struct sockaddr* addr, int32_t socklen, void* arg);
    static void accept_error_cb(struct evconnlistener*, void* arg);
    static void http_process_cb(struct evhttp_request* req, void* arg);

    void OnAccept(int32_t fd, struct sockaddr* addr, int32_t, evconnlistener* listener);

public:
    event_base*    GetEVBase() const { return m_pBase; }
    CEventManager* GetEventManager() const { return m_pEventManager.get(); }
    size_t         GetSocketAmount();
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
    void _AddSocket(CNetSocket* pSocket);
    void _CloseSocket(uint32_t nSocketIdx);
    void _AddConnectingSocket(CNetSocket* pSocket);
    void _RemoveSocket(CNetSocket* pSocket);
    void _AddCloseingSocket(CNetSocket* pSocket);
    void JoinIOThread();

private:
    void _ProceseCloseingSocket();

protected:
    event_base*                                     m_pBase;
    std::map<evconnlistener*, CNetEventHandler*>    m_setListener;
    struct evhttp*                                  m_pHttpHandle = nullptr;
    std::function<void(struct evhttp_request* req)> m_funcOnReciveHttp;
    std::mutex                                      m_mutex;

    std::unique_ptr<CEventManager> m_pEventManager;

    std::map<SOCKET, CNetSocket*>   m_setSocket;
    std::deque<SocketIdx_t>         m_SocketIdxPool;
    std::array<CNetSocket*, 0xFFFF> m_setSocketByIdx;

    std::unordered_set<CNetSocket*> m_setConnectingSocket;
    std::unordered_set<CNetSocket*> m_setCloseingSocket;

    MPSCQueue<CNetworkMessage*> m_MessageQueue;

    std::unique_ptr<std::thread> m_pIOThread;

    struct event*     m_pIOTimeOutEvent                 = nullptr;
    struct event*     m_pCloseSocketEvent               = nullptr;
    std::atomic<bool> m_bWaitingProcessCloseSocketEvent = false;

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
