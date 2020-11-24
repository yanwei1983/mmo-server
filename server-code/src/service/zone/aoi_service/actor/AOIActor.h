#ifndef AOIACTOR_H
#define AOIACTOR_H

#include "BaseCode.h"
#include "SceneObject.h"
namespace ServerMSG
{
    class ActorCreate;
}
class CAOIMonster;
class CAOIPlayer;

class CAOIActor : public CSceneObject
{
protected:
    CAOIActor();
public:  
    virtual ~CAOIActor();

public:
    CreateNewImpl(CAOIActor);

    virtual bool Init(const ServerMSG::ActorCreate& msg);

public:    
    export_lua virtual ActorType GetActorType() const override{ return m_nType;};
    export_lua uint32_t GetCampID() const;
    export_lua void     SetCampID(uint32_t id) { m_idCamp = id; };
    virtual uint64_t    GetOwnerID() const override { return m_idOwner; }
    void                SetOwnerID(uint64_t val) { m_idOwner = val; }
    uint32_t            GetTeamID() const  { return m_idTeam; }
    void                SetTeamID(uint64_t val) { m_idTeam = val; }
    uint64_t            GetGuildID() const  { return m_idGuild; }
    void                SetGuildID(uint64_t val) { m_idGuild = val; }
    bool                NeedSyncAI()const {return m_bNeedSyncAI;}
    export_lua float GetMoveSpeed() const;

    export_lua const std::string& GetName() const { return m_name; }
    void                          SetName(const std::string& val) { m_name = val; }

    export_lua void SetProperty(uint32_t nType, uint32_t nVal);

public:
public:
public:
    CAOIActor*   QueryOwner() const;
    virtual bool ViewTest(CSceneObject* pActor) const override;
    virtual bool IsEnemy(CSceneObject* pActor) const override;
    
    virtual bool UpdateViewList(bool bForce) override;
    virtual bool IsMustAddToViewList(CSceneObject* pSceneObj) const override;
    virtual void OnAOIProcess(const BROADCAST_SET& setBCAOIActorDel, const BROADCAST_SET& setBCAOIActor, const BROADCAST_SET& setBCAOIActorAdd) override;

private:
    void OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCAOIActorDel);
    void OnAOIProcess_ActorAddToAOI(const BROADCAST_SET& setBCAOIActorAdd);
    void SendAOIChangeToOther(const BROADCAST_SET& setBCAOIActorDel, const BROADCAST_SET& setBCAOIActorAdd);
protected:
    ActorType   m_nType     = ActorType::ACT_UNKNOW;
    uint32_t    m_idCamp   = 0; //阵营ID
    uint64_t    m_idOwner  = 0; //归属ID
    bool        m_bMustSee = false;
    std::string m_name;
    uint32_t    m_nMoveSPD = 0;
    uint32_t    m_idTeam   = 0;
    uint64_t    m_idGuild  = 0;
    bool        m_bNeedSyncAI = false;
};
#endif /* AOIACTOR_H */
