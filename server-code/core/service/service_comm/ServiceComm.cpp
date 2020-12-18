#include "ServiceComm.h"

#include <iostream>

#include "CheckUtil.h"
#include "DB2PB.h"
#include "EventManager.h"
#include "GlobalSetting.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MysqlConnection.h"
#include "NetMSGProcess.h"
#include "NetworkService.h"
#include "NormalCrypto.h"
#include "protomsg_to_cmd.h"
#include "serverinfodb.h"
#include "serverinfodb.pb.h"

CServiceCommon::CServiceCommon() {}

CServiceCommon::~CServiceCommon() {}

void CServiceCommon::DestoryServiceCommon()
{
    __ENTER_FUNCTION
    // GetMessageRoute()->CloseMessagePort(m_pMessagePort);
    StopLogicThread();
    if(auto shared_ptr = m_pMessagePort.lock())
    {
        shared_ptr->SetPortEventHandler(nullptr);
        m_pMessagePort.reset();
    }

    if(m_pEventManager)
    {
        m_pEventManager->Destroy();
        m_pEventManager.reset();
    }

    if(m_pMonitorMgr)
    {
        m_pMonitorMgr.reset();
    }

    LOGMESSAGE("{} {} Close", GetServiceName().c_str(), GetServerPort().GetServiceID());
    __LEAVE_FUNCTION
}

bool CServiceCommon::Init(const ServerPort& nServerPort, bool bEventManagerPause)
{
    __ENTER_FUNCTION
    m_nServerPort    = nServerPort;
    m_ServiceName    = ::GetServiceName(nServerPort.GetServiceID());
    m_pNetMsgProcess = std::make_unique<CNetMSGProcess>();
    m_pEventManager.reset(CEventManager::CreateNew(nullptr, bEventManagerPause));
    CHECKF(m_pEventManager.get());
    m_pMonitorMgr.reset(CMonitorMgr::CreateNew());
    CHECKF(m_pMonitorMgr.get());
    return true;
    __LEAVE_FUNCTION
    return true;
}

bool CServiceCommon::CreateService(int32_t nWorkInterval /*= 100*/, class CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
    __ENTER_FUNCTION
    if(ListenMessagePort(GetServiceName(), pEventHandler) == false)
        return false;
    //开启逻辑线程处理来自其他服务器的消息
    StartLogicThread(nWorkInterval, GetServiceName() + "_Logic");

    LOGMESSAGE("{} {} Create", GetServiceName().c_str(), GetServerPort().GetServiceID());
    __LEAVE_FUNCTION
    return true;
}

bool CServiceCommon::ListenMessagePort(const std::string& service_name, CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
    __ENTER_FUNCTION
    auto shared_ptr = GetMessageRoute()->QueryMessagePort(GetServerPort(), false);
    if(shared_ptr != nullptr)
    {
        m_pMessagePort = shared_ptr;
        if(pEventHandler != nullptr)
            shared_ptr->SetPortEventHandler(pEventHandler);
    }
    return shared_ptr != nullptr;
    __LEAVE_FUNCTION
    return false;
}

void CServiceCommon::StartLogicThread(int32_t nWorkInterval, const std::string& name)
{
    __ENTER_FUNCTION
    if(m_pLogicThread)
    {
        return;
    }
    m_pLogicThread = std::make_unique<CNormalThread>(nWorkInterval,
                                                     name,
                                                     std::bind(&CServiceCommon::OnLogicThreadProc, this),
                                                     std::bind(&CServiceCommon::OnLogicThreadCreate, this),
                                                     std::bind(&CServiceCommon::OnLogicThreadExit, this));
    __LEAVE_FUNCTION
}

void CServiceCommon::StopLogicThread()
{
    __ENTER_FUNCTION
    if(m_pLogicThread)
    {
        m_pLogicThread->Stop();
        m_pLogicThread->Join();
    }
    __LEAVE_FUNCTION
}

void CServiceCommon::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    if(m_pNetMsgProcess->Process(pNetworkMsg) == false)
    {
        LOGERROR("CMD {} from {} to {} forward_count {} didn't have ProcessHandler",
                 pNetworkMsg->GetCmd(),
                 pNetworkMsg->GetFrom(),
                 pNetworkMsg->GetTo(),
                 pNetworkMsg->GetForward().size());
    }
}

void CServiceCommon::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    constexpr int32_t MAX_PROCESS_PER_LOOP = 1000;
    int32_t           nCount               = 0;

    CNetworkMessage* pMsg = nullptr;
    if(auto pMessagePort = m_pMessagePort.lock())
    {
        // process message_port msg
        while(nCount < MAX_PROCESS_PER_LOOP && pMessagePort->TakePortMsg(pMsg))
        {
            nCount++;
            OnProcessMessage(pMsg);
            SAFE_DELETE(pMsg);
        }
    }

    m_nMessageProcess += nCount;

    //定时器回掉
    m_pEventManager->OnTimer();

    __LEAVE_FUNCTION
}

