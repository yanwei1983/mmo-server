#ifndef SceneService_h__
#define SceneService_h__

#include "IService.h"
#include "MyTimer.h"
#include "ServiceComm.h"
#include "game_common_def.h"
#include "BaseCode.h"

class CMapManager;
class CSystemVarSet;
class CActorManager;
class CSceneManager;
class CLoadingThread;
class CGMManager;
class CMonitorMgr;
class CTeamInfoManager;
class CMysqlConnection;
class CNetworkMessage;
class CLUAScriptManager;

export_lua class CSceneService : public IService, public CServiceCommon
{
    CSceneService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CSceneService();
    void Destory();

public:
    void Release() override;
    CreateNewRealeaseImpl(CSceneService);
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }

public:
    export_lua uint16_t   GetZoneID() const { return GetServiceID().GetServiceIdx(); }
    export_lua ServerPort GetAIServerPort() const { return ServerPort(GetWorldID(), AI_SERVICE, GetServiceID().GetServiceIdx()); }
    export_lua ServerPort GetAOIServerPort() const { return ServerPort(GetWorldID(), AOI_SERVICE, GetServiceID().GetServiceIdx()); }
    export_lua bool       IsSharedZone() const { return GetWorldID() == 0; }
    export_lua uint64_t   CreateUID();

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;
    virtual void OnAllWaitedServiceReady() override;
    virtual void OnServiceReadyFromCrash(const ServiceID& service_id) override;

public:
    void   CreateSocketMessagePool(const VirtualSocket& vs);
    void   DelSocketMessagePool(const VirtualSocket& vs);
    void   PushMsgToMessagePool(const VirtualSocket& vs, CNetworkMessage* pMsg);
    size_t GetMessagePoolMsgCount(const VirtualSocket& vs) const;

    std::unique_ptr<CNetworkMessage> PopMsgFromMessagePool(const VirtualSocket& vs);
    //发送消息给World
    export_lua bool SendProtoMsgToWorld(uint16_t idWorld, const proto_msg_t& msg) const;
    //转发消息给其他的zone
    export_lua bool SendProtoMsgToAllScene(const proto_msg_t& msg) const;
    //广播消息给所有的玩家
    export_lua bool SendProtoMsgToAllPlayer(const proto_msg_t& msg) const;

    //发送消息给玩家
    export_lua bool SendProtoMsgToPlayer(const VirtualSocket& vs, const proto_msg_t& msg) const;

    //发送消息给AIService
    export_lua bool SendProtoMsgToAIService(const proto_msg_t& msg) const;

    //发送消息给AOIService
    export_lua bool SendProtoMsgToAOIService(const proto_msg_t& msg) const;

    //发送广播包给玩家
    void _ID2VS(OBJID id, VirtualSocketMap_t& VSMap) const override;

public:
    export_lua CMysqlConnection* GetGameDB(uint16_t nWorldID);
    void                         ReleaseGameDB(uint16_t nWorldID);

    CMysqlConnection* _ConnectGameDB(uint16_t nWorldID, CMysqlConnection* pServerInfoDB);

    export_lua CLUAScriptManager* GetScriptManager() const { return m_pScriptManager.get(); }
    export_lua CMapManager* GetMapManager() const { return m_pMapManager.get(); }
    export_lua CSystemVarSet* GetSystemVarSet() const { return m_pSystemVarSet.get(); }

    export_lua CActorManager* GetActorManager() const { return m_pActorManager.get(); }
    export_lua CSceneManager* GetSceneManager() const { return m_pSceneManager.get(); }
    export_lua CLoadingThread* GetLoadingThread() const { return m_pLoadingThread.get(); }

    export_lua CGMManager* GetGMManager() const { return m_pGMManager.get(); }
    export_lua CTeamInfoManager* GetTeamInfoManager() const { return m_pTeamInfoManager.get(); }

private:
    void ProcessPortMessage();

