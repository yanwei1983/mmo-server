#ifndef AOISERVICE_H
#define AOISERVICE_H

#include "BaseCode.h"
#include "IService.h"
#include "MyTimer.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

class CAOIActorManager;
class CAOISceneManager;
class CMapManager;
class CRemoteIMGuiServer;

class CAOIService : public IService, public CServiceCommon
{
    CAOIService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CAOIService();
    void Destroy();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CAOIService);

    ServerPort GetSceneServerPort() const { return ServerPort(GetWorldID(), SCENE_SERVICE, GetZoneID()); }
    uint16_t   GetZoneID() const { return GetServiceID().GetServiceIdx(); }
    ServerPort GetAIServerPort() const { return ServerPort(GetWorldID(), AI_SERVICE, GetServiceID().GetServiceIdx()); }

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;

    //发送消息给AOIService
    bool SendProtoMsgToSceneService(const proto_msg_t& msg) const;
    bool SendProtoMsgToAIService(const proto_msg_t& msg) const;

public:
    CAOISceneManager* GetAOISceneManager() const { return m_pAOISceneManager.get(); }
    CAOIActorManager* GetAOIActorManager() const { return m_pAOIActorManager.get(); }
    CMapManager*      GetMapManager() const { return m_pMapManager.get(); }
    CRemoteIMGuiServer* GetRemoteIMGui() const { return m_pRemoteIMGui.get(); }
private:
    CMyTimer m_tLastDisplayTime;

    std::unique_ptr<CAOISceneManager> m_pAOISceneManager;
    std::unique_ptr<CAOIActorManager> m_pAOIActorManager;
    std::unique_ptr<CMapManager>      m_pMapManager;
    std::unique_ptr<CRemoteIMGuiServer> m_pRemoteIMGui;

public:
    //配置文件
};

CAOIService* AOIService();
void         SetAOIServicePtr(CAOIService* ptr);
inline auto  EventManager()
{
    return AOIService()->GetEventManager();
}

inline auto AOISceneManager()
{
    return AOIService()->GetAOISceneManager();
}
inline auto AOIActorManager()
{
    return AOIService()->GetAOIActorManager();
}
inline auto NetMsgProcess()
{
    return AOIService()->GetNetMsgProcess();
}
inline auto MapManager()
{
    return AOIService()->GetMapManager();
}
inline auto RemoteIMGui()
{
    return AOIService()->GetRemoteIMGui();
}

#endif /* AOISERVICE_H */
