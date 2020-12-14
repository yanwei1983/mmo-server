#ifndef NetSocket_h__
#define NetSocket_h__

#include <deque>
#include <memory>
#include <numeric>
#include <string>

#include <time.h>

#include "LockfreeQueue.h"
#include "NetworkDefine.h"
#include "NetworkMessage.h"
#include "ObjectHeap.h"
#include "PerSecondCount.h"
class CNetworkService;
class CNetEventHandler;
using CNetEventHandlerSharedPtr = std::shared_ptr<CNetEventHandler>;
using CNetEventHandlerWeakPtr   = std::weak_ptr<CNetEventHandler>;

enum NET_SOCKET_STATUS
{
    NSS_NOT_INIT,
    NSS_CONNECTING,
    NSS_WAIT_RECONNECT,
    NSS_READY,
    NSS_CLOSEING,
    NSS_CLOSED,
};

struct bufferevent;
struct event;
class CDecryptor;
class CEncryptor;
class CNetSocket : public std::enable_shared_from_this<CNetSocket>
{
public:
    CNetSocket(CNetworkService* pService, const CNetEventHandlerSharedPtr& pEventHandler);
    virtual ~CNetSocket();

    virtual bool Init(bufferevent* pBufferEvent)    = 0;
    virtual void Interrupt(bool bClearEventHandler) = 0;
    virtual bool CreateByListener() const { return true; }

    bool SendNetworkMessage(CNetworkMessage&& msg);
    bool SendNetworkMessage(const CNetworkMessage& msg, bool bNeedDuplicate = true);
    void InitDecryptor(uint32_t seed);
    void InitEncryptor(uint32_t seed);

public:
    virtual void OnDisconnected();
    virtual void OnClosing();
    virtual void OnRecvData(byte* pBuffer, size_t len);
    virtual void OnRecvTimeout(bool& bReconnect);

public:
    void         _SetTimeout();
    void         _OnReceive(bufferevent* b);
    virtual void _OnError(const std::string& what) = 0;

    static void _OnSocketRead(bufferevent*, void* ctx);
    static void _OnSendOK(bufferevent* b, void* ctx);
    static void _OnCheckAllSendOK(bufferevent* b, void* ctx);
    static void _OnSocketEvent(bufferevent*, short, void* ctx);

public:
    CNetworkService* GetService() const { return m_pService; }
    bufferevent*     GetBufferevent() const { return m_pBufferevent; }

    NET_SOCKET_STATUS GetStatus() const { return m_Status; }
    void              SetStatus(NET_SOCKET_STATUS val) { m_Status = val; }

    void SetAddrAndPort(const char* addr, int32_t port);

    SOCKET      GetSocket() const { return m_socket; }
    void        SetSocket(SOCKET val) { m_socket = val; }
    uint16_t    GetSocketIdx() const { return m_nSocketIdx; }
    void        SetSocketIdx(uint16_t val) { m_nSocketIdx = val; }
    std::string GetAddrString() const { return m_strAddr; }
    uint64_t    GetAddr() const { return m_addr; }
    void        SetAddr(const std::string& val);
    int32_t     GetPort() const { return m_nPort; }
    void        SetPort(int32_t val) { m_nPort = val; }

    int32_t GetRecvTimeOutSec() const { return m_nRecvTimeOutSec; }
    void    SetRecvTimeOutSec(int32_t val) { m_nRecvTimeOutSec = val; }
    size_t  GetLogWriteHighWateMark() const { return m_nLogWriteHighWateMark; }
    void    SetLogWriteHighWateMark(size_t val) { m_nLogWriteHighWateMark = val; }
    size_t  GetWaitWriteSize();
    size_t  GetPacketSizeMax() const { return m_nPacketSizeMax; }
    void    _SetEventHandler(const CNetEventHandlerSharedPtr& v) { m_pEventHandler = v; }
    void    DetachEventHandler();
    void    set_sock_nodely();
    void    set_sock_quickack();

    bool _SendMsg(byte* pBuffer, size_t len);

public:
    OBJECTHEAP_DECLARATION(s_Heap);

protected:
    struct SendMsgData
    {
        SendMsgData(CNetworkMessage&& msg);
        SendMsgData(const CNetworkMessage& msg);

        CNetworkMessage send_msg;
    };

    void _SendAllMsg();
    void PostSend();

protected:
    CNetworkService*        m_pService;
    bufferevent*            m_pBufferevent;
    CNetEventHandlerWeakPtr m_pEventHandler;

    struct evbuffer* m_Sendbuf;

    MPSCQueue<SendMsgData*> m_SendMsgQueue;
    struct event*           m_pEventSendMsg;

    std::string m_strAddr;
    int32_t     m_nPort;
    uint64_t    m_addr;

    SOCKET   m_socket;
    uint16_t m_nSocketIdx;

    NET_SOCKET_STATUS m_Status;
    int32_t           m_nRecvTimeOutSec;

    std::unique_ptr<CDecryptor> m_pDecryptor;
    std::unique_ptr<CEncryptor> m_pEncryptor;
    size_t                      m_nPacketSizeMax;
    size_t                      m_nLogWriteHighWateMark; // default log when output>=4M
    std::unique_ptr<byte[]>     m_ReadBuff;
    unsigned short              m_nLastProcessMsgCMD = 0;
    unsigned short              m_nLastCMDSize       = 0;

    std::atomic<uint64_t> m_nWaitWriteSize = 0;
};

using CNetSocketSharedPtr = std::shared_ptr<CNetSocket>;
using CNetSocketWeakPtr   = std::weak_ptr<CNetSocket>;
#endif // NetSocket_h__