void CServiceCommon::OnLogicThreadCreate()
{
    __ENTER_FUNCTION
    BaseCode::InitMonitorLog(m_ServiceName);
    __LEAVE_FUNCTION
}

void CServiceCommon::OnLogicThreadExit() {}

bool CServiceCommon::ForwardProtoMsgToPlayer(const ServerPort& nServerPort, uint64_t id_player, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(msg_to_cmd(msg), msg, GetServerVirtualSocket(), nServerPort);
    _msg.AddMultiIDTo(id_player);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendBroadcastMsgToPort(const ServerPort& nServerPort, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(msg_to_cmd(msg), msg, GetServerVirtualSocket(), nServerPort);
    _msg.SetBroadcastType(BROADCAST_ALL);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendProtoMsgToZonePort(const ServerPort& nServerPort, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(msg_to_cmd(msg), msg, GetServerVirtualSocket(), nServerPort);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendMsgToVirtualSocket(const VirtualSocket& vsTo, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(msg_to_cmd(msg), msg, GetServerVirtualSocket(), vsTo);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToPort(const ServerPort& nServerPort, const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(*pMsg);
    _msg.SetTo(nServerPort);
    _msg.CopyBuffer();

    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToSomePort(const std::vector<ServerPort>& setServerPortList, const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    for(const auto& server_port: setServerPortList)
    {
        CNetworkMessage _msg(*pMsg);
        _msg.SetTo(server_port);
        _msg.CopyBuffer();
        _SendMsgToZonePort(_msg);
    }
    return setServerPortList.empty() == false;
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToThisZoneAllPort(const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldID(GetWorldID(), false);
    return TransmitMsgToSomePort(serverport_list, pMsg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToThisZoneWithServiceType(const CNetworkMessage* pMsg, ServiceType_t idServiceType) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(GetWorldID(), idServiceType, false);
    return TransmitMsgToSomePort(serverport_list, pMsg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToThisZoneAllPortExcept(const CNetworkMessage* pMsg, const std::set<ServiceType_t>& setExcept) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDExcept(GetWorldID(), setExcept, false);
    return TransmitMsgToSomePort(serverport_list, pMsg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToAllRoute(const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    if(GetWorldID() == 0)
    {
        auto serverport_list = GetMessageRoute()->GetServerPortListByServiceTypeExcept(ROUTE_SERVICE, {0});
        return TransmitMsgToSomePort(serverport_list, pMsg);
    }
    else
    {
        //发送给0-route,由0-route转发给所有的route,所有的route再转发给所有的server
        CNetworkMessage _msg(*pMsg);
        _msg.SetTo(ServerPort(0, ROUTE_SERVICE, 0));
        _msg.CopyBuffer();
        _msg.SetBroadcastType(BROADCAST_INCLUDE);
        _msg.AddBroadcastTo(ROUTE_SERVICE);
        _SendMsgToZonePort(_msg);
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToAllRouteExcept(const CNetworkMessage* pMsg, const std::set<WorldID_t>& setExcept) const
{
    __ENTER_FUNCTION
    if(GetWorldID() == 0)
    {
        auto serverport_list = GetMessageRoute()->GetServerPortListByServiceTypeExcept(ROUTE_SERVICE, setExcept);
        return TransmitMsgToSomePort(serverport_list, pMsg);
    }
    else
    {
        LOGWARNING("TransmitMsgToAllRouteExcept Call On WorldID != 0");
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::_SendMsgToZonePort(const CNetworkMessage& msg) const
{
    __ENTER_FUNCTION
    LOGNETTRACE("SendMsgToZonePort from:{} To:{} Cmd:{} ForwardCount:{}", msg.GetFrom(), msg.GetTo(), msg.GetCmd(), msg.GetForwardCount());
    const auto& vs = msg.GetTo();
    if(vs.IsVaild() == false)
    {
        LOGWARNING("SendMsgToZonePort fail To Is Not Valild.");
        LOGWARNING("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7)));
        return false;
    }
    if(GetMessageRoute() && vs.GetServerPort() != m_nServerPort)
    {
        m_pMonitorMgr->AddSendInfo(msg.GetCmd(), msg.GetSize());
        if(vs.GetServerPort().GetWorldID() == GetWorldID())
        {
            // trans msg in same world
            CMessagePortSharedPtr pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
            if(pMessagePort)
            {
                return pMessagePort->SendMsgToPort(msg);
            }
            else
            {
                LOGWARNING("SendMsgToZonePort world from:{} To:{} Cmd:{} ForwardCount:{} Worng.",
                           msg.GetFrom(),
                           msg.GetTo(),
                           msg.GetCmd(),
                           msg.GetForwardCount());
            }
        }
        else if(GetServiceID().GetServiceType() == ROUTE_SERVICE && vs.GetServerPort().GetServiceType() == ROUTE_SERVICE)
        {
            // route_service send msg to other route
            CMessagePortSharedPtr pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
            if(pMessagePort)
            {
                return pMessagePort->SendMsgToPort(msg);
            }
            else
            {
                LOGWARNING("SendMsgToZonePort trans from:{} To:{} Cmd:{} ForwardCount:{} Worng.",
                           msg.GetFrom(),
                           msg.GetTo(),
                           msg.GetCmd(),
                           msg.GetForwardCount());
            }
        }
        else
        {
            //通过route转发
            CNetworkMessage newmsg(msg);
            newmsg.AddForward(msg.GetTo());
            return TransmitMsgToPort(ServerPort(GetWorldID(), ROUTE_SERVICE, 0), &newmsg);
        }
        return false;
    }
    else if(GetMessageRoute() && vs.GetServerPort() == m_nServerPort)
    {
        // send to this service
        CMessagePortSharedPtr pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
        if(pMessagePort)
        {
            return pMessagePort->SendMsgToPort(msg);
        }
        else
        {
            LOGWARNING("SendMsgToZonePort local from:{} To:{} Cmd:{} ForwardCount:{} Worng.",
                       msg.GetFrom(),
                       msg.GetTo(),
                       msg.GetCmd(),
                       msg.GetForwardCount());
        }
        return false;
    }
    else if(vs.GetServerPort().IsVaild() == false && vs.GetSocketIdx() != 0)
    {
        // direct send message
        LOGWARNING("Message Want Send from:{} To:{} Cmd:{} Worng.", msg.GetFrom(), msg.GetTo(), msg.GetCmd());
        return false;
    }
    else
    {
        LOGWARNING("Message Want Send from:{} To:{} Cmd:{} ForwardCount:{} Worng.", msg.GetFrom(), msg.GetTo(), msg.GetCmd(), msg.GetForwardCount());
        return false;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendProtoMsgTo(const VirtualSocketMap_t& setSocketMap, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    auto            nCmd = msg_to_cmd(msg);
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket());
    for(auto& [nServerPort, socket_list]: setSocketMap)
    {
        if(socket_list.size() == 1)
        {
            _msg.SetTo(socket_list.front());
            m_pMonitorMgr->AddSendInfo(nCmd, _msg.GetSize());
            _SendMsgToZonePort(_msg);
        }
        else
        {
            m_pMonitorMgr->AddSendInfo_some(nCmd, _msg.GetSize(), socket_list.size());
            _msg.SetMultiTo(socket_list);
            _msg.SetTo(nServerPort);
            _SendMsgToZonePort(_msg);
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

std::unique_ptr<db::tbld_dbinfo> CServiceCommon::QueryDBInfo(WorldID_t nWorldID, CMysqlConnection* pServerInfoDB)
{
    return DB2PB::QueryOneConst<TBLD_DBINFO, db::tbld_dbinfo, TBLD_DBINFO::WORLDID>(pServerInfoDB, nWorldID);
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectDB(const db::tbld_dbinfo* pInfo)
{
    CHECKF(pInfo);
    auto pDB            = std::make_unique<CMysqlConnection>();
    auto real_mysql_url = NormalCrypto::default_instance().Decode(pInfo->db_url());
    auto result         = pDB->Connect(real_mysql_url);

    if(result == false)
    {
        return nullptr;
    }
    return pDB;
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectGlobalDB(CMysqlConnection* pServerInfoDB)
{
    __ENTER_FUNCTION
    CHECKF(pServerInfoDB);
    //通过ServerInfodb查询localdb
    auto db_info = QueryDBInfo(0, pServerInfoDB);
    CHECKF(db_info);
    m_globaldb_info.reset(db_info.release());
    return ConnectDB(m_globaldb_info.get());

    __LEAVE_FUNCTION
    return nullptr;
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectGlobalDB()
{
    __ENTER_FUNCTION

    return ConnectDB(m_globaldb_info.get());

    __LEAVE_FUNCTION
    return nullptr;
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectServerInfoDB()
{
    const auto& settings       = GetGlobalSetting()->GetData();
    auto        pServerInfoDB  = std::make_unique<CMysqlConnection>();
    auto        mysql_url      = settings["ServerInfoMYSQL"]["url"];
    auto        real_mysql_url = NormalCrypto::default_instance().Decode(mysql_url);
    if(pServerInfoDB->Connect(real_mysql_url) == false)
    {
        return nullptr;
    }
    return pServerInfoDB;
}

void CServiceCommon::AddWaitServiceReady(ServiceID&& service_id)
{
    m_setWaitServiceReady.emplace(service_id);
}

void CServiceCommon::OnWaitedServiceReady(const ServiceID& service_id)
{
    __ENTER_FUNCTION
    if(m_setWaitServiceReady.empty())
    {
        OnServiceReadyFromCrash(service_id);
        return;
    }

    m_setWaitServiceReady.erase(service_id);
    LOGMESSAGE("WaitedServiceReady:{}  left_need:{}", ::GetServiceName(service_id), m_setWaitServiceReady.size());
    if(m_setWaitServiceReady.empty() == true)
    {
        LOGMESSAGE("AllWaitedServiceReady");
        OnAllWaitedServiceReady();
    }

    __LEAVE_FUNCTION
}