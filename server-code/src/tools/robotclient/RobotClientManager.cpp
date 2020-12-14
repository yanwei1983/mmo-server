#include "RobotClientManager.h"

#include <iostream>

#include "NetMSGProcess.h"
#include "NetworkMessage.h"
#include "RobotClient.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "pb_luahelper.h"

void export_to_lua(lua_State* L, void* pManager)
{
    RobotClient::initInLua(L);
    lua_tinker::class_add<RobotClientManager>(L, "RobotClientManager");
    lua_tinker::class_def<RobotClientManager>(L, "ConnectServer", &RobotClientManager::ConnectServer);
    lua_tinker::class_def<RobotClientManager>(L, "DelClient", &RobotClientManager::DelClient);
    lua_tinker::class_def<RobotClientManager>(L, "RegisterCMD", &RobotClientManager::RegisterCMD);
    lua_tinker::class_def<RobotClientManager>(L, "GetProcessCMD", &RobotClientManager::GetProcessCMD);
    lua_tinker::class_def<RobotClientManager>(L, "GetClientCount", &RobotClientManager::GetClientCount);
    lua_tinker::class_def<RobotClientManager>(L, "AddTimedCallback", &RobotClientManager::AddTimedCallback);
    lua_tinker::set(L, "robot_manager", (RobotClientManager*)pManager);

    pb_luahelper::init_lua(L);
    pb_luahelper::export_protobuf_enum_to_lua(L, ACTOR_ATTRIB_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, ACTOR_PROPERTY_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, MONEY_TYPE_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, PK_MODE_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, TalkChannel_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, TaskState_descriptor());

    pb_luahelper::export_protobuf_enum_to_lua(L, CS_CMD_descriptor());
    pb_luahelper::export_protobuf_enum_to_lua(L, SC_CMD_descriptor());
}

RobotClientManager::RobotClientManager(uint32_t nRobStart, uint32_t nRobAmount, const std::string& lua_file_name)
    : m_pNetMsgProcess(std::make_unique<CNetMSGProcess>())
{
    m_pEventManager.reset(CEventManager::CreateNew(GetEVBase(), false));

    m_pScriptManager.reset(CLUAScriptManager::CreateNew("script", export_to_lua, this, "robot_client", lua_file_name.c_str(), false));
    m_pScriptManager->_ExecScript<void>("main", nRobStart, nRobAmount);

    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     = [pScriptManager = m_pScriptManager.get()]() {
        // pScriptManager->OnTimer(TimeGetMonotonic());
        pScriptManager->FullGC();
    };
    param.tWaitTime = 200;
    param.bPersist  = true;

    m_pEventManager->ScheduleEvent(param, m_EventScriptGC);
}

RobotClientManager::~RobotClientManager()
{
    BreakLoop();
    JoinIOThread();
    m_setClient.clear();
    Destroy();
}

RobotClientPtr RobotClientManager::ConnectServer(const char* addr, int32_t port)
{
    RobotClientPtr pClient = std::make_shared<RobotClient>(this);
    auto           pSocket = AsyncConnectTo(addr, port, pClient);
    if(pSocket.expired())
    {
        return nullptr;
    }
    m_setClient.emplace(pClient);
    return pClient;
}

void RobotClientManager::DelClient(const RobotClientPtr& pClient)
{
    m_setClient.erase(pClient);
}

void RobotClientManager::AddTimedCallback(uint32_t tIntervalMS, const std::string& func_name, bool bPersist)
{
    __ENTER_FUNCTION

    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     = [this, _func_name = func_name]() {
        ExecScript<void>(_func_name.c_str(), this);
    };
    param.tWaitTime = tIntervalMS;
    param.bPersist  = bPersist;
    m_pEventManager->ScheduleEvent(param, m_EventList);
    __LEAVE_FUNCTION
}