#include "NetworkService.h"

#include <iostream>

#include "NetClientSocket.h"
#include "NetEventHandler.h"
#include "NetServerSocket.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/http.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "event2/util.h"

void log_cb(int32_t severity, const char* msg)
{
    LOGNETERROR("libevent：{}", msg);
}


void InitGlobal()
{
    
#ifdef WIN32
    WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
#endif
    event_set_log_callback(log_cb);
    event_enable_debug_mode();
#ifdef WIN32
    evthread_use_windows_threads();
#else
    evthread_use_pthreads();
#endif
    
}
std::once_flag g_init_network_global;

CNetworkService::CNetworkService()
{
    std::call_once(g_init_network_global, &InitGlobal);
   
    m_SocketIdxPool.resize(MAX_SOCKET_IDX, 0);
    for(size_t i = 0; i < m_SocketIdxPool.size(); i++)
    {
        m_SocketIdxPool[i]  = i + 1;
        m_setSocketByIdx[i] = nullptr;
    }
}

CNetworkService::~CNetworkService()
{
    Destroy();
}

void CNetworkService::Destroy()
{
    __ENTER_FUNCTION
    if(m_bStop == true)
        return;

    BreakLoop();
    m_bStop = true;

    JoinIOThread();
    if(m_pIOTimeOutEvent)
    {
        event_del(m_pIOTimeOutEvent);
        event_free(m_pIOTimeOutEvent);
        m_pIOTimeOutEvent = nullptr;
    }

    if(m_pCloseSocketEvent)
    {
        event_del(m_pCloseSocketEvent);
        event_free(m_pCloseSocketEvent);
        m_pCloseSocketEvent = nullptr;
    }
    for(auto& v: m_setSocketByIdx)
    {
        v.reset();
    }

    for(auto& v: m_setListener)
    {
        evconnlistener_free(v.first);
    }
    m_setListener.clear();
    for(auto& v: m_setEvTimed)
    {
        event_free(v);
    }
    m_setEvTimed.clear();

    m_setSocket.clear();
    m_setConnectingSocket.clear();

    _ProceseClosingSocket();

    if(m_pHttpHandle != nullptr)
    {
        evhttp_free(m_pHttpHandle);
        m_pHttpHandle = nullptr;
    }

    if(m_pBase)
    {
        event_base_free(m_pBase);
        m_pBase = nullptr;
    }

    CNetworkMessage* pMsg;
    while(m_MessageQueue.get(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    __LEAVE_FUNCTION
}

static void clock_socket_cb(evutil_socket_t, short, void* ctx) 
{
    CNetworkService* pThis = (CNetworkService*)ctx;
    pThis->_ProceseClosingSocket();
}


static void _IOThreadTimeOut(evutil_socket_t, short, void* ctx)
{
    __ENTER_FUNCTION
    CNetworkService* pThis = (CNetworkService*)ctx;
    pThis->OnIOThreadTimeOut();
    __LEAVE_FUNCTION
}

bool CNetworkService::Init(std::function<void()>&& time_out_func, uint32_t time_out_ms)
{
    __ENTER_FUNCTION
    CHECKF(m_pBase == nullptr);

    m_pBase = event_base_new();
    m_IOThreadTimeOutFunc = std::move(time_out_func);
    m_pIOTimeOutEvent     = event_new(m_pBase, -1, EV_PERSIST, &_IOThreadTimeOut, this);
    m_pCloseSocketEvent   = event_new(m_pBase, -1, 0, &clock_socket_cb, this);
    struct timeval tv
    {
        time_out_ms / 1000, time_out_ms % 1000
    };
    event_add(m_pIOTimeOutEvent, &tv);

    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CNetworkService::EnableListener(evconnlistener* listener, bool bEnable)
{
    __ENTER_FUNCTION
    if(listener)
    {
        if(bEnable)
            return evconnlistener_enable(listener) == 0;
        else
            return evconnlistener_disable(listener) == 0;
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_mutexListener);
        for(const auto& pair_val: m_setListener)
        {
            if(bEnable)
                evconnlistener_enable(pair_val.first);
            else
                evconnlistener_disable(pair_val.first);
        }
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

void CNetworkService::OnReciveHttp(struct evhttp_request* req)
{
    m_funcOnReciveHttp(req);
}

static void http_process_cb(struct evhttp_request* req, void* arg)
{
    __ENTER_FUNCTION
    CNetworkService* pThis = (CNetworkService*)arg;
    pThis->OnReciveHttp(req);
    __LEAVE_FUNCTION
}


bool CNetworkService::ListenHttpPort(const std::string& addr, int32_t port, std::function<void(struct evhttp_request* req)>&& func)
{
    __ENTER_FUNCTION
    if(m_pHttpHandle != nullptr)
        evhttp_free(m_pHttpHandle);

    m_pHttpHandle = evhttp_new(m_pBase);
    if(m_pHttpHandle == nullptr)
        return false;

    int32_t ret = evhttp_bind_socket(m_pHttpHandle, addr.c_str(), port);
    if(ret != 0)
    {
        return false;
    }

    m_funcOnReciveHttp = std::move(func);
    evhttp_set_gencb(m_pHttpHandle, http_process_cb, this);
    return true;
    __LEAVE_FUNCTION
    return false;
}


static void accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* addr, int32_t socklen, void* arg)
{
    __ENTER_FUNCTION

    static_cast<CNetworkService*>(arg)->OnAccept(fd, addr, socklen, listener);

    __LEAVE_FUNCTION
}


static void accept_error_cb(struct evconnlistener* listener, void* arg)
{
    __ENTER_FUNCTION
    // struct event_base* base   = evconnlistener_get_base(listener);
    int32_t     err    = EVUTIL_SOCKET_ERROR();
    const char* errstr = evutil_socket_error_to_string(err);

    // log error
    LOGNETERROR("CNetworkService::accept_error_cb {}", errstr);
    __LEAVE_FUNCTION
}

evconnlistener* CNetworkService::Listen(const std::string& addr, int32_t port, const CNetEventHandlerSharedPtr& pEventHandler)
{
    __ENTER_FUNCTION
    struct evutil_addrinfo  hits;
    struct evutil_addrinfo* answer = nullptr;
    memset(&hits, 0, sizeof(hits));
    hits.ai_family   = AF_UNSPEC;
    hits.ai_socktype = SOCK_STREAM;
    hits.ai_protocol = IPPROTO_TCP;
    hits.ai_flags    = EVUTIL_AI_ADDRCONFIG;

    if(evutil_getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hits, &answer) != 0)
    {
        LOGNETERROR("CNetworkService::Listen:{}:{} evutil_getaddrinfo fail", addr, port);
        return nullptr;
    }
    std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

    evconnlistener* pListener = evconnlistener_new_bind(m_pBase,
                                                        &accept_conn_cb,
                                                        this,
                                                        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE,
                                                        -1,
                                                        answer_ptr->ai_addr,
                                                        answer_ptr->ai_addrlen);
    if(pListener == nullptr)
    {
        LOGNETERROR("CNetworkService::Listen:{}:{} evconnlistener_new_bind fail:{}", addr, port, strerror(errno));
        return nullptr;
    }
    evconnlistener_set_error_cb(pListener, &accept_error_cb);
    {
        std::lock_guard<std::mutex> lock(m_mutexListener);
        m_setListener.emplace(pListener, pEventHandler);
    }
    LOGNETDEBUG("CNetworkService::Listen:{}:{}", addr, port);

    return pListener;
    __LEAVE_FUNCTION
    return nullptr;
}

CServerSocketWeakPtr CNetworkService::ConnectTo(const std::string& addr, int32_t port, const CNetEventHandlerSharedPtr& pEventHandler, bool bAutoReconnect)
{
    __ENTER_FUNCTION
    struct evutil_addrinfo  hits;
    struct evutil_addrinfo* answer = nullptr;
    memset(&hits, 0, sizeof(hits));
    hits.ai_family   = AF_UNSPEC;
    hits.ai_socktype = SOCK_STREAM;
    hits.ai_protocol = IPPROTO_TCP;
    hits.ai_flags    = EVUTIL_AI_ADDRCONFIG;

    if(evutil_getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hits, &answer) != 0)
    {
        LOGNETERROR("CNetworkService::ConnectTo:{}:{} evutil_getaddrinfo fail", addr, port);
        return {};
    }
    std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

    auto fd = socket(answer_ptr->ai_family, answer_ptr->ai_socktype, answer_ptr->ai_protocol);
    if(fd < 0)
    {
        LOGNETERROR("CNetworkService::ConnectTo:{}:{} socket fail", addr, port);
        return {};
    }
    if(connect(fd, answer_ptr->ai_addr, answer_ptr->ai_addrlen))
    {
        LOGNETERROR("CNetworkService::ConnectTo:{}:{} connect fail", addr, port);
        evutil_closesocket(fd);
        return {};
    }

    bufferevent* pBufferEvent = bufferevent_socket_new(m_pBase, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if(pBufferEvent == nullptr)
    {
        LOGNETERROR("CNetworkService::ConnectTo:{}:{} bufferevent_socket_new fail", addr, port);
        return {};
    }

    CServerSocketSharedPtr pSocket = CreateServerSocket(pEventHandler, bAutoReconnect);
    CHECK_RET(pSocket, pSocket);
    pSocket->SetAddrAndPort(addr, port);
    pSocket->SetSocket(fd);
    pSocket->Init(pBufferEvent);
    pSocket->OnStartConnect();
    _AddSocket(pSocket);
    pSocket->OnConnected();
    LOGNETDEBUG("CNetworkService::ConnectTo:{}:{}", addr, port);

    return pSocket;
    __LEAVE_FUNCTION
    return {};
}

CServerSocketWeakPtr CNetworkService::AsyncConnectTo(const std::string&               addr,
                                                     int32_t                          port,
                                                     const CNetEventHandlerSharedPtr& pEventHandler,
                                                     bool                             bAutoReconnect)
{
    __ENTER_FUNCTION
    struct evutil_addrinfo  hits;
    struct evutil_addrinfo* answer = nullptr;
    memset(&hits, 0, sizeof(hits));
    hits.ai_family   = AF_UNSPEC;
    hits.ai_socktype = SOCK_STREAM;
    hits.ai_protocol = IPPROTO_TCP;
    hits.ai_flags    = EVUTIL_AI_ADDRCONFIG;

    if(evutil_getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hits, &answer) != 0)
    {
        LOGNETERROR("CNetworkService::AsyncConnectTo:{}:{} evutil_getaddrinfo fail", addr, port);

        return {};
    }
    std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

    bufferevent* pBufferEvent = bufferevent_socket_new(m_pBase, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if(pBufferEvent == nullptr)
    {
        LOGNETERROR("CNetworkService::AsyncConnectTo:{}:{} bufferevent_socket_new fail", addr, port);

        return {};
    }

    CServerSocketSharedPtr pSocket = CreateServerSocket(pEventHandler, bAutoReconnect);
    CHECK_RET(pSocket, pSocket);
    pSocket->SetAddrAndPort(addr, port);
    pSocket->Init(pBufferEvent);
    pSocket->OnStartConnect();
    _AddConnectingSocket(pSocket);

    if(bufferevent_socket_connect(pBufferEvent, answer_ptr->ai_addr, answer_ptr->ai_addrlen) != 0)
    {
        LOGNETERROR("CNetworkService::AsyncConnectTo:{}:{} bufferevent_socket_connect fail", addr, port);
        pSocket->OnConnectFailed();
        _RemoveSocket(pSocket);
        _ReleaseSocket(pSocket);
        bufferevent_free(pBufferEvent);
        return {};
    }

    LOGNETDEBUG("CNetworkService::AsyncConnectTo:{}:{}", addr, port);

    return pSocket;
    __LEAVE_FUNCTION
    return {};
}

bool CNetworkService::_AsyncReconnect(const CServerSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    struct evutil_addrinfo  hits;
    struct evutil_addrinfo* answer = nullptr;
    memset(&hits, 0, sizeof(hits));
    hits.ai_family   = AF_UNSPEC;
    hits.ai_socktype = SOCK_STREAM;
    hits.ai_protocol = IPPROTO_TCP;
    hits.ai_flags    = EVUTIL_AI_ADDRCONFIG;

    if(evutil_getaddrinfo(pSocket->GetAddrString().c_str(), std::to_string(pSocket->GetPort()).c_str(), &hits, &answer) != 0)
    {
        LOGNETERROR("CNetworkService::_AsyncReconnect:{}:{} evutil_getaddrinfo fail", pSocket->GetAddrString(), pSocket->GetPort());

        return false;
    }
    std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);
    pSocket->Init(pSocket->GetBufferevent());
    if(bufferevent_socket_connect(pSocket->GetBufferevent(), answer_ptr->ai_addr, answer_ptr->ai_addrlen) != 0)
    {
        int32_t     err    = EVUTIL_SOCKET_ERROR();
        const char* errstr = evutil_socket_error_to_string(err);

        LOGNETERROR("CNetworkService::_AsyncReconnect:{}:{} bufferevent_socket_connect fail:{}",
                    pSocket->GetAddrString(),
                    pSocket->GetPort(),
                    errstr);
        return false;
    }
    LOGNETDEBUG("CNetworkService::_AsyncReconnect:{}:{}", pSocket->GetAddrString(), pSocket->GetPort());

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CNetworkService::BreakLoop()
{
    __ENTER_FUNCTION
    event_base_loopbreak(m_pBase);
    __LEAVE_FUNCTION
}

void CNetworkService::OnIOThreadTimeOut()
{
    __ENTER_FUNCTION
    if(m_IOThreadTimeOutFunc)
        m_IOThreadTimeOutFunc();
    __LEAVE_FUNCTION
}

static void IOThreadProc(event_base* pBase, const std::string& _thread_name, const ServiceID& idService)
{
    __ENTER_FUNCTION
    #ifdef __linux__
    pthread_setname_np(pthread_self(), _thread_name.c_str());
    #endif
    BaseCode::SetNdc(_thread_name);
    LOGDEBUG("ThreadCreate:{} ThreadID:{}", _thread_name, get_cur_thread_id());
    int32_t result = 0;
    do
    {
        result = event_base_dispatch(pBase);
        if(event_base_got_break(pBase) || event_base_got_exit(pBase))
            break;
    } while(result != -1);
    if(result == -1)
    {
        // showerror

        LOGNETERROR("CNetworkService {} IOThread Close with ERROR:", idService);
    }
    LOGNETDEBUG("CNetworkService IOThread Close:{}", idService);
    LOGDEBUG("ThreadExit:{} ThreadID:{}", _thread_name, get_cur_thread_id());
    BaseCode::ClearNdc();
    __LEAVE_FUNCTION
}

void CNetworkService::StartIOThread(const std::string& thread_name, const ServiceID& idService)
{
    __ENTER_FUNCTION
    if(m_pIOThread)
    {
        return;
    }
    m_pIOThread = std::make_unique<std::thread>(&IOThreadProc, m_pBase, thread_name, idService);

    __LEAVE_FUNCTION
}

void CNetworkService::RunOnce()
{
    __ENTER_FUNCTION
    if(m_pIOThread)
    {
        return;
    }

    event_base_loop(m_pBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);

    __LEAVE_FUNCTION
}

CServerSocketSharedPtr CNetworkService::CreateServerSocket(const CNetEventHandlerSharedPtr& pHandle, bool bAutoReconnect)
{
    auto pSocket = std::make_shared<CServerSocket>(this, pHandle, bAutoReconnect);
    CHECKF(pSocket);
    CHECKF(_AllocSocketIdx(pSocket));
    if(pHandle)
        pHandle->OnBindSocket(pSocket);
    return pSocket;
}
CClientSocketSharedPtr CNetworkService::CreateClientSocket(const CNetEventHandlerSharedPtr& pHandle)
{
    auto pSocket = std::make_shared<CClientSocket>(this, pHandle);
    CHECKF(pSocket);
    CHECKF(_AllocSocketIdx(pSocket));
    if(pHandle)
        pHandle->OnBindSocket(pSocket);
    return pSocket;
}



void CNetworkService::OnAccept(evutil_socket_t fd, sockaddr* addr, int32_t, evconnlistener* listener)
{
    __ENTER_FUNCTION
    char           szHost[512] = {};
    unsigned short uPort       = 0;

    if(addr)
    {
        sockaddr_in* addr_in = (sockaddr_in*)addr;
        evutil_inet_ntop(addr->sa_family, &addr_in->sin_addr, szHost, 512);
        uPort = ntohs(addr_in->sin_port);
    }

    if(m_bIPCheck)
    {
        auto&  refData = m_setIP[szHost];
        time_t now     = TimeGetSecond();
        if(now > refData.nextResetTime)
        {
            refData.nextResetTime = now + 60;
            refData.nCount        = 1;
        }
        else
        {
            refData.nCount++;
        }

        if(refData.nCount > m_nIPCheckNum)
        {
            evutil_closesocket(fd);
            return;
        }
    }
    bufferevent* pBufferEvent = bufferevent_socket_new(GetEVBase(), fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if(pBufferEvent == nullptr)
    {
        LOGNETERROR("CNetworkService::OnAccept bufferevent_socket_new fail:{}:{} ", szHost, uPort);
        evutil_closesocket(fd);
        return;
    }
    CNetEventHandlerSharedPtr pEventHandler = QueryListenerEventHander(listener);
    CClientSocketSharedPtr    pSocket       = CreateClientSocket(pEventHandler);
    CHECK(pSocket);
    pSocket->SetAddrAndPort(szHost, uPort);
    pSocket->SetSocket(fd);

    pSocket->Init(pBufferEvent);

    _AddSocket(pSocket);
    pSocket->OnAccepted();

    LOGNETDEBUG("CNetworkService::OnAccept:{}:{}", szHost, uPort);
    __LEAVE_FUNCTION
}

CNetEventHandlerSharedPtr CNetworkService::QueryListenerEventHander(evconnlistener* listener)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutexListener);
    auto                        it_find = m_setListener.find(listener);
    if(it_find != m_setListener.end())
        return it_find->second.lock();
    __LEAVE_FUNCTION
    return {};
}

CNetSocketSharedPtr CNetworkService::QuerySocketByIdx(uint16_t nSocketIdx)
{
    __ENTER_FUNCTION
    if(nSocketIdx == INVALID_SOCKET_IDX)
        return nullptr;
    std::lock_guard<std::mutex> lock(m_mutex);
    CHECKF_V(nSocketIdx < m_setSocketByIdx.size(), nSocketIdx);
    return m_setSocketByIdx[nSocketIdx];
    __LEAVE_FUNCTION
    return nullptr;
}

bool CNetworkService::_AllocSocketIdx(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);
    CHECKF(m_SocketIdxPool.empty() == false);
    auto nSocketIdx = m_SocketIdxPool.front();
    m_SocketIdxPool.pop_front();
    m_setSocketByIdx[nSocketIdx] = pSocket;
    pSocket->SetSocketIdx(nSocketIdx);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CNetworkService::_ReleaseSocketIdx(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    CHECK(pSocket);
    CHECK(pSocket->GetSocketIdx() != INVALID_SOCKET_IDX);
    std::lock_guard<std::mutex> lock(m_mutex);

    m_SocketIdxPool.push_back(pSocket->GetSocketIdx());
    m_setSocketByIdx[pSocket->GetSocketIdx()] = nullptr;
    pSocket->SetSocketIdx(INVALID_SOCKET_IDX);

    __LEAVE_FUNCTION
}

void CNetworkService::_AddSocket(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    CHECK(pSocket);
    CHECK(pSocket->GetSocket() != INVALID_SOCKET);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_setConnectingSocket.erase(pSocket);
    m_setSocket[pSocket->GetSocket()] = pSocket;

    __LEAVE_FUNCTION
}

void CNetworkService::_AddConnectingSocket(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    CHECK(pSocket);
    CHECK(pSocket->GetSocket() == INVALID_SOCKET);
    std::lock_guard<std::mutex> lock(m_mutex);
    m_setSocket.erase(pSocket->GetSocket());
    m_setConnectingSocket.insert(pSocket);
    __LEAVE_FUNCTION
}

void CNetworkService::_ReleaseSocket(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    pSocket->DetachEventHandler();
    _ReleaseSocketIdx(pSocket);
    __LEAVE_FUNCTION
}

void CNetworkService::_RemoveSocket(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    CHECK(pSocket);
    std::lock_guard<std::mutex> lock(m_mutex);
    m_setConnectingSocket.erase(pSocket);
    if(pSocket->GetSocket() != INVALID_SOCKET)
    {
        m_setSocket.erase(pSocket->GetSocket());
    }

    __LEAVE_FUNCTION
}

void CNetworkService::_AddClosingSocket(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    CHECK(pSocket);
    m_setClosingSocket.push(pSocket);

    if(m_bWaitingProcessCloseSocketEvent == false)
    {
        // add a remove event
        m_bWaitingProcessCloseSocketEvent = true;
        struct timeval tv
        {
            0, 0
        };
        evtimer_add(m_pCloseSocketEvent, &tv);
    }
    __LEAVE_FUNCTION
}

void CNetworkService::_ProceseClosingSocket()
{
    __ENTER_FUNCTION
    m_bWaitingProcessCloseSocketEvent = false;
    CNetSocketSharedPtr pSocket;
    while(m_setClosingSocket.pop(pSocket))
    {
        _ReleaseSocket(pSocket);
        pSocket.reset();
    }

    __LEAVE_FUNCTION
}

void CNetworkService::JoinIOThread()
{
    __ENTER_FUNCTION
    if(m_pIOThread)
    {
        m_pIOThread->join();
        m_pIOThread.reset();
    }
    __LEAVE_FUNCTION
}

void CNetworkService::_CloseSocket(uint32_t nSocketIdx)
{
    __ENTER_FUNCTION
    std::unique_lock<std::mutex> lock(m_mutex);

    auto pSocket = m_setSocketByIdx[nSocketIdx];
    if(pSocket)
    {
        lock.unlock();
        pSocket->Interrupt(true);
    }

    __LEAVE_FUNCTION
}

size_t CNetworkService::GetSocketAmount()
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_setSocket.size();
    __LEAVE_FUNCTION
    return 0;
}