private:
    CUIDFactory                       m_UIDFactory;
    std::unique_ptr<CMysqlConnection> m_pGlobalDB;
    
    std::unordered_map<uint16_t, std::unique_ptr<CMysqlConnection>> m_GameDBMap;

    CMyTimer m_tLastDisplayTime;
    size_t   m_nMessageProcess = 0;

    std::unique_ptr<CActorManager>  m_pActorManager;
    std::unique_ptr<CSceneManager>  m_pSceneManager;
    std::unique_ptr<CLoadingThread> m_pLoadingThread;

    using MessagePool = std::deque<std::unique_ptr<CNetworkMessage>>;
    std::unordered_map<uint64_t, MessagePool> m_MessagePoolBySocket;

    std::unique_ptr<CGMManager>        m_pGMManager;
    std::unique_ptr<CLUAScriptManager> m_pScriptManager;
    std::unique_ptr<CMapManager>       m_pMapManager;
    std::unique_ptr<CSystemVarSet>     m_pSystemVarSet;

    std::unique_ptr<CTeamInfoManager> m_pTeamInfoManager;

public:
    //配置文件
    DEFINE_CONFIG_SET(CStatusTypeSet);
    DEFINE_CONFIG_SET(CUserAttrSet);
    DEFINE_CONFIG_SET(CDataCountLimitSet);
    DEFINE_CONFIG_SET(CSkillTypeSet);
    DEFINE_CONFIG_SET(CMonsterTypeSet);
    DEFINE_CONFIG_SET(CBulletTypeSet);
    DEFINE_CONFIG_SET(CItemTypeSet);
    DEFINE_CONFIG_SET(CItemAdditionSet);
    DEFINE_CONFIG_SET(CItemFormulaDataSet);
    DEFINE_CONFIG_SET(CItemUpgradeDataSet);
    DEFINE_CONFIG_SET(CSuitEquipSet);
    DEFINE_CONFIG_SET(CTaskTypeSet);
    DEFINE_CONFIG_SET(CAchievementTypeSet);
    DEFINE_CONFIG_SET(CNpcTypeSet);
    DEFINE_CONFIG_SET(CPetTypeSet);
};

export_lua CSceneService* SceneService();
void                      SetSceneServicePtr(CSceneService*);

export_lua inline auto EventManager()
{
    return SceneService()->GetEventManager();
}
export_lua inline auto NetMsgProcess()
{
    return SceneService()->GetNetMsgProcess();
}
export_lua inline auto ScriptManager()
{
    return SceneService()->GetScriptManager();
}
export_lua inline auto ActorManager()
{
    return SceneService()->GetActorManager();
}
export_lua inline auto SceneManager()
{
    return SceneService()->GetSceneManager();
}
export_lua inline auto MapManager()
{
    return SceneService()->GetMapManager();
}
export_lua inline auto SystemVarSet()
{
    return SceneService()->GetSystemVarSet();
}
export_lua inline auto MonitorMgr()
{
    return SceneService()->GetMonitorMgr();
}
export_lua inline auto GMManager()
{
    return SceneService()->GetGMManager();
}
export_lua inline auto TeamManager()
{
    return SceneService()->GetTeamInfoManager();
}

export_lua inline auto StatusTypeSet()
{
    return SceneService()->GetCStatusTypeSet();
}
export_lua inline auto UserAttrSet()
{
    return SceneService()->GetCUserAttrSet();
}
export_lua inline auto DataCountLimitSet()
{
    return SceneService()->GetCDataCountLimitSet();
}
export_lua inline auto SkillTypeSet()
{
    return SceneService()->GetCSkillTypeSet();
}
export_lua inline auto MonsterTypeSet()
{
    return SceneService()->GetCMonsterTypeSet();
}
export_lua inline auto BulletTypeSet()
{
    return SceneService()->GetCBulletTypeSet();
}
export_lua inline auto ItemTypeSet()
{
    return SceneService()->GetCItemTypeSet();
}
export_lua inline auto ItemAdditionSet()
{
    return SceneService()->GetCItemAdditionSet();
}
export_lua inline auto ItemFormulaDataSet()
{
    return SceneService()->GetCItemFormulaDataSet();
}
export_lua inline auto ItemUpgradeDataSet()
{
    return SceneService()->GetCItemUpgradeDataSet();
}
export_lua inline auto SuitEquipSet()
{
    return SceneService()->GetCSuitEquipSet();
}
export_lua inline auto TaskTypeSet()
{
    return SceneService()->GetCTaskTypeSet();
}
export_lua inline auto AchievementTypeSet()
{
    return SceneService()->GetCAchievementTypeSet();
}
export_lua inline auto NpcTypeSet()
{
    return SceneService()->GetCNpcTypeSet();
}
export_lua inline auto PetTypeSet()
{
    return SceneService()->GetCPetTypeSet();
}

#endif // SceneService_h__
