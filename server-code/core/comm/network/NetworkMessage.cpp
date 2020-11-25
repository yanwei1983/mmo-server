#include "NetworkMessage.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
//#include "proto/options.pb.h"
#include "Decryptor.h"
#include "Encryptor.h"

OBJECTHEAP_IMPLEMENTATION(CNetworkMessage, s_Heap);

CNetworkMessage::CNetworkMessage()
    : m_nFrom(0)
    , m_nTo(0)
    , m_pBuf(nullptr)
    , m_nBufSize(0)
{
}

CNetworkMessage::~CNetworkMessage() {}

CNetworkMessage::CNetworkMessage(CNetworkMessage&& rht) noexcept
    : m_nFrom(rht.m_nFrom)
    , m_nTo(rht.m_nTo)
    , m_setForward(std::move(rht.m_setForward))
    , m_pBuf(rht.m_pBuf)
    , m_pBuffer(std::move(rht.m_pBuffer))
    , m_nBufSize(rht.m_nBufSize)
    , m_MultiTo(std::move(rht.m_MultiTo))
    , m_MultiIDTo(std::move(rht.m_MultiIDTo))
    , m_BroadcastTo(std::move(rht.m_BroadcastTo))
    , m_nBroadCastType(rht.m_nBroadCastType)
{
}

CNetworkMessage::CNetworkMessage(const CNetworkMessage& rht)
    : m_nFrom(rht.m_nFrom)
    , m_nTo(rht.m_nTo)
    , m_setForward(rht.m_setForward)
    , m_pBuf(rht.m_pBuf)
    , m_pBuffer(rht.m_pBuffer)
    , m_nBufSize(rht.m_nBufSize)
    , m_MultiTo(rht.m_MultiTo)
    , m_MultiIDTo(rht.m_MultiIDTo)
    , m_BroadcastTo(rht.m_BroadcastTo)
    , m_nBroadCastType(rht.m_nBroadCastType)
{
}

CNetworkMessage::CNetworkMessage(byte* buf, size_t len, const VirtualSocket& from /*= 0*/, const VirtualSocket& to /*= 0*/)
    : m_nFrom(from)
    , m_nTo(to)
    , m_pBuf(buf)
    , m_nBufSize(len)
{
}

CNetworkMessage::CNetworkMessage(uint16_t msg_cmd, const proto_msg_t& msg, const VirtualSocket& from /*= 0*/, const VirtualSocket& to /*= 0*/)
    : m_nFrom(from)
    , m_nTo(to)
    , m_pBuf(nullptr)
    , m_nBufSize(0)
{
    int32_t nDataSize = msg.ByteSizeLong();
    AllocBuffer(nDataSize + sizeof(MSG_HEAD));
    msg.SerializeToArray(GetMsgBody(), nDataSize);

    MSG_HEAD* pHead = GetMsgHead();
    pHead->msg_size   = nDataSize + sizeof(MSG_HEAD);
    pHead->msg_cmd    = msg_cmd;
    pHead->is_ciper   = FALSE;
    pHead->reserved   = 0;
    // pHead->msg_cmd =  = msg.GetDescriptor()->options().GetExtension(NetMSG::msgid);

    // static auto desp = google::protobuf::DescriptorPool::generated_pool()->FindEnumTypeByName(std::string("MSGID"));
    // auto evd = desp->FindValueByName(msg.GetDescriptor()->full_name());
    // pHead->msg_cmd = evd->number();
}

CNetworkMessage::CNetworkMessage(uint16_t msg_cmd, byte* body, size_t body_len, const VirtualSocket& from /*= 0*/, const VirtualSocket& to /*= 0*/)
    : m_nFrom(from)
    , m_nTo(to)
    , m_pBuf(nullptr)
    , m_nBufSize(0)
{
    AllocBuffer(body_len + sizeof(MSG_HEAD));
    memcpy(GetMsgBody(), body, body_len);
    MSG_HEAD* pHead = GetMsgHead();
    pHead->msg_size   = body_len + sizeof(MSG_HEAD);
    pHead->msg_cmd    = msg_cmd;
    pHead->is_ciper   = FALSE;
    pHead->reserved   = 0;
}

void CNetworkMessage::CopyRawMessage(const CNetworkMessage& rht)
{
    m_nFrom    = rht.m_nFrom;
    m_nTo      = rht.m_nTo;
    m_pBuf     = rht.m_pBuf;
    m_pBuffer  = rht.m_pBuffer;
    m_nBufSize = rht.m_nBufSize;
}

byte* CNetworkMessage::GetBuf() const
{
    if(m_pBuf)
        return m_pBuf;
    return m_pBuffer.get();
}

void CNetworkMessage::SetForward(const uint64_t* pVS, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        m_setForward.push_back(VirtualSocket{pVS[i]});
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNetworkMessage::AddMultiTo(const VirtualSocket& to)
{
    m_MultiTo.push_back(to);
}

void CNetworkMessage::AddMultiTo(const std::vector<VirtualSocket>& _MultiTo)
{
    m_MultiTo.insert(m_MultiTo.end(), _MultiTo.begin(), _MultiTo.end());
}

void CNetworkMessage::SetMultiTo(const uint64_t* pVS, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        m_MultiTo.push_back(VirtualSocket{pVS[i]});
    }
}

void CNetworkMessage::SetMultiTo(const std::vector<VirtualSocket>& _MultiTo)
{
    m_MultiTo = _MultiTo;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNetworkMessage::AddMultiIDTo(OBJID to)
{
    m_MultiIDTo.push_back(to);
}

void CNetworkMessage::SetMultiIDTo(const std::vector<OBJID>& _MultiIDTo)
{
    m_MultiIDTo = _MultiIDTo;
}

void CNetworkMessage::AddMultiIDTo(const std::vector<OBJID>& _MultiIDTo)
{
    m_MultiIDTo.insert(m_MultiIDTo.end(), _MultiIDTo.begin(), _MultiIDTo.end());
}

void CNetworkMessage::SetMultiIDTo(const OBJID* pIDS, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        m_MultiIDTo.push_back(pIDS[i]);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CNetworkMessage::AddBroadcastTo(const ServiceType_t& to)
{
    m_BroadcastTo.push_back(to);
}

void CNetworkMessage::SetBroadcastTo(const std::vector<ServiceType_t>& _BroadcastTo)
{
    m_BroadcastTo = _BroadcastTo;
}

void CNetworkMessage::AddBroadcastTo(const std::vector<ServiceType_t>& _BroadcastTo)
{
    m_BroadcastTo.insert(m_BroadcastTo.end(), _BroadcastTo.begin(), _BroadcastTo.end());
}

void CNetworkMessage::SetBroadcastTo(const ServiceType_t* pData, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        m_BroadcastTo.push_back(pData[i]);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNetworkMessage::CopyBuffer()
{
    if(m_pBuf == nullptr)
        return;
    if(m_pBuffer != nullptr)
        return;
    AllocBuffer(m_nBufSize);
    memcpy(m_pBuffer.get(), m_pBuf, m_nBufSize);
    m_pBuf = nullptr;
}

void CNetworkMessage::AllocBuffer(size_t len)
{
    CHECK(m_pBuffer == nullptr);
    m_pBuffer  = std::shared_ptr<byte>(new byte[len], [](byte* p) { delete[] p; });
    m_nBufSize = len;
}

void CNetworkMessage::DuplicateBuffer()
{
    if(m_pBuffer == nullptr)
    {
        CopyBuffer();
    }   
    else
    {
        auto new_buffer  = std::shared_ptr<byte>(new byte[m_nBufSize], [](byte* p) { delete[] p; });
        memcpy(new_buffer.get(), m_pBuffer.get(), m_nBufSize);
        m_pBuffer = new_buffer;
    } 
}

bool CNetworkMessage::NeedDuplicateWhenEncryptor() const
{
    if(IsBroadcast())
        return true;
    if(m_nTo.GetSocketIdx() != 0)
        return m_MultiTo.empty() && m_MultiIDTo.empty();
    return m_MultiTo.size() + m_MultiIDTo.size() > 1;
}

void CNetworkMessage::Encryptor(CEncryptor* pEnc)
{
    if(pEnc && GetMsgHead()->is_ciper == FALSE)
    {
        constexpr size_t sizeof_HEAD = sizeof(MSG_HEAD);
        size_t buff_len = GetBodySize();
        byte* plain_buff = GetMsgBody();
        size_t cipher_len = pEnc->Encryptor(plain_buff, buff_len, plain_buff, buff_len);
        GetMsgHead()->msg_size = cipher_len + sizeof_HEAD;
        GetMsgHead()->is_ciper = TRUE;
    }
}

void CNetworkMessage::Decryptor(CDecryptor* pDec)
{
    if(pDec && GetMsgHead()->is_ciper == TRUE)
    {
        constexpr size_t sizeof_HEAD = sizeof(MSG_HEAD);
        size_t buff_len = GetBodySize();
        byte* cipher_buff = GetMsgBody();
        size_t plain_len = pDec->Decryptor(cipher_buff, buff_len, cipher_buff, buff_len);
        GetMsgHead()->msg_size = plain_len + sizeof_HEAD;
        GetMsgHead()->is_ciper = FALSE;
    }
}