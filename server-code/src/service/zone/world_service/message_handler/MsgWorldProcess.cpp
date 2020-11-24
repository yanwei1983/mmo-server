#include "MsgWorldProcess.h"

#include "AccountManager.h"
#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_SERVERMSG(CWorldService, PlayerChangeZone)
{
    CUser* pUser = UserManager()->QueryUser(msg.socket());
    if(pUser == nullptr)
    {
        return;
    }

    pUser->OnChangeZone(msg.idzone());

    //将消息直接转发给对应的zone
    WorldService()->TransmitMsgToPort(ServerPort(WorldService()->GetWorldID(), msg.idzone()), pMsg);
}

ON_SERVERMSG(CWorldService, PlayerChangeZone_Data)
{
    CUser* pUser = UserManager()->QueryUser(msg.socket());
    if(pUser == nullptr)
    {
        return;
    }

    //将消息直接转发给对应的zone
    WorldService()->TransmitMsgToPort(ServerPort(WorldService()->GetWorldID(), pUser->GetZoneID()), pMsg);
}

ON_SERVERMSG(CWorldService, ServiceReady)
{
    WorldService()->OnWaitedServiceReady(ServerPort(msg.serverport()).GetServiceID());
}

ON_SERVERMSG(CWorldService, ServiceCmd)
{
    LOGDEBUG("ServiceCmd recv, cmd:{}", msg.cmds(0).c_str());
}

ON_SERVERMSG(CWorldService, SocketConnect) {}

ON_SERVERMSG(CWorldService, SocketClose)
{
    AccountManager()->Logout(msg.vs());
}

//////////////////////////////////////////////////////////////////////////
void RegisterWorldMessageHandler()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = WorldService()->GetNetMsgProcess();
    for(const auto& [k, v]: MsgProcRegCenter<CWorldService>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
    }

    __LEAVE_FUNCTION
}