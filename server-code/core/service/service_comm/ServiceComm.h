#ifndef SERVICECOMM_H
#define SERVICECOMM_H
#include "BaseCode.h"

#include "NetworkDefine.h"

#include "ServiceDefine.h"
#include "UIDFactory.h"
//#include <google/protobuf/descriptor.pb.h>

class CMessageRoute;
class CMessagePort;
class CMonitorMgr;
class CMysqlConnection;
class CNetworkService;
class CNetMSGProcess;
class CEventManager;
class CNetworkMessage;
class CNormalThread;

namespace db
{
    class tbld_dbinfo;
};

export_lua class CServiceCommon : public NoncopyableT<CServiceCommon>
{
protected:
    CServiceCommon();
    bool Init(const ServerPort& nServerPort);

public:
    virtual ~CServiceCommon();
    void DestoryServiceCommon();

public:
    export_lua VirtualSocket GetServerVirtualSocket() const { return VirtualSocket(m_nServerPort, 0); }
    export_lua const ServerPort& GetServerPort() const { return m_nServerPort; }
    export_lua void              SetServerPort(const ServerPort& val) { m_nServerPort = val; }
    export_lua WorldID_t         GetWorldID() const { return m_nServerPort.GetWorldID(); }
    export_lua const ServiceID& GetServiceID() const { return m_nServerPort.GetServiceID(); }
    export_lua ServiceType_t    GetServiceType() const { return GetServiceID().GetServiceType(); }
    export_lua ServiceIdx_t     GetServiceIdx() const { return GetServiceID().GetServiceIdx(); }

    export_lua CNetworkService* GetNetworkService() const { return m_pNetworkService.get(); }
    export_lua const std::string& GetServiceName() const { return m_ServiceName; }
    void                          SetServiceName(const std::string& val) { m_ServiceName = val; }
    uint32_t                      GetMessageProcess() const { return m_nMessageProcess; }
    void                          SetMessageProcess(uint32_t val) { m_nMessageProcess = val; }

public:
    bool CreateNetworkService();
    bool CreateService(int32_t nWorkInterval /*= 100*/, class CMessagePortEventHandler* pEventHandler = nullptr);

public:
    //开启逻辑线程，如果没有开启IO线程，也会每一个循环触发一次RunOnce
    void         StartLogicThread(int32_t nWorkInterval = 100, const std::string& name = std::string());
    void         StopLogicThread();
    virtual void OnProcessMessage(CNetworkMessage*);
    virtual void OnLogicThreadProc();
    virtual void OnLogicThreadCreate();
    virtual void OnLogicThreadExit();

public:
    bool ListenMessagePort(const std::string& service_name, class CMessagePortEventHandler* pEventHandler = nullptr);

    //发送消息给MessagePort
    bool _SendMsgToZonePort(const CNetworkMessage& msg) const;
    //转发消息给MessagePort
    bool TransmitMsgToPort(const ServerPort& nServerPort, const CNetworkMessage* pMsg) const;
    bool TransmitMsgToSomePort(const std::vector<ServerPort>& setServerPortList, const CNetworkMessage* pMsg) const;
    bool TransmitMsgToThisZoneAllPort(const CNetworkMessage* pMsg) const;
    bool TransmitMsgToThisZoneWithServiceType(const CNetworkMessage* pMsg, ServiceType_t idServiceType) const;
    bool TransmitMsgToThisZoneAllPortExcept(const CNetworkMessage* pMsg, const std::set<ServiceType_t>& setExcept) const;
    bool TransmitMsgToAllRoute(const CNetworkMessage* pMsg) const;
    bool TransmitMsgToAllRouteExcept(const CNetworkMessage* pMsg, const std::set<WorldID_t>& setExcept) const;

public:
    bool SendMsgToVirtualSocket(const VirtualSocket& vsTo, const proto_msg_t& msg) const;
    bool SendProtoMsgToZonePort(const ServerPort& nServerPort, const proto_msg_t& msg) const;
    bool SendBroadcastMsgToPort(const ServerPort& nServerPort, const proto_msg_t& msg) const;
    bool ForwardProtoMsgToPlayer(const ServerPort& nServerPort, uint64_t id_player, const proto_msg_t& msg) const;

    template<class T>
    VirtualSocketMap_t IDList2VSMap(const T& idList, OBJID idExtInclude = 0) const
    {
        VirtualSocketMap_t setSocketMap;
        __ENTER_FUNCTION

        if constexpr(std::is_invocable<T>::value)
        {
            idList([this, &setSocketMap](OBJID id) { _ID2VS(id, setSocketMap); });
        }
        else if constexpr(std::is_same<typename T::value_type, OBJID>::value)
        {
            for(OBJID id: idList)
            {
                _ID2VS(id, setSocketMap);
            }
        }
        else
        {
            for(const auto& [id, v]: idList)
            {
                _ID2VS(id, setSocketMap);
            }
        }

        if(idExtInclude != 0)
            _ID2VS(idExtInclude, setSocketMap);

        __LEAVE_FUNCTION
        return setSocketMap;
    }
    virtual void _ID2VS(OBJID id, VirtualSocketMap_t& VSMap) const {}

    bool SendProtoMsgTo(const VirtualSocketMap_t& setSocketMap, const proto_msg_t& msg) const;
    bool SendProtoMsgTo(const VirtualSocketMap_t& setSocketMap, uint16_t nCmd, const proto_msg_t& msg) const;

public:
    export_lua CEventManager* GetEventManager() const { return m_pEventManager.get(); }
    export_lua CNetMSGProcess* GetNetMsgProcess() const { return m_pNetMsgProcess.get(); }
    export_lua CMonitorMgr* GetMonitorMgr() const { return m_pMonitorMgr.get(); }

public:
    std::unique_ptr<CMysqlConnection>        ConnectGlobalDB(CMysqlConnection* pServerInfoDB);
    std::unique_ptr<CMysqlConnection>        ConnectGlobalDB();
    std::unique_ptr<CMysqlConnection>        ConnectServerInfoDB();
    static std::unique_ptr<db::tbld_dbinfo>  QueryDBInfo(uint16_t nWorldID, CMysqlConnection* pServerInfoDB);
    static std::unique_ptr<CMysqlConnection> ConnectDB(const db::tbld_dbinfo* pInfo);

public:
    void         AddWaitServiceReady(ServiceID&& service_id);
    void         OnWaitedServiceReady(const ServiceID& service_id);
    virtual void OnAllWaitedServiceReady() {}
    virtual void OnServiceReadyFromCrash(const ServiceID& service_id) {}

protected:
    std::unique_ptr<CNetworkService> m_pNetworkService;
    CMessagePort*                    m_pMessagePort;
    ServerPort                       m_nServerPort;

    std::unique_ptr<db::tbld_dbinfo> m_globaldb_info;
    std::unique_ptr<CNormalThread>   m_pLogicThread;
    std::unique_ptr<CEventManager>   m_pEventManager;
    std::unique_ptr<CNetMSGProcess>  m_pNetMsgProcess;
    std::string                      m_ServiceName;
    uint32_t                         m_nMessageProcess = 0;
    std::unique_ptr<CMonitorMgr>     m_pMonitorMgr;

    std::unordered_set<ServiceID> m_setWaitServiceReady;
};

#endif /* SERVICECOMM_H */
