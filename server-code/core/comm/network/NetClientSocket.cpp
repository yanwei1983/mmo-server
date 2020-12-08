#include "NetClientSocket.h"

#include <event2/bufferevent.h>
#include <event2/event.h>
#include "NetEventHandler.h"
#include "NetworkMessage.h"
#include "NetworkService.h"
#include "event2/buffer.h"
#include "event2/util.h"

CClientSocket::CClientSocket(CNetworkService* pService, const CNetEventHandlerSharedPtr& pEventHandler)
    : CNetSocket(pService, pEventHandler)
{
}

CClientSocket::~CClientSocket() {}

bool CClientSocket::Init(bufferevent* pBufferEvent)
{
    __ENTER_FUNCTION

    if(pBufferEvent)
    {
        bufferevent_setcb(pBufferEvent, _OnSocketRead, _OnSendOK, _OnSocketEvent, (void*)this);
        bufferevent_enable(pBufferEvent, EV_WRITE | EV_READ | EV_PERSIST);
        _SetTimeout();
    }
    {
        evutil_make_socket_nonblocking(m_socket);
        set_sock_nodely();
        set_sock_quickack();
    }
    m_pBufferevent = pBufferEvent;
    SetStatus(NSS_READY);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CClientSocket::Interrupt(bool bClearEventHandler)
{
    __ENTER_FUNCTION
    LOGNETDEBUG("CClientSocket Interrupt {}:{}", GetAddrString(), GetPort());
    if(bClearEventHandler)
    {
        if(auto event_handler = m_pEventHandler.lock())
            event_handler->OnUnbindSocket(shared_from_this());
        m_pEventHandler.reset();
    }
    if(GetStatus() == NSS_READY || GetStatus() == NSS_CONNECTING)
    {
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

void CClientSocket::_OnError(const std::string& what)
{
    __ENTER_FUNCTION
    LOGNETDEBUG("CServerSocket _OnError {}:{} err:{}", GetAddrString(), GetPort(), what);
    OnDisconnected();

    __LEAVE_FUNCTION
}

void CClientSocket::OnAccepted()
{
    __ENTER_FUNCTION
    if(auto event_handler = m_pEventHandler.lock())
            event_handler->OnAccepted(shared_from_this());
    __LEAVE_FUNCTION
}