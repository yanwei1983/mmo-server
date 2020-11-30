#ifndef MSGPROCESSREGISTER_H
#define MSGPROCESSREGISTER_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

#include "BaseCode.h"
#include "NetworkMessage.h"
#include "NetMSGProcess.h"

template<class T, class FuncType>
void ProcessMsg(CNetworkMessage* pMsg, FuncType func)
{
    __ENTER_FUNCTION

    T msg;
    if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
        return;
    std::invoke(func, msg, pMsg);

    __LEAVE_FUNCTION
}

using MsgProcessFunc = std::function<void(CNetworkMessage*)>;

template<class Service>
struct MsgProcRegCenter
{
    static MsgProcRegCenter<Service>& instance()
    {
        static MsgProcRegCenter<Service> s_instance;
        return s_instance;
    }
    void reg(int32_t cmd, const char* name, MsgProcessFunc&& func)
    {
        CHECK_FMT(m_MsgProc.find(cmd) == m_MsgProc.end(), "dup register msg:{}", cmd);
        m_MsgProc.emplace(cmd, std::make_tuple(name, std::move(func)));
    }
    std::map<uint32_t, std::tuple<const char*, MsgProcessFunc> > m_MsgProc;
};

template<class Service_T>
inline void RegisterAllMsgProcess(CNetMSGProcess* pNetMsgProcess)
{
    for(const auto& [k, v]: MsgProcRegCenter<Service_T>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
    }
}

template<class Service>
struct MsgProcRegister
{
    MsgProcRegister(int32_t cmd, const char* name, MsgProcessFunc&& func) { MsgProcRegCenter<Service>::instance().reg(cmd, name, std::move(func)); }
};

#define TO_CSTR(v) #v

#define ON_MSG(Service, MsgType)                                                                              \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg);                                          \
                                                                                                              \
    MsgProcRegister<Service> register_##MsgType(                                                              \
        CMD_##MsgType,                                                                                        \
        TO_CSTR(CMD_##MsgType),                                                                               \
        std::bind(&ProcessMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
                                                                                                              \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg)

#define ON_SERVERMSG(Service, MsgType)                                                                                   \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg);                                          \
                                                                                                                         \
    MsgProcRegister<Service> register_##MsgType(                                                                         \
        ServerMSG::MsgID_##MsgType,                                                                                      \
        TO_CSTR(ServerMSG::MsgID_##MsgType),                                                                             \
        std::bind(&ProcessMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
                                                                                                                         \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define ON_RAWMSG(Service, MsgID, MsgType)                                                                            \
    void OnMsg_##MsgType(CNetworkMessage* pMsg);                                                                      \
                                                                                                                      \
    MsgProcRegister<Service> register_##MsgType(MsgID, #MsgType, std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
                                                                                                                      \
    void OnMsg_##MsgType(CNetworkMessage* pMsg)

#endif /* MSGPROCESSREGISTER_H */
