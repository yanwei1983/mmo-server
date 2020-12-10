#include "RouteService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "server_msg/server_side.pb.h"

static thread_local CRouteService* tls_pService = nullptr;
CRouteService*                     RouteService()
{
    return tls_pService;
}
void SetRouteServicePtr(CRouteService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CRouteService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CRouteService::CRouteService() {}

CRouteService::~CRouteService() {}

void CRouteService::Release()
{

    Destory();
    delete this;
}

void CRouteService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CRouteService::Init(const ServerPort& nServerPort)
{
    //各种初始化
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

    //注册消息
    RegisterAllMsgProcess<CRouteService>(GetNetMsgProcess());

    if(CreateService(100) == false)
        return false;
    GetEventManager()->Pause(false);


    if(GetWorldID() != 0)
    {
        ServerMSG::ServiceReady msg;
        msg.set_serverport(GetServerPort());
        SendProtoMsgToZonePort(ServerPort(GetWorldID(), WORLD_SERVICE, 0), msg);
    }
    return true;
}


ON_SERVERMSG(CRouteService, ServiceRegister)
{
    ServerPort server_port{msg.serverport()};
    LOGMESSAGE("World:{} register", server_port.GetWorldID());
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), true);

    if(server_port.GetWorldID() == RouteService()->GetWorldID()) //本区
    {
        //转发给0区
        CNetworkMessage send_msg(*pMsg);
        send_msg.SetFrom(RouteService()->GetServerPort());
        send_msg.SetTo(ServerPort(0, ROUTE_SERVICE, 0));
        RouteService()->_SendMsgToZonePort(send_msg);

        //注册30秒发送一次ready
        CEventEntryCreateParam param;
        //param.evType = EVENTID_SEND_READY;
        param.cb = []() {
            RouteService()->SendServiceReady();
        };
        param.tWaitTime = 30 * 1000;
        param.bPersist  = true;
        EventManager()->ScheduleEvent(param);
    }
    else if(RouteService()->GetWorldID() == 0) // 0区
    {
        // worldid == 0
        GetMessageRoute()->ReloadServiceInfo(msg.update_time(), server_port.GetWorldID());
        //通知0区所有gm_proxy服
        auto server_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(0, GM_PROXY_SERVICE, false);
        RouteService()->TransmitMsgToSomePort(server_list, pMsg);
        //通知所有Route,除了原区和当前这个0区的Route
        RouteService()->TransmitMsgToAllRouteExcept(pMsg, {0, server_port.GetWorldID()});
    }
    else //其他区的route
    {

        GetMessageRoute()->ReloadServiceInfo(msg.update_time(), server_port.GetWorldID());
    }
}

void CRouteService::SendServiceReady()
{
    ServerMSG::ServiceReady send_msg;
    send_msg.set_serverport(GetServerPort());
    send_msg.set_ready(true);
    SendProtoMsgToZonePort(ServerPort(0, ROUTE_SERVICE, 0), send_msg);
}

void CRouteService::SendServiceUnReady()
{
    ServerMSG::ServiceReady send_msg;
    send_msg.set_serverport(GetServerPort());
    send_msg.set_ready(false);
    SendProtoMsgToZonePort(ServerPort(0, ROUTE_SERVICE, 0), send_msg);
}

ON_SERVERMSG(CRouteService, ServiceReady)
{
    ServerPort server_port{msg.serverport()};
    if(msg.ready() == false)
    {
        LOGMESSAGE("World:{} shutdown", server_port.GetWorldID());
    }
    else
    {
        LOGDEBUG("World:{} Ready", server_port.GetWorldID());
    }

    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), msg.ready());

    if(server_port.GetWorldID() == RouteService()->GetWorldID()) //本区
    {
        //转发给0区
        RouteService()->TransmitMsgToPort(ServerPort(0, ROUTE_SERVICE, 0), pMsg);
    }
    else if(RouteService()->GetWorldID() == 0) // 0区
    {
        // world == 0

        //通知所有Route,除了当前这个0区的Route
        // RouteService()->TransmitMsgToAllRouteExcept(pMsg, {0, server_port.GetWorldID()});
    }
    else //其他区
    {
    }
}

void CRouteService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{

    if(m_pNetMsgProcess->Process(pNetworkMsg) == true)
    {
        return;
    }

    //需要转发的，直接转发
    if(pNetworkMsg->GetForward().empty() == false)
    {
        auto forward = pNetworkMsg->GetForward().front();
        pNetworkMsg->PopForward();
        pNetworkMsg->SetTo(forward);
        LOGNETTRACE("TransMsg From:{} Forward:{} Cmd:{}", pNetworkMsg->GetFrom(), forward, pNetworkMsg->GetCmd());
        if(forward.GetServerPort() == GetServerPort())
        {
            OnProcessMessage(pNetworkMsg);
        }
        else
        {
            _SendMsgToZonePort(*pNetworkMsg);
        }
        return;
    }

    //转发给所有的Route
    if(pNetworkMsg->IsBroadcast())
    {
        if(GetWorldID() == 0)
        {
            LOGNETTRACE("TransmitMsgToAllRoute From:{} To:{} Cmd:{}", pNetworkMsg->GetFrom(), pNetworkMsg->GetTo(), pNetworkMsg->GetCmd());
            TransmitMsgToAllRoute(pNetworkMsg);
        }
        else if(pNetworkMsg->GetBroadcastType() == BROADCAST_INCLUDE)
        {
            LOGNETTRACE("TransmitMsgToThisZoneWithServiceType From:{} To:{} Cmd:{}", pNetworkMsg->GetFrom(), pNetworkMsg->GetTo(), pNetworkMsg->GetCmd());
            for(const auto& server_type: pNetworkMsg->GetBroadcastTo())
            {
                TransmitMsgToThisZoneWithServiceType(pNetworkMsg, server_type);
            }
        }
        else if(pNetworkMsg->GetBroadcastType() == BROADCAST_EXCLUDE)
        {
            LOGNETTRACE("TransmitMsgToThisZoneAllPortExcept From:{} To:{} Cmd:{}", pNetworkMsg->GetFrom(), pNetworkMsg->GetTo(), pNetworkMsg->GetCmd());
            std::set<ServiceType_t> exclude_list(pNetworkMsg->GetBroadcastTo().begin(), pNetworkMsg->GetBroadcastTo().end());
            TransmitMsgToThisZoneAllPortExcept(pNetworkMsg, exclude_list);
        }
        else
        {
            LOGNETTRACE("TransmitMsgToThisZoneAllPort From:{} To:{} Cmd:{}", pNetworkMsg->GetFrom(), pNetworkMsg->GetTo(), pNetworkMsg->GetCmd());
            TransmitMsgToThisZoneAllPort(pNetworkMsg);
        }
    }

    //转发给对应的Service
    if(pNetworkMsg->GetMultiTo().empty() == false)
    {
        LOGNETTRACE("SendToMultiTo From:{} To:{} Cmd:{}", pNetworkMsg->GetFrom(), pNetworkMsg->GetTo(), pNetworkMsg->GetCmd());
        for(const auto& vs: pNetworkMsg->GetMultiTo())
        {
            CNetworkMessage send_msg;
            send_msg.CopyRawMessage(*pNetworkMsg);
            send_msg.SetFrom(pNetworkMsg->GetFrom());
            send_msg.SetTo(vs);
            _SendMsgToZonePort(send_msg);
        }
    }
}


void CRouteService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

