#include "RobotClient.h"

#include "NetMSGProcess.h"
#include "NetServerSocket.h"
#include "NetworkService.h"
#include "ProtobuffUtil.h"
#include "RobotClientManager.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "pb_luahelper.h"
#include "protomsg_to_cmd.h"
RobotClient::RobotClient(RobotClientManager* pManager)
    : m_pManager(pManager)
{
}

RobotClient::~RobotClient()
{
    if(auto pServerSocket = m_pServerSocket.lock())
    {
        pServerSocket->Interrupt(true);
        m_pServerSocket.reset();
    }
}

void RobotClient::initInLua(struct lua_State* L)
{
    lua_tinker::class_add<RobotClient>(L, "RobotClient", true);
    lua_tinker::class_def<RobotClient>(L, "IsConnectServer", &RobotClient::IsConnectServer);
    lua_tinker::class_def<RobotClient>(L, "DisconnectServer", &RobotClient::DisconnectServer);
    lua_tinker::class_def<RobotClient>(L, "GetClientID", &RobotClient::GetClientID);
    lua_tinker::class_def<RobotClient>(L, "SetClientID", &RobotClient::SetClientID);
    lua_tinker::class_def<RobotClient>(L, "SendProtobufToServer", &RobotClient::SendProtobufToServer);
    lua_tinker::class_def<RobotClient>(L, "AddEventCallBack", &RobotClient::AddEventCallBack);

    pb_luahelper::init_lua(L);
}

void RobotClient::OnConnected(const CNetSocketSharedPtr& pSocket)
{
    m_pServerSocket = pSocket;
}

void RobotClient::OnConnectFailed(const CNetSocketSharedPtr&)
{
    LOGDEBUG("RobotClient::OnConnectFailed");
}

void RobotClient::OnDisconnected(const CNetSocketSharedPtr&)
{
    m_pServerSocket.reset();

    m_pManager->ExecScript<void>("OnDisconnected", this);
}

void RobotClient::OnAccepted(const CNetSocketSharedPtr&) {}

void RobotClient::OnRecvData(const CNetSocketSharedPtr& pSocket, byte* pBuffer, size_t len)
{
    MSG_HEAD* pHeader = (MSG_HEAD*)pBuffer;
    switch(pHeader->msg_cmd)
    {
        case CMD_INTERRUPT:
        {
            LOGDEBUG("INITATIVE_CLOSE:{}", m_idClient);
            CServerSocketSharedPtr pServerSocket = std::static_pointer_cast<CServerSocket>(pSocket);
            pServerSocket->SetReconnect(false);
            DisconnectServer();
        }
        break;
        case CMD_SC_KEY:
        {
            SC_KEY msg;
            if(msg.ParseFromArray(pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD)) == false)
            {
                LOGERROR("ParseFromArray Fail:{}", CMD_SC_KEY);
            }

            pSocket->InitEncryptor(msg.key());
            LOGMESSAGE("recv SC_KEY");
            m_pManager->ExecScript<void>("OnConnect", this);
        }
        break;
        default:
        {
            // m_pManager->GetNetMessageProcess()->Process(pHeader->msg_cmd, pBuffer + sizeof(MSG_HEAD), len -
            // sizeof(MSG_HEAD), VirtualSocket(0, pSocket->GetSocketIdx()), 0, 0);
            std::string func_name = m_pManager->GetProcessCMD(pHeader->msg_cmd);
            if(func_name.empty() == false)
            {
                LOGDEBUG("process net_msg:{}", pHeader->msg_cmd);
                m_pManager->ExecScript<void>(func_name.c_str(), this, pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD));
                // m_pManager->GetScriptManager()->FullGC();
            }
            else
            {

                LOGDEBUG("not find processer net_msg:{}", pHeader->msg_cmd);
            }
        }
        break;
    }
}

void RobotClient::OnProcessMessage(CNetworkMessage*) {}

void RobotClient::OnRecvTimeout(const CNetSocketSharedPtr&) {}

void RobotClient::AddEventCallBack(uint32_t nWaitMs, const std::string& func_name, bool bPersist)
{
    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     = [func_name, pThis = this]() {
        pThis->m_pManager->ExecScript<void>(func_name.c_str(), pThis);
    };
    param.tWaitTime = nWaitMs;
    param.bPersist  = bPersist;
    m_pManager->GetEventManager()->ScheduleEvent(param, m_Event);
}

bool RobotClient::IsConnectServer()
{
    return m_pServerSocket.expired() == false;
}

void RobotClient::DisconnectServer()
{
    if(auto pServerSocket = m_pServerSocket.lock())
    {
        pServerSocket->Interrupt(true);
        m_pServerSocket.reset();
    }
    auto pClient = shared_from_this();
    m_pManager->DelClient(std::static_pointer_cast<RobotClient>(pClient));
}

void RobotClient::SendToServer(const CNetworkMessage& msg)
{
    if(auto pServerSocket = m_pServerSocket.lock())
        pServerSocket->SendNetworkMessage(msg);
}

void RobotClient::SendProtobufToServer(proto_msg_t* pMsg)
{
    CHECK(pMsg);
    CNetworkMessage _msg(msg_to_cmd(*pMsg), *pMsg);
    SendToServer(_msg);
}
