#include "NetworkMessage.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
//#include "proto/options.pb.h"
#include "Decryptor.h"
#include "Encryptor.h"
#include "lz4.h"

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
    auto nDataSize = msg.ByteSizeLong();
    AllocBuffer(nDataSize + sizeof(MSG_HEAD));
    msg.SerializeToArray(GetMsgBody(), nDataSize);

    MSG_HEAD* pHead = GetMsgHead();
    pHead->msg_size = nDataSize + sizeof(MSG_HEAD);
    pHead->msg_cmd  = msg_cmd;
    pHead->msg_flag.is_ciper = FALSE;
    pHead->msg_flag.is_zip = FALSE;
    pHead->msg_flag.reserved = 0;
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
    pHead->msg_size = body_len + sizeof(MSG_HEAD);
    pHead->msg_cmd  = msg_cmd;
    pHead->msg_flag.is_ciper = FALSE;
    pHead->msg_flag.is_zip = FALSE;
    pHead->msg_flag.reserved = 0;
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
        auto new_buffer = std::shared_ptr<byte>(new byte[m_nBufSize], [](byte* p) { delete[] p; });
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
    if(GetMsgHead()->msg_flag.is_ciper == TRUE)
        return;

    CHECK(pEnc);

    constexpr size_t sizeof_HEAD = sizeof(MSG_HEAD);
    size_t           buff_len    = GetBodySize();
    byte*            plain_buff  = GetMsgBody();
    size_t           cipher_len  = pEnc->Encryptor(plain_buff, buff_len, plain_buff, buff_len);
    GetMsgHead()->msg_size          = cipher_len + sizeof_HEAD;
    GetMsgHead()->msg_flag.is_ciper = TRUE;
    
}

void CNetworkMessage::Decryptor(CDecryptor* pDec)
{
    if(GetMsgHead()->msg_flag.is_ciper == FALSE)
        return;

    CHECK(pDec);      

    constexpr size_t sizeof_HEAD = sizeof(MSG_HEAD);
    size_t           buff_len    = GetBodySize();
    byte*            cipher_buff = GetMsgBody();
    size_t           plain_len   = pDec->Decryptor(cipher_buff, buff_len, cipher_buff, buff_len);
    GetMsgHead()->msg_size       = plain_len + sizeof_HEAD;
    GetMsgHead()->msg_flag.is_ciper       = FALSE;
    
}

void CNetworkMessage::Compress()
{   
    if(GetMsgHead()->msg_flag.is_zip == TRUE)
        return;
    constexpr size_t MIN_ZIP_SIZE = 1024;
    if(GetBodySize() < MIN_ZIP_SIZE)
        return;

    
    size_t           buff_len    = GetBodySize();
    byte*            plain_buff  = GetMsgBody();
    
    size_t dst_capacity = LZ4_compressBound(buff_len);
    auto dest_buffer = std::shared_ptr<byte>(new byte[dst_capacity+sizeof(MSG_HEAD_ZIP)], [](byte* p) { delete[] p; });
    
    //copy old head to new head
    memcpy(dest_buffer.get(), GetMsgHead(), sizeof(MSG_HEAD));
    //compress
    size_t cipher_len  = LZ4_compress_default((const char*)plain_buff, (char*)dest_buffer.get() + sizeof(MSG_HEAD_ZIP), buff_len, dst_capacity);
    MSG_HEAD_ZIP* pNewHead = (MSG_HEAD_ZIP*)dest_buffer.get();
    pNewHead->msg_flag.is_zip = TRUE;
    pNewHead->plain_size = buff_len;
    pNewHead->msg_size = cipher_len + sizeof(MSG_HEAD_ZIP);
    //move buffer
    m_pBuffer = dest_buffer;   
    m_nBufSize = pNewHead->msg_size;
    m_pBuf = nullptr;
    
}

void CNetworkMessage::Decompress()
{
    if(GetMsgHead()->msg_flag.is_zip == FALSE)
        return;
    
    MSG_HEAD_ZIP* pHead = (MSG_HEAD_ZIP*)GetBuf();
    size_t cipher_len  = GetSize() - sizeof(MSG_HEAD_ZIP);       
    byte*  cipher_buff = GetBuf() + sizeof(MSG_HEAD_ZIP);
    auto plain_size = pHead->plain_size;  
    auto dest_buffer = std::shared_ptr<byte>(new byte[plain_size + sizeof(MSG_HEAD)], [](byte* p) { delete[] p; });

    //copy old head to new head
    memcpy(dest_buffer.get(), GetMsgHead(), sizeof(MSG_HEAD));
    //decompress
    size_t plain_len = LZ4_decompress_safe((const char*)cipher_buff, (char*)dest_buffer.get() + sizeof(MSG_HEAD), cipher_len, plain_size);
    if(plain_len != plain_size)
    {
        LOGERROR("msg decompress error plain:{} newplain:{} cmd:{} from:{} to:{}", plain_size, plain_len, GetCmd(), GetFrom(), GetTo());
    }
    MSG_HEAD* pNewHead = (MSG_HEAD*)dest_buffer.get();
    pNewHead->msg_size = plain_len + sizeof(MSG_HEAD);
    pNewHead->msg_flag.is_zip  = FALSE;
    //move buffer
    m_pBuffer = dest_buffer;   
    m_nBufSize = pNewHead->msg_size;
    m_pBuf = nullptr;

    
}
