#include "MessagePort.h"

#include "CheckUtil.h"
#include "MessageRoute.h"
#include "NetSocket.h"
#include "NetServerSocket.h"
#include "NetworkMessage.h"
#include "msg_internal.pb.h"

CMessagePort::CMessagePort() {}

bool CMessagePort::Init(const ServerPort& nServerPort, CMessageRoute* pRoute)
{
    m_nServerPort = nServerPort;
    m_pRoute      = pRoute;
    return true;
}

CMessagePort::~CMessagePort()
{
    __ENTER_FUNCTION
    Destory();
    __LEAVE_FUNCTION
}

void CMessagePort::Destory()
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg;
    while(TakePortMsg(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    while(m_SendMsgQueue.get(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    if(m_pRemoteServerSocket)
    {
        m_pRemoteServerSocket->Interrupt(true);
        DetachRemoteSocket();
    }
    m_pPortEventHandler = nullptr;
    m_Event.Cancel();
    m_Event.Clear();
    __LEAVE_FUNCTION
}

bool CMessagePort::TakePortMsg(CNetworkMessage*& msg)
{
    __ENTER_FUNCTION
    return m_RecvMsgQueue.get(msg);
    __LEAVE_FUNCTION
    return false;
}

void CMessagePort::OnConnected(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    MSG_HEAD msg;
    msg.msg_cmd  = COMMON_CMD_PING;
    msg.msg_size = sizeof(MSG_HEAD);
    pSocket->_SendMsg((byte*)&msg, sizeof(msg));
    //服务器间通信扩充recv缓冲区大小
    pSocket->SetPacketSizeMax(_MAX_MSGSIZE * 10);
    static_cast<CServerSocket*>(pSocket)->SetReconnect(true);
    pSocket->SetLogWriteHighWateMark(100 * 1024 * 1024);
    
    LOGNETDEBUG("MessagePort:{} OnConnected {}:{}",
                GetServerPort().GetServiceID(),
                pSocket->GetAddrString().c_str(),
                pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortConnected(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::OnConnectFailed(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    LOGNETINFO("MessagePort:{} OnConnectFailed {}:{}",
               GetServerPort().GetServiceID(),
               pSocket->GetAddrString().c_str(),
               pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortConnectFailed(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::OnDisconnected(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    LOGNETINFO("MessagePort:{} OnDisconnected {}:{}",
               GetServerPort().GetServiceID(),
               pSocket->GetAddrString().c_str(),
               pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortDisconnected(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::OnAccepted(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    LOGNETDEBUG("MessagePort:{} OnAccpet {}:{}",
                GetServerPort().GetServiceID(),
                pSocket->GetAddrString().c_str(),
                pSocket->GetPort());
    //服务器间通信扩充recv缓冲区大小
    pSocket->SetPacketSizeMax(_MAX_MSGSIZE * 10);
    pSocket->SetLogWriteHighWateMark(100 * 1024 * 1024);
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortAccepted(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::SetRemoteSocket(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    m_pRemoteServerSocket = pSocket;
    __LEAVE_FUNCTION
}

void CMessagePort::DetachRemoteSocket()
{
    if(m_pRemoteServerSocket)
    {
        m_pRemoteServerSocket->SetEventHandler(nullptr);
        m_pRemoteServerSocket = nullptr;
    }
}

void CMessagePort::OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    MSG_HEAD*   pHead = (MSG_HEAD*)pBuffer;
    InternalMsg internal_msg;
    byte*       pData    = pBuffer + sizeof(MSG_HEAD);
    size_t      data_len = len - sizeof(MSG_HEAD);
    if(internal_msg.ParseFromArray(pData, static_cast<int>(data_len)) == false)
    {
        LOGNETERROR("MessagePort:{} Recv a unknow cmd:{} size:{}",
                    GetServerPort().GetServiceID(),
                    pHead->msg_cmd,
                    pHead->msg_size);
        return;
    }

    CNetworkMessage* pMsg =
        new CNetworkMessage((byte*)internal_msg.proto_msg().c_str(), internal_msg.proto_msg().size(), internal_msg.from(), internal_msg.to());
    pMsg->SetForward(internal_msg.forward().data(), internal_msg.forward_size());
    pMsg->SetMultiTo(internal_msg.multi_vs().data(), internal_msg.multi_vs_size());
    pMsg->SetMultiTo(internal_msg.multi_id().data(), internal_msg.multi_id_size());
    for(const auto& v: internal_msg.broadcast_to())
    {
        pMsg->AddBroadcastTo(v);
    }
    pMsg->SetBroadcastType(static_cast<BroadcastType>(internal_msg.brocast_type()));
    pMsg->CopyBuffer();
    m_RecvMsgQueue.push(pMsg);

    auto pHandler = m_pPortEventHandler.load();
    if(pHandler)
    {
        pHandler->OnPortRecvData(pSocket, pBuffer, len);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::OnRecvTimeout(CNetSocket* pSocket)
{
    auto pHandler = m_pPortEventHandler.load();
    if(pHandler)
    {
        pHandler->OnPortRecvTimeout(pSocket);
    }
}

size_t CMessagePort::GetWriteBufferSize()
{
    __ENTER_FUNCTION
    if(m_pRemoteServerSocket)
    {
        size_t nNeedWrite = m_pRemoteServerSocket->GetWaitWriteSize();
        return nNeedWrite;
    }
    __LEAVE_FUNCTION
    return 0;
}

void CMessagePort::PostSend()
{
    __ENTER_FUNCTION
    if(m_Event.IsRunning() == false)
    {
        CEventEntryCreateParam param;
        param.cb = std::bind(&CMessagePort::_SendAllMsg, this);
        m_pRoute->GetEventManager()->ScheduleEvent(param, m_Event);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::_SendAllMsg()
{
    __ENTER_FUNCTION
    CHECK(m_pRemoteServerSocket);
    CNetworkMessage* pMsg;
    while(m_SendMsgQueue.get(pMsg))
    {
        __ENTER_FUNCTION
        InternalMsg internal_msg;
        internal_msg.set_from(pMsg->GetFrom());
        internal_msg.set_to(pMsg->GetTo());
        internal_msg.set_proto_msg(pMsg->GetBuf(), pMsg->GetSize());

        for(const auto& v: pMsg->GetForward())
        {
            internal_msg.add_forward(v);
        }
        for(const auto& v: pMsg->GetMultiTo())
        {
            internal_msg.add_multi_vs(v);
        }
        for(const auto& v: pMsg->GetMultiIDTo())
        {
            internal_msg.add_multi_id(v);
        }
        for(const auto& v: pMsg->GetMultiIDTo())
        {
            internal_msg.add_multi_id(v);
        }
        for(const auto& v: pMsg->GetBroadcastTo())
        {
            internal_msg.add_broadcast_to(v);
        }
        internal_msg.set_brocast_type(pMsg->GetBroadcastType());

        CNetworkMessage send_msg(0xFFFF, internal_msg);

        m_pRemoteServerSocket->SendNetworkMessage(std::move(send_msg));

        __LEAVE_FUNCTION
        SAFE_DELETE(pMsg);
    }
    __LEAVE_FUNCTION
}

bool CMessagePort::SendMsgToPort(const CNetworkMessage& msg)
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg = new CNetworkMessage(msg);
    pMsg->CopyBuffer();

    if(m_bLocalPort == false)
    {
        m_SendMsgQueue.push(pMsg);
        PostSend();
        return true;
    }
    else
    {
        m_RecvMsgQueue.push(pMsg);
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}
