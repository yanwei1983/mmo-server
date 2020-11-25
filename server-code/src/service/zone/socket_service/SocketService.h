#ifndef SocketService_h__
#define SocketService_h__

#include "IService.h"
#include "MyTimer.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

class CSocketService;
class CGameClient
{
public:
    CGameClient();
    ~CGameClient();

    const ServerPort& GetDestServerPort() const { return m_nDestServerPort; }
    void              SetDestServerPort(const ServerPort& val) { m_nDestServerPort = val; }
    VirtualSocket     GetVirtualSocket() const { return m_VirtualSocket; }
    void              SetVirtualSocket(VirtualSocket val) { m_VirtualSocket = val; }

    uint16_t GetMessageAllowBegin() { return m_nMessageAllowBegin; }
    uint16_t GetMessageAllowEnd() { return m_nMessageAllowEnd; }
    void     SetMessageAllow(uint16_t nMessageAllowBegin, uint16_t nMessageAllowEnd)
    {
        m_nMessageAllowBegin = nMessageAllowBegin;
        m_nMessageAllowEnd   = nMessageAllowEnd;
    }

    void Interrupt();
    bool SendSocketMsg(const CNetworkMessage& msg);

    CSocketService* GetService() const { return m_pService; }
    void            SetService(CSocketService* val) { m_pService = val; }
    std::string     GetSocketAddr() const { return m_SocketAddr; }
    void            SetSocketAddr(std::string val) { m_SocketAddr = val; }
    uint32_t        GetSocketPort() const { return m_nSocketPort; }
    void            SetSocketPort(uint32_t val) { m_nSocketPort = val; }
    bool            IsVaild() const;
    bool            IsAuth() const { return m_bAuth; }
    void            SetAuth(bool val) { m_bAuth = val; }
    OBJID           GetUserID() const { return m_idUser; }
    void            SetUserID(OBJID val) { m_idUser = val; }

private:
    VirtualSocket m_VirtualSocket;
    ServerPort    m_nDestServerPort;
    uint16_t      m_nMessageAllowBegin;
    uint16_t      m_nMessageAllowEnd;
    std::string   m_SocketAddr;
    uint32_t      m_nSocketPort = 0;

    OBJID           m_idUser   = 0;
    bool            m_bAuth    = false;
    CSocketService* m_pService = nullptr;
};

struct event;
class CNetMSGProcess;
class CSocketService : public IService, public CServiceCommon, public CNetEventHandler
{
    CSocketService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CSocketService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CSocketService);

public:
    virtual void OnConnected(CNetSocket* pSocket) override;
    virtual void OnConnectFailed(CNetSocket*) override;
    virtual void OnDisconnected(CNetSocket* pSocket) override;
    virtual void OnAccepted(CNetSocket* pSocket) override;
    virtual void OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len) override;
    virtual void OnRecvTimeout(CNetSocket* pSocket) override;

    void         OnProcessMessage(CNetworkMessage*) override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;
    virtual void OnLogicThreadProc() override;
    virtual void OnAllWaitedServiceReady()override;
    
    CGameClient* QueryClient(const VirtualSocket& vs);
    void         AddClient(const VirtualSocket& vs, CGameClient* pClient);
    void         RemoveClient(const VirtualSocket& vs);

    void         MapClientByUserID(OBJID idUser, CGameClient* pClient);
    CGameClient* QueryClientByUserID(OBJID idUser);
    bool         DelClientByUserID(OBJID idUser);

private:
    std::mutex                            m_mutex;
    std::map<VirtualSocket, CGameClient*> m_setVirtualSocket;
    std::map<OBJID, CGameClient*>         m_mapClientByUserID;

    // CUIDFactory m_UIDFactory;

    std::deque<uint16_t> m_SocketPool;

    uint64_t m_nSocketMessageProcess    = 0;
    uint64_t m_nWebSocketMessageProcess = 0;

    CMyTimer m_tLastDisplayTime;
};

CSocketService* SocketService();
void            SetSocketServicePtr(CSocketService* ptr);
#endif // SocketService_h__