void CNetworkService::BrocastMsg(const CNetworkMessage& msg, SOCKET execpt_this)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);

    for(auto it = m_setSocket.begin(); it != m_setSocket.end(); it++)
    {
        if(execpt_this != 0 && it->first == execpt_this)
            continue;

        it->second->SendNetworkMessage(msg, true);
    }
    __LEAVE_FUNCTION
}

bool CNetworkService::KickSocket(SOCKET _socket)
{
    __ENTER_FUNCTION
    CNetSocketSharedPtr pSocket;
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_setSocket.find(_socket);
        if(it != m_setSocket.end())
        {
            pSocket = it->second;
            if(pSocket == nullptr)
            {
                m_setSocket.erase(_socket);
                return false;
            }
        }
    }
    // must do it out of lock,because it will remove self from m_setSocket;
    if(pSocket)
    {
        pSocket->Interrupt(true);
        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CNetworkService::SendSocketMsg(SOCKET _socket, const CNetworkMessage& msg, bool bNeedDuplicate)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);
    auto                        it = m_setSocket.find(_socket);
    if(it != m_setSocket.end())
    {
        return it->second->SendNetworkMessage(msg, bNeedDuplicate);
    }
    __LEAVE_FUNCTION
    return false;
}

bool CNetworkService::SendSocketMsgByIdx(SocketIdx_t nSocketIdx, const CNetworkMessage& msg, bool bNeedDuplicate)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);

    auto pSocket = m_setSocketByIdx[nSocketIdx];
    if(pSocket)
    {
        return pSocket->SendNetworkMessage(msg, bNeedDuplicate);
    }
    __LEAVE_FUNCTION
    return false;
}

void CNetworkService::AddRecvByteCount(size_t len)
{
    __ENTER_FUNCTION
    m_RecvBPS.AddCount(len);
    __LEAVE_FUNCTION
}

void CNetworkService::AddSendByteCount(size_t len)
{
    __ENTER_FUNCTION
    m_SendBPS.AddCount(len);
    __LEAVE_FUNCTION
}
