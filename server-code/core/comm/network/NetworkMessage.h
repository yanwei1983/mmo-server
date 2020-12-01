#ifndef NetworkMessage_h__
#define NetworkMessage_h__

#include <memory>

#include "BaseCode.h"
#include "NetworkDefine.h"
#include "ObjectHeap.h"
//#include <google/protobuf/message.h>

export_lua enum BroadcastType {
    BROADCAST_NONE    = 0,
    BROADCAST_ALL     = 1,
    BROADCAST_INCLUDE = 2,
    BROADCAST_EXCLUDE = 2,
};

export_lua class CNetworkMessage
{
public:
    CNetworkMessage();
    ~CNetworkMessage();
    CNetworkMessage(CNetworkMessage&& rht) noexcept;
    export_lua CNetworkMessage(const CNetworkMessage& rht);
    export_lua CNetworkMessage(byte* buf, size_t len, const VirtualSocket& from = 0, const VirtualSocket& to = 0);
    export_lua CNetworkMessage(uint16_t msg_cmd, byte* body, size_t body_len, const VirtualSocket& from = 0, const VirtualSocket& to = 0);
    export_lua CNetworkMessage(uint16_t msg_cmd, const proto_msg_t& msg, const VirtualSocket& from = 0, const VirtualSocket& to = 0);

    export_lua void CopyRawMessage(const CNetworkMessage& rht);

    export_lua uint16_t GetSize() const { return GetMsgHead()->msg_size; }
    export_lua uint16_t GetCmd() const { return GetMsgHead()->msg_cmd; }
    export_lua uint16_t GetBodySize() const { return GetSize() - sizeof(MSG_HEAD); }
    export_lua byte* GetBuf() const;
    export_lua MSG_HEAD* GetMsgHead() { return (MSG_HEAD*)GetBuf(); }
    export_lua const MSG_HEAD* GetMsgHead() const { return (const MSG_HEAD*)GetBuf(); }
    export_lua byte* GetMsgBody() { return GetBuf() + sizeof(MSG_HEAD); }
    export_lua const byte* GetMsgBody() const { return GetBuf() + sizeof(MSG_HEAD); }

    void Decryptor(class CDecryptor* pDec);
    void Encryptor(class CEncryptor* pEnc);

    export_lua const VirtualSocket& GetFrom() const { return m_nFrom; }
    export_lua void                 SetFrom(const VirtualSocket& val) { m_nFrom = val; }
    export_lua const VirtualSocket& GetTo() const { return m_nTo; }
    export_lua void                 SetTo(const VirtualSocket& val) { m_nTo = val; }

    export_lua const std::deque<VirtualSocket>& GetForward() const { return m_setForward; }

    export_lua void SetForward(const uint64_t* pVS, size_t len);
    export_lua void SetForward(const std::deque<VirtualSocket>& setForward) { m_setForward = setForward; }
    export_lua void AddForward(const VirtualSocket& forward) { m_setForward.push_back(forward); }
    export_lua void ClearForward() { m_setForward.clear(); }
    export_lua void PopForward() { m_setForward.pop_front(); }
    export_lua size_t GetForwardCount() const { return m_setForward.size(); }

    export_lua const std::vector<VirtualSocket>& GetMultiTo() const { return m_MultiTo; }

    export_lua bool IsMultiTo() const { return m_MultiTo.empty() == false; }
    export_lua void SetMultiTo(const uint64_t* pVS, size_t len);
    export_lua void SetMultiTo(const std::vector<VirtualSocket>& _MultiTo);
    export_lua void AddMultiTo(const std::vector<VirtualSocket>& _MultiTo);
    export_lua void AddMultiTo(const VirtualSocket& to);
    export_lua void ClearMultiTo() { m_MultiTo.clear(); }

    export_lua const std::vector<OBJID>& GetMultiIDTo() const { return m_MultiIDTo; }

    export_lua bool IsMultiIDTo() const { return m_MultiIDTo.empty() == false; }
    export_lua void SetMultiIDTo(const OBJID* pIDS, size_t len);
    export_lua void SetMultiIDTo(const std::vector<OBJID>& _MultiIDTo);
    export_lua void AddMultiIDTo(const std::vector<OBJID>& _MultiIDTo);
    export_lua void AddMultiIDTo(OBJID to);
    export_lua void ClearMultiIDTo() { m_MultiIDTo.clear(); }

    export_lua const std::vector<ServiceType_t>& GetBroadcastTo() const { return m_BroadcastTo; }

    export_lua void SetBroadcastTo(const ServiceType_t* pData, size_t len);
    export_lua void SetBroadcastTo(const std::vector<ServiceType_t>& _BroadcastTo);
    export_lua void AddBroadcastTo(const std::vector<ServiceType_t>& _BroadcastTo);
    export_lua void AddBroadcastTo(const ServiceType_t& to);
    export_lua void ClearBroadcastTo() { m_BroadcastTo.clear(); }

    export_lua bool          IsBroadcast() const { return m_nBroadCastType != BROADCAST_NONE; }
    export_lua void          SetBroadcastType(BroadcastType v) { m_nBroadCastType = v; }
    export_lua BroadcastType GetBroadcastType() const { return m_nBroadCastType; }
    export_lua void          ClearBroadcast() { m_nBroadCastType = BROADCAST_NONE; }

    export_lua void CopyBuffer();
    void            DuplicateBuffer();
    bool            NeedDuplicateWhenEncryptor() const;

private:
    void AllocBuffer(size_t len);

public:
    OBJECTHEAP_DECLARATION(s_Heap);

private:
    VirtualSocket              m_nFrom;
    VirtualSocket              m_nTo;
    std::deque<VirtualSocket>  m_setForward;
    std::vector<VirtualSocket> m_MultiTo;
    std::vector<OBJID>         m_MultiIDTo;
    std::vector<ServiceType_t> m_BroadcastTo;
    BroadcastType              m_nBroadCastType = BROADCAST_NONE;
    byte*                      m_pBuf;
    std::shared_ptr<byte>      m_pBuffer;
    size_t                     m_nBufSize;
};

#endif // NetworkMessage_h__
