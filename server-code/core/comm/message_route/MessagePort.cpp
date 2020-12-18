#include "MessagePort.h"

#include "CheckUtil.h"
#include "MessageRoute.h"
#include "NetServerSocket.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "NetworkService.h"
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
    Destroy();
    __LEAVE_FUNCTION
}

void CMessagePort::Destroy()
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg;
    while(TakePortMsg(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    if(m_SocketIdxList.empty() == false)
    {
        auto socket_idx_list_copy = m_SocketIdxList;
        m_SocketIdxList.clear();
        for(auto socket_idx: socket_idx_list_copy)
        {
            if(socket_idx != INVALID_SOCKET_IDX)
            {
                m_pRoute->GetNetworkService()->_CloseSocket(socket_idx);
            }
        }
    }

    m_pRemoteSocket.reset();
    m_pPortEventHandler = nullptr;
    __LEAVE_FUNCTION
}

bool CMessagePort::TakePortMsg(CNetworkMessage*& msg)
{
    __ENTER_FUNCTION
    return m_RecvMsgQueue.get(msg);
    __LEAVE_FUNCTION
    return false;
}

void CMessagePort::OnBindSocket(const CNetSocketSharedPtr& pSocket)
{
    m_SocketIdxList.insert(pSocket->GetSocketIdx());
}

void CMessagePort::OnUnbindSocket(const CNetSocketSharedPtr& pSocket)
{
    m_SocketIdxList.erase(pSocket->GetSocketIdx());
}

void CMessagePort::OnStartConnect(const CNetSocketSharedPtr& pSocket)
{
    SetRemoteSocket(pSocket);
}

void CMessagePort::OnConnected(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    MSG_HEAD msg;
    msg.msg_cmd  = COMMON_CMD_PING;
    msg.msg_size = sizeof(MSG_HEAD);
    pSocket->_SendMsg((byte*)&msg, sizeof(msg));
    //服务器间通信扩充recv缓冲区大小
    pSocket->SetLogWriteHighWateMark(100 * 1024 * 1024);

    LOGNETDEBUG("MessagePort:{} OnConnected {}:{}", GetServerPort().GetServiceID(), pSocket->GetAddrString(), pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortConnected(pSocket);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::OnConnectFailed(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    LOGNETINFO("MessagePort:{} OnConnectFailed {}:{}", GetServerPort().GetServiceID(), pSocket->GetAddrString(), pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortConnectFailed(pSocket);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::OnDisconnected(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    LOGNETINFO("MessagePort:{} OnDisconnected {}:{}", GetServerPort().GetServiceID(), pSocket->GetAddrString(), pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortDisconnected(pSocket);
    }
    if(m_pRemoteSocket.expired() == false)
    {
        auto remove_shared_ptr = m_pRemoteSocket.lock();
        if(!remove_shared_ptr || remove_shared_ptr == pSocket)
            m_pRemoteSocket.reset();
    }
    __LEAVE_FUNCTION
}
void CMessagePort::OnWaitReconnect(const CNetSocketSharedPtr& pSocket)
{
    LOGNETTRACE("MessagePort:{} OnWaitReconnect {}:{}", GetServerPort().GetServiceID(), pSocket->GetAddrString(), pSocket->GetPort());
}

void CMessagePort::OnClosing(const CNetSocketSharedPtr& pSocket)
{
    LOGNETTRACE("MessagePort:{} OnClosing {}:{}", GetServerPort().GetServiceID(), pSocket->GetAddrString(), pSocket->GetPort());
}

void CMessagePort::OnAccepted(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    LOGNETDEBUG("MessagePort:{} OnAccpet {}:{}", GetServerPort().GetServiceID(), pSocket->GetAddrString(), pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortAccepted(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::SetRemoteSocket(const CNetSocketSharedPtr& pRemoteSocket)
{
    __ENTER_FUNCTION
    m_pRemoteSocket = pRemoteSocket;
    __LEAVE_FUNCTION
}

CNetSocketSharedPtr CMessagePort::GetRemoteSocket() const
{
    __ENTER_FUNCTION
    return m_pRemoteSocket.lock();
    __LEAVE_FUNCTION
    return nullptr;
}

void CMessagePort::DetachRemoteSocket()
{
    __ENTER_FUNCTION
    auto pSocket = GetRemoteSocket();
    if(pSocket)
        pSocket->DetachEventHandler();
    else
        m_pRemoteSocket.reset();

    __LEAVE_FUNCTION
}

void CMessagePort::OnRecvData(const CNetSocketSharedPtr& pSocket, byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    MSG_HEAD*   pHead = (MSG_HEAD*)pBuffer;
    InternalMsg internal_msg;
    byte*       pData    = pBuffer + sizeof(MSG_HEAD);
    size_t      data_len = len - sizeof(MSG_HEAD);
    if(internal_msg.ParseFromArray(pData, static_cast<int>(data_len)) == false)
    {
        LOGNETERROR("MessagePort:{} Recv a unknow cmd:{} size:{}", GetServerPort().GetServiceID(), pHead->msg_cmd, pHead->msg_size);
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
        pHandler->OnPortRecvData(*pMsg);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::OnRecvTimeout(const CNetSocketSharedPtr& pSocket)
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
    if(auto shared_ptr = m_pRemoteSocket.lock())
    {
        size_t nNeedWrite = shared_ptr->GetWaitWriteSize();
        return nNeedWrite;
    }
    __LEAVE_FUNCTION
    return 0;
}

void CMessagePort::_SendMsgToRemoteSocket(const CNetworkMessage& msg)
{

    __ENTER_FUNCTION
    CHECK(m_pRemoteSocket.expired() == false);

    InternalMsg internal_msg;
    internal_msg.set_from(msg.GetFrom());
    internal_msg.set_to(msg.GetTo());
    internal_msg.set_proto_msg(msg.GetBuf(), msg.GetSize());

    for(const auto& v: msg.GetForward())
    {
        internal_msg.add_forward(v);
    }
    for(const auto& v: msg.GetMultiTo())
    {
        internal_msg.add_multi_vs(v);
    }
    for(const auto& v: msg.GetMultiIDTo())
    {
        internal_msg.add_multi_id(v);
    }
    for(const auto& v: msg.GetMultiIDTo())
    {
        internal_msg.add_multi_id(v);
    }
    for(const auto& v: msg.GetBroadcastTo())
    {
        internal_msg.add_broadcast_to(v);
    }
    internal_msg.set_brocast_type(msg.GetBroadcastType());

    CNetworkMessage send_msg(0xFFFF, internal_msg);
    if(auto shared_ptr = m_pRemoteSocket.lock())
        m_pRoute->GetNetworkService()->SendSocketMsgByIdx(shared_ptr->GetSocketIdx(), send_msg, false);

    __LEAVE_FUNCTION
}

bool CMessagePort::SendMsgToPort(const CNetworkMessage& msg)
{
    __ENTER_FUNCTION
    if(m_bLocalPort == true)
    {
        CNetworkMessage* pMsg = new CNetworkMessage(msg);
        pMsg->CopyBuffer();
        m_RecvMsgQueue.push(pMsg);
        auto pHandler = m_pPortEventHandler.load();
        if(pHandler)
        {
            pHandler->OnPortRecvData(*pMsg);
        }
        return true;
    }
    else
    {
        _SendMsgToRemoteSocket(msg);
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}
