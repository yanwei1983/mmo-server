#include "NetServerSocket.h"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include "NetEventHandler.h"
#include "NetworkMessage.h"
#include "NetworkService.h"
#include "event2/buffer.h"
#include "event2/util.h"

CServerSocket::CServerSocket(CNetworkService* pService, const CNetEventHandlerSharedPtr& pEventHandler, bool bReconnect)
    : CNetSocket(pService, pEventHandler)
    , m_bReconnect(bReconnect)
    , m_pReconnectEvent(nullptr)
    , m_nReconnectTimes(-1)
{
}

CServerSocket::~CServerSocket()
{
    if(m_pReconnectEvent)
    {
        event_del(m_pReconnectEvent);
        event_free(m_pReconnectEvent);
        m_pReconnectEvent = nullptr;
    }
}

bool CServerSocket::Init(bufferevent* pBufferEvent)
{
    __ENTER_FUNCTION

    if(pBufferEvent)
    {
        bufferevent_setcb(pBufferEvent, NULL, NULL, _OnSocketConnectorEvent, (void*)this);
        bufferevent_enable(pBufferEvent, EV_WRITE | EV_READ | EV_PERSIST);
        _SetTimeout();
    }
    m_pBufferevent = pBufferEvent;
    SetStatus(NSS_CONNECTING);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CServerSocket::Interrupt(bool bClearEventHandler)
{
    __ENTER_FUNCTION
    LOGNETDEBUG("CServerSocket Interrupt {}:{}", GetAddrString(), GetPort());
    if(bClearEventHandler)
    {
        if(auto event_handler = m_pEventHandler.lock())
            event_handler->OnUnbindSocket(shared_from_this());
        m_pEventHandler.reset();
    }
    if(GetStatus() == NSS_READY || GetStatus() == NSS_CONNECTING)
    {
        SetReconnect(false);
        bufferevent_disable(m_pBufferevent, EV_READ);
        bufferevent_setcb(m_pBufferevent, nullptr, _OnCheckAllSendOK, _OnSocketEvent, this);

        MSG_HEAD msg;
        msg.msg_cmd  = COMMON_CMD_INTERRUPT;
        msg.msg_size = sizeof(MSG_HEAD);
        _SendMsg((byte*)&msg, sizeof(msg));
        SetStatus(NSS_CLOSEING);
    }
    __LEAVE_FUNCTION
}

void CServerSocket::_OnError(const std::string& what)
{
    __ENTER_FUNCTION
    if(GetStatus() == NSS_CLOSEING)
    {
        OnDisconnected();
        return;
    }
    SetStatus(NSS_CLOSEING);

    LOGNETDEBUG("CServerSocket _OnError {}:{} err:{}", GetAddrString(), GetPort(), what);

    if(m_nReconnectTimes > 0 && m_bReconnect)
    {
        m_pService->_RemoveSocket(shared_from_this());
        if(m_pBufferevent)
        {
            bufferevent_setfd(m_pBufferevent, INVALID_SOCKET);
            auto pOutBuf = bufferevent_get_output(m_pBufferevent);
            evbuffer_drain(pOutBuf, evbuffer_get_length(pOutBuf));
        }
        if(GetSocket() != INVALID_SOCKET)
            evutil_closesocket(m_socket);
        SetSocket(INVALID_SOCKET);

        m_nReconnectTimes--;
        if(m_pReconnectEvent == nullptr)
            m_pReconnectEvent = event_new(m_pService->GetEVBase(), -1, 0, _OnReconnect, this);
        struct timeval reconnet_dealy = {5, 0};
        event_add(m_pReconnectEvent, &reconnet_dealy);
        SetStatus(NSS_WAIT_RECONNECT);
        m_pService->_AddConnectingSocket(shared_from_this());
        if(auto event_handler = m_pEventHandler.lock())
            event_handler->OnWaitReconnect(shared_from_this());
        LOGNETDEBUG("CServerSocket _OnError Reconnect Wait,5s, {}:{}", GetAddrString(), GetPort());
    }
    else
    {
        if(m_pBufferevent)
        {
            bufferevent_disable(m_pBufferevent, EV_READ | EV_PERSIST);
        }

        OnClosing();
    }
    __LEAVE_FUNCTION
}

void CServerSocket::_OnReconnect(int32_t fd, short what, void* ctx)
{
    __ENTER_FUNCTION

    CServerSocket* pSocket       = (CServerSocket*)ctx;
    auto           shared_socket = pSocket->shared_from_this();
    LOGNETDEBUG("CServerSocket Reconnect:{}::{}", pSocket->GetAddrString(), pSocket->GetPort());

    if(pSocket->GetService()->_AsyncReconnect(std::static_pointer_cast<CServerSocket>(shared_socket)) == false)
    {
        pSocket->OnDisconnected();
    }
    __LEAVE_FUNCTION
}

void CServerSocket::_OnSocketConnectorEvent(bufferevent* b, short what, void* ctx)
{
    __ENTER_FUNCTION
    CServerSocket* pSocket       = (CServerSocket*)ctx;
    auto           shared_socket = pSocket->shared_from_this();
    if(what == BEV_EVENT_CONNECTED)
    {
        int32_t fd = bufferevent_getfd(b);
        evutil_make_socket_nonblocking(fd);
        pSocket->set_sock_nodely();
        pSocket->set_sock_quickack();
        pSocket->SetSocket(fd);
        pSocket->GetService()->_AddSocket(shared_socket);
        bufferevent_setcb(b, _OnSocketRead, _OnSendOK, _OnSocketEvent, ctx);
        pSocket->SetStatus(NSS_READY);
        pSocket->OnConnected();
        bufferevent_enable(b, EV_READ | EV_WRITE | EV_PERSIST);
        pSocket->_SetTimeout();
        LOGNETDEBUG("CServerSocket::SocketConnectSucc:{}:{}  SendWait:{}", pSocket->GetAddrString(), pSocket->GetPort(), pSocket->m_nWaitWriteSize);

        bufferevent_write_buffer(b, pSocket->m_Sendbuf);
    }
    else
    {
        int32_t     err    = evutil_socket_geterror(bufferevent_getfd(b));
        const char* errstr = evutil_socket_error_to_string(err);
        LOGNETDEBUG("CServerSocket::SocketConnectFail:{}:{} {}", pSocket->GetAddrString(), pSocket->GetPort(), errstr);

        pSocket->OnConnectFailed();
    }
    __LEAVE_FUNCTION
}

void CServerSocket::OnRecvTimeout(bool& bReconnect)
{
    __ENTER_FUNCTION
    if(auto event_handler = m_pEventHandler.lock())
        event_handler->OnRecvTimeout(shared_from_this());

    bReconnect = m_bReconnect;
    __LEAVE_FUNCTION
}

void CServerSocket::OnStartConnect()
{
    __ENTER_FUNCTION

    if(auto event_handler = m_pEventHandler.lock())
        event_handler->OnStartConnect(shared_from_this());
    __LEAVE_FUNCTION
}

void CServerSocket::OnConnected()
{
    __ENTER_FUNCTION
    if(auto event_handler = m_pEventHandler.lock())
        event_handler->OnConnected(shared_from_this());
    __LEAVE_FUNCTION
}

void CServerSocket::OnConnectFailed()
{
    __ENTER_FUNCTION
    if(auto event_handler = m_pEventHandler.lock())
        event_handler->OnConnectFailed(shared_from_this());

    _OnError("AsyncConnectFailed");
    __LEAVE_FUNCTION
}

void CServerSocket::OnRecvData(byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    MSG_HEAD* pHeader = (MSG_HEAD*)pBuffer;
    if(pHeader->msg_cmd == COMMON_CMD_INTERRUPT)
    {
        //对端主动断开,不要重连了
        SetReconnectTimes(0);
        SetReconnect(false);
    }
    CNetSocket::OnRecvData(pBuffer, len);
    __LEAVE_FUNCTION
}