#include "SocketService.h"

#include <functional>

#include "EventManager.h"
#include "MemoryHelp.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgProcessRegister.h"
#include "NetEventHandler.h"
#include "NetSocket.h"
#include "NetworkService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"
#include "protomsg_to_cmd.h"
#include "server_msg/server_side.pb.h"

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CSocketService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

CGameClient::CGameClient()
    : m_nDestServerPort(0, AUTH_SERVICE, 0)
    , m_nMessageAllowBegin(CMD_CS_LOGIN) // only accept CS_AUTH
    , m_nMessageAllowEnd(CMD_CS_LOGIN)   // only accept CS_AUTH

{
}

CGameClient::~CGameClient() {}

void CGameClient::Interrupt()
{
    CHECK(m_pService);
    LOGNETDEBUG("Client {}:{} Close.", GetSocketAddr().c_str(), GetSocketPort());
    SetMessageAllow(0, 0);
    m_pService->GetNetworkService()->_CloseSocket(m_VirtualSocket.GetSocketIdx());
    m_VirtualSocket.SetSocketIdx(0);
}

bool CGameClient::SendSocketMsg(const CNetworkMessage& msg)
{
    CHECKF(m_pService);
    return m_pService->GetNetworkService()->SendSocketMsgByIdx(m_VirtualSocket.GetSocketIdx(), msg, msg.NeedDuplicateWhenEncryptor());
}

bool CGameClient::IsVaild() const
{
    return m_VirtualSocket.GetSocketIdx() != 0;
}

static thread_local CSocketService* tls_pService = nullptr;

CSocketService* SocketService()
{
    return tls_pService;
}

void SetSocketServicePtr(CSocketService* ptr)
{
    tls_pService = ptr;
}

class CSocketServiceNetEventHandler : public CNetEventHandler
{
public:
    CSocketServiceNetEventHandler(CSocketService* pService)
        : m_pService(pService)
    {
    }
    virtual size_t GetSendPacketSizeMax() const override { return _MAX_MSGSIZE * 1024; }//4M_1Packet
    virtual void OnDisconnected(const CNetSocketSharedPtr& pSocket) override { m_pService->OnDisconnected(pSocket); }
    virtual void OnAccepted(const CNetSocketSharedPtr& pSocket) override { m_pService->OnAccepted(pSocket); }
    virtual void OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg) override { m_pService->OnRecvData(pSocket, std::move(recv_msg)); }

    CSocketService* m_pService;
};

//////////////////////////////////////////////////////////////////////////
CSocketService::CSocketService()
{
    m_tLastDisplayTime.Startup(20);
}

CSocketService::~CSocketService() {}

void CSocketService::Release()
{

    Destroy();
    delete this;
}

void CSocketService::Destroy()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
    if(m_pNetworkService)
    {
        m_pNetworkService->Destroy();
        m_pNetworkService.reset();
    }
    for(auto& [k, v]: m_setVirtualSocket)
    {
        SAFE_DELETE(v);
    }
    m_setVirtualSocket.clear();
    m_mapClientByUserID.clear();
}

bool CSocketService::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION

    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    CServiceCommon::Init(nServerPort, true);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };
    m_pNetEventHandler              = std::make_shared<CSocketServiceNetEventHandler>(this);
    const ServerAddrInfo* pAddrInfo = GetMessageRoute()->QueryServiceInfo(GetServerPort());
    if(pAddrInfo == nullptr)
    {
        LOGFATAL("CSocketService::Create QueryServerInfo {} fail", GetServerPort().GetServiceID());
        return false;
    }

    CHECKF(CreateNetworkService());

    //开启对外监听端口
    if(GetNetworkService()->Listen(pAddrInfo->bind_addr.c_str(), pAddrInfo->publish_port, m_pNetEventHandler) == nullptr)
    {
        return false;
    }
    GetNetworkService()->EnableListener(nullptr, false);
    GetNetworkService()->StartIOThread(GetServiceName() + "_Network");
    // websocket监听
    //		if (!GetNetworkService()->ListenWebSocket(9555, this))
    //		{
    //			return false;
    //		}

    // SetIPCheck(true);
    RegisterAllMsgProcess<CSocketService>(GetNetMsgProcess());
    AddWaitServiceReady(ServiceID{AUTH_SERVICE, GetServiceID().GetServiceIdx()});

    if(CreateService(100) == false)
        return false;

    return true;

    __LEAVE_FUNCTION

    return false;
}

bool CSocketService::CreateNetworkService()
{
    __ENTER_FUNCTION
    if(m_pNetworkService)
        return false;
    m_pNetworkService.reset(CNetworkService::CreateNew());
    return true;
    __LEAVE_FUNCTION
    return true;
}

void CSocketService::MapClientByUserID(OBJID idUser, CGameClient* pClient)
{
    if(idUser == 0)
        return;

    m_mapClientByUserID[idUser] = pClient;
    pClient->SetUserID(idUser);
}

CGameClient* CSocketService::QueryClientByUserID(OBJID idUser)
{
    if(idUser == 0)
        return nullptr;
    auto it = m_mapClientByUserID.find(idUser);
    if(it != m_mapClientByUserID.end())
        return it->second;
    else
        return nullptr;
}

bool CSocketService::DelClientByUserID(OBJID idUser)
{
    if(idUser == 0)
        return false;

    auto it = m_mapClientByUserID.find(idUser);
    if(it != m_mapClientByUserID.end())
    {
        m_mapClientByUserID.erase(it);
        return true;
    }
    else
        return false;
}

CGameClient* CSocketService::QueryClient(const VirtualSocket& vs)
{
    auto it = m_setVirtualSocket.find(vs);
    if(it != m_setVirtualSocket.end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void CSocketService::AddClient(const VirtualSocket& vs, CGameClient* pClient)
{
    m_setVirtualSocket[vs] = pClient;
    m_NonAuthClientCount++;
}

void CSocketService::RemoveClient(const VirtualSocket& vs)
{
    auto it = m_setVirtualSocket.find(vs);
    if(it != m_setVirtualSocket.end())
    {
        CGameClient* pClient = it->second;
        if(pClient->IsAuth())
        {
            ServerMSG::SocketClose msg;
            msg.set_vs(vs);
            SendProtoMsgToZonePort(ServerPort(GetServerPort().GetWorldID(), WORLD_SERVICE, 0), msg);
        }
        else
        {
            m_NonAuthClientCount--;
            ServerMSG::SocketClose msg;
            msg.set_vs(vs);
            SendProtoMsgToZonePort(ServerPort(GetServerPort().GetWorldID(), AUTH_SERVICE, GetServerPort().GetServiceIdx()), msg);
        }

        SAFE_DELETE(pClient);
        m_setVirtualSocket.erase(it);
    }
}

void CSocketService::OnDisconnected(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION
    RemoveClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
    __LEAVE_FUNCTION
}

void CSocketService::OnAccepted(const CNetSocketSharedPtr& pSocket)
{
    __ENTER_FUNCTION

    CGameClient* pClient = new CGameClient();
    pClient->SetService(this);
    pClient->SetDestServerPort(ServerPort(GetServerPort().GetWorldID(), AUTH_SERVICE, GetServerPort().GetServiceIdx()));
    pClient->SetVirtualSocket(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
    pClient->SetSocketAddr(pSocket->GetAddrString());
    pClient->SetSocketPort(pSocket->GetPort());
    AddClient(pClient->GetVirtualSocket(), pClient);

    // send crypto key to client
    uint32_t seed = TimeGetSecond();
    pSocket->InitDecryptor(seed);
    SC_KEY msg;
    msg.set_key(seed);
    CNetworkMessage _msg(msg_to_cmd(msg), msg);
    pSocket->SendNetworkMessage(std::move(_msg));
    __LEAVE_FUNCTION
}

void CSocketService::OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg)
{
    __ENTER_FUNCTION
    m_nSocketMessageProcess++;
    CGameClient* pClient = QueryClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
    if(pClient == nullptr)
        return;
    if(pClient->IsVaild() == false)
    {
        return;
    }
    // recv msg from client
    MSG_HEAD* pHead = recv_msg.GetMsgHead();
    if(pHead->msg_cmd < pClient->GetMessageAllowBegin() || pHead->msg_cmd > pClient->GetMessageAllowEnd())
    {
        LOGWARNING("RECV ClientMsg:{} not Allow {}.{}", pHead->msg_cmd, pSocket->GetAddrString(), pSocket->GetPort());
        pSocket->Interrupt(true);
        return;
    }
    m_pMonitorMgr->CmdProcessAdd(pHead->msg_cmd);

    switch(pHead->msg_cmd)
    {
        default:
        {
            if(pClient->GetDestServerPort().IsVaild() == false)
                return;
            // send to other server
            recv_msg.SetFrom(pClient->GetVirtualSocket());
            recv_msg.SetTo(VirtualSocket(pClient->GetDestServerPort()));
            _SendMsgToZonePort(recv_msg);
        }
        break;
    }
    __LEAVE_FUNCTION
}

void CSocketService::OnAllWaitedServiceReady()
{
    __ENTER_FUNCTION

    ServerMSG::ServiceReady send;
    send.set_serverport(SocketService()->GetServerPort());
    SocketService()->SendProtoMsgToZonePort(ServerPort(GetWorldID(), WORLD_SERVICE, 0), send);

    __LEAVE_FUNCTION
}

void CSocketService::OnAuthSucc(CGameClient* pClient)
{
    m_NonAuthClientCount--;
}

ON_SERVERMSG(CSocketService, ServiceReady)
{
    SocketService()->OnWaitedServiceReady(ServerPort(msg.serverport()).GetServiceID());
}

ON_SERVERMSG(CSocketService, SocketStartAccept)
{
    LOGMESSAGE("START_ACCEPT");
    SocketService()->GetNetworkService()->EnableListener(nullptr, true);
}

ON_SERVERMSG(CSocketService, SocketStopAccept)
{
    LOGMESSAGE("STOP_ACCEPT");
    SocketService()->GetNetworkService()->EnableListener(nullptr, false);
}

ON_SERVERMSG(CSocketService, SocketChangeDest)
{
    CGameClient* pClient = SocketService()->QueryClient(msg.vs());
    if(pClient && pClient->IsVaild())
    {
        ServerPort destport{msg.destport()};

        pClient->SetDestServerPort(destport);
        LOGNETDEBUG("SCK_CHG_DEST {}:{} To Service:{}", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort(), destport.GetServiceID());
    }
}

ON_SERVERMSG(CSocketService, SocketClose)
{
    CGameClient* pClient = SocketService()->QueryClient(msg.vs());
    if(pClient && pClient->IsVaild())
    {
        LOGDEBUG("CLOSE CLIENT BYVS:{}:{} FROM OTHER SERVER", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort());
        //主动关闭客户端连接，需要通知客户端不要重连
        pClient->Interrupt();
    }
}

ON_SERVERMSG(CSocketService, SocketAuth)
{
    CGameClient* pClient = SocketService()->QueryClient(msg.vs());

    if(pClient && pClient->IsVaild())
    {
        LOGDEBUG("AuthSucc BYVS:{}:{} ", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort());
        pClient->SetAuth(true);
        pClient->SetMessageAllow(CLIENT_MSG_ID_BEGIN, CLIENT_MSG_ID_END);
        pClient->SetDestServerPort(ServerPort(SocketService()->GetWorldID(), WORLD_SERVICE, 0));
        SocketService()->OnAuthSucc(pClient);

        ServerMSG::SocketLogin login_msg;
        login_msg.set_vs(msg.vs());
        login_msg.set_open_id(msg.open_id());
        SocketService()->SendProtoMsgToZonePort(ServerPort(SocketService()->GetWorldID(), WORLD_SERVICE, 0), login_msg);
    }
}

void CSocketService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    __ENTER_FUNCTION

    //只需要处理来自其他服务器的消息
    //来自客户端的消息已经直接发往对应服务器了
    if(m_pNetMsgProcess->Process(pNetworkMsg) == true)
    {
        return;
    }

    {
        CGameClient* pClient = QueryClient(pNetworkMsg->GetTo());
        if(pClient && pClient->IsVaild())
        {
            pClient->SendSocketMsg(*pNetworkMsg);
        }
    }
    for(const auto& vs: pNetworkMsg->GetMultiTo())
    {
        CGameClient* pClient = QueryClient(vs);
        if(pClient && pClient->IsVaild() && pClient->IsAuth())
        {
            pClient->SendSocketMsg(*pNetworkMsg);
        }
    }
    for(const auto& id: pNetworkMsg->GetMultiIDTo())
    {
        CGameClient* pClient = QueryClientByUserID(id);
        if(pClient && pClient->IsVaild() && pClient->IsAuth())
        {
            pClient->SendSocketMsg(*pNetworkMsg);
        }
    }
    if(pNetworkMsg->IsBroadcast())
    {
        for(const auto& v: m_setVirtualSocket)
        {
            CGameClient* pClient = v.second;
            if(pClient && pClient->IsVaild() && pClient->IsAuth())
            {
                pClient->SendSocketMsg(*pNetworkMsg);
            }
        }
    }

    __LEAVE_FUNCTION
}

void CSocketService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CSocketService::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    m_pNetworkService->RunOnce();

    CServiceCommon::OnLogicThreadProc();

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================");
        buf += fmt::format(FMT_STRING("\nClient:{}\tNotAuth:{}"), m_setVirtualSocket.size(), m_NonAuthClientCount);
        buf += fmt::format(FMT_STRING("\nServerMsgProcess:{}\tSocketMsgRecv:{}\tMem:{}"),
                           GetMessageProcess(),
                           m_nSocketMessageProcess,
                           get_thread_memory_allocted());
        buf += fmt::format(FMT_STRING("\nRecvTotal:{}\tRecvAvg:{}"),
                           GetNetworkService()->GetRecvBPS().GetTotal(),
                           GetNetworkService()->GetRecvBPS().GetAvgBPS());
        buf += fmt::format(FMT_STRING("\nSendTotal:{}\tSendAvg:{}"),
                           GetNetworkService()->GetSendBPS().GetTotal(),
                           GetNetworkService()->GetSendBPS().GetAvgBPS());

        GetMessageRoute()->ForEach([&buf](auto pMessagePort) {
            if(pMessagePort && pMessagePort->GetWriteBufferSize())
            {
                buf += fmt::format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"),
                                   pMessagePort->GetServerPort().GetServiceID(),
                                   pMessagePort->GetWriteBufferSize());
            }
        });

        LOGMONITOR("{}", buf.c_str());
        m_pMonitorMgr->Print();
        SetMessageProcess(0);
        m_nSocketMessageProcess = 0;
    }

    __LEAVE_FUNCTION
}
