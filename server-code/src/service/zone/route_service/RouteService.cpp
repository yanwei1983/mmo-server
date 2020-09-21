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
#include "SettingMap.h"
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

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint8_t idServiceType, uint8_t idServiceIdx)
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
    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };

    if(CreateService(100, this) == false)
        return false;

    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CRouteService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
        }
    }

    return true;
}

void CRouteService::OnPortConnected(CNetSocket* pSocket) {}

void CRouteService::OnPortConnectFailed(CNetSocket*) {}

void CRouteService::OnPortDisconnected(CNetSocket* pSocket) {}

void CRouteService::OnPortAccepted(CNetSocket* pSocket) {}

void CRouteService::OnPortRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len) {}

void CRouteService::OnPortRecvTimeout(CNetSocket* pSocket) {}

ON_SERVERMSG(CRouteService, ServiceRegister)
{
    ServerPort server_port{msg.serverport()};
    LOGMESSAGE("World:{} start", server_port.GetWorldID());
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), true);
    
    if(server_port.GetWorldID() == RouteService()->GetWorldID())//本区
    {
        //转发给0区
        CNetworkMessage send_msg(*pMsg);
        send_msg.SetFrom(RouteService()->GetServerPort());
        send_msg.SetTo(ServerPort(0, ROUTE_SERVICE, 0));
        RouteService()->_SendMsgToZonePort(send_msg);
        //注册30秒发送一次ready
        CEventEntryCreateParam param;
        param.cb = []() {
            RouteService()->SendServiceReady();
        }; 
        param.tWaitTime = 30 * 1000;
        param.bPersist  = true;
        EventManager()->ScheduleEvent(param);
    }
    else if(RouteService()->GetWorldID() == 0) // 0区
    {
        //worldid == 0
        GetMessageRoute()->ReloadServiceInfo(msg.update_time(), server_port.GetWorldID());
        //通知0区所有服
        RouteService()->TransmitMsgToThisZoneAllPortExcept(pMsg, ROUTE_SERVICE);
        //通知所有Route,除了当前这个0区的Route
        RouteService()->TransmitMsgToAllRouteExcept(pMsg, 0);
    }
    else //其他区
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

    if(server_port.GetWorldID() == RouteService()->GetWorldID())//本区
    {
        //转发给0区
        RouteService()->TransmitMsgToPort(ServerPort(0, ROUTE_SERVICE, 0), pMsg);
    }
    else if(RouteService()->GetWorldID() == 0) // 0区
    {
        //world == 0
        //通知0区所有服
        RouteService()->TransmitMsgToThisZoneAllPortExcept(pMsg, ROUTE_SERVICE);
        //通知所有Route,除了当前这个0区的Route
        RouteService()->TransmitMsgToAllRouteExcept(pMsg, 0);
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
    if(pNetworkMsg->GetForward().IsVaild())
    {
        pNetworkMsg->SetTo(pNetworkMsg->GetForward());
        _SendMsgToZonePort(*pNetworkMsg);
        return;
    }

    switch(pNetworkMsg->GetMultiType())
    {
        case MULTITYPE_BROADCAST:
        {
            //转发给所有的World
            time_t now = TimeGetSecond();
            for(const auto& [k, v]: GetMessageRoute()->GetWorldReadyList())
            {
                if(now < v)
                {
                    TransmitMsgToPort(ServerPort(k, WORLD_SERVICE, 0), pNetworkMsg);
                }
            }
        }
        break;
        case MULTITYPE_VIRTUALSOCKET:
        {
            //转发给对应的Service
            CNetworkMessage send_msg(*pNetworkMsg);
            const auto&     refSet = pNetworkMsg->GetMultiTo();
            for(auto it = refSet.begin(); it != refSet.end(); it++)
            {
                const auto& vs = *it;
                send_msg.SetTo(vs);
                _SendMsgToZonePort(send_msg);
            }
        }
        break;
        default:
            break;
    }
}

void CRouteService::OnLogicThreadProc()
{

    constexpr int32_t MAX_PROCESS_PER_LOOP = 1000;
    int32_t           nCount               = 0;
    CNetworkMessage*  pMsg                 = nullptr;
    if(m_pMessagePort)
    {
        // process message_port msg
        while(nCount < MAX_PROCESS_PER_LOOP && m_pMessagePort->TakePortMsg(pMsg))
        {
            OnProcessMessage(pMsg);
            SAFE_DELETE(pMsg);
        }
    }
}

void CRouteService::OnLogicThreadCreate()
{
    tls_pService = this;
}

void CRouteService::OnLogicThreadExit() {}