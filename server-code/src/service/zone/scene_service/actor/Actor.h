#ifndef ACTOR_H
#define ACTOR_H

#include "BaseCode.h"
#include "MyTimer.h"
#include "NetworkDefine.h"
#include "SceneObject.h"
class CMonster;
class CPlayer;
class SC_AOI_NEW;
class SC_AOI_REMOVE;

class CSkillFSM;
class CActorAttrib;
class CActorStatusSet;
class CEventEntryMap;
class CEventEntryQueue;
class CCoolDownSet;

export_lua class CActor : public CSceneObject
{
protected:
    CActor();
    bool Init();

public:
    virtual ~CActor();

    export_lua bool IsDelThis() const { return m_bDelThis; }
    export_lua void DelThis();

public:
    export_lua bool IsDead() const { return GetHP() == 0; }
    export_lua CActor*  QueryOwner() const;
    export_lua uint32_t GetLastMoveTime() const { return m_tLastMoveTime; }
    export_lua void     SetLastMoveTime(uint32_t val) { m_tLastMoveTime = val; }

    export_lua virtual void AddHide() override;
    export_lua virtual void RemoveHide() override;

    export_lua uint32_t GetCampID() const { return m_idCamp; }
    export_lua void     SetCampID(uint32_t id, uint32_t nSync = SYNC_FALSE);
    export_lua uint16_t GetMapID() const;
    export_lua uint64_t GetSceneIdx() const;

    export_lua virtual uint16_t           GetWorldID() const { return 0; }
    export_lua virtual uint32_t           GetLev() const { return 0; }
    export_lua virtual const std::string& GetName() const
    {
        static const std::string s_Empty;
        return s_Empty;
    }

    export_lua virtual uint32_t GetHP() const { return 0; }
    export_lua virtual uint32_t GetMP() const { return 0; }
    export_lua virtual uint32_t GetFP() const { return 0; }
    export_lua virtual uint32_t GetNP() const { return 0; }
    export_lua virtual uint32_t GetHPMax() const;
    export_lua virtual uint32_t GetMPMax() const;
    export_lua virtual uint32_t GetFPMax() const;
    export_lua virtual uint32_t GetNPMax() const;
    export_lua virtual void     _SetHP(uint32_t v) {}
    export_lua virtual void     _SetMP(uint32_t v) {}
    export_lua virtual void     _SetFP(uint32_t v) {}
    export_lua virtual void     _SetNP(uint32_t v) {}
    export_lua float            GetMoveSpeed() const;

    export_lua virtual void     AddProperty(uint32_t nType, int32_t nVal, uint32_t nSync = SYNC_TRUE);
    export_lua virtual void     SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync = SYNC_TRUE);
    export_lua virtual void     _SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync = SYNC_TRUE);
    export_lua virtual uint32_t GetPropertyMax(uint32_t nType) const;
    export_lua virtual uint32_t GetProperty(uint32_t nType) const;

    export_lua void SendDelayAttribChage();
    export_lua void AddDelayAttribChange(uint32_t nType, uint32_t nVal);

public:
    // AOI相关
    export_lua void SendRoomMessage(const proto_msg_t& msg);
    export_lua void SendRoomMessageExcludeSelf(const proto_msg_t& msg);
    export_lua void SendRoomMessage(const proto_msg_t& msg, uint64_t idExclude);

    export_lua void SendWorldMessage(const proto_msg_t& msg);

    export_lua virtual bool SendMsg(const proto_msg_t& msg) const { return false; }

    void         BroadcastMessageTo(const proto_msg_t& msg, const VirtualSocketMap_t& setSocketMap);
    virtual void MakeShowData(SC_AOI_NEW& msg) {}

    export_lua void SendShowToDealyList();
    export_lua void SendShowTo(CPlayer* pPlayer);
    export_lua void AddDelaySendShowToAllViewPlayer();

public:
    // AOI相关
    export_lua virtual bool IsEnemy(CSceneObject* pTarget) const override { return false; }

    virtual void OnBeforeClearViewList(bool bSendMsgToSelf) override;
    virtual void RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase) override;
    virtual bool UpdateViewList(bool bForce) override;
    void         OnReciveAOIUpdate(const BROADCAST_SET& setBCActorDel, const BROADCAST_SET& setBCActorAdd);

protected:
    void AddDelaySendShowTo(OBJID id);
    void RemoveDelaySendShowTo(OBJID id);

    void OnAOIProcess_ActorAddToAOI(const BROADCAST_SET& setBCActorAdd);
    void OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCActorDe);
    void OnAOIProcess_PosUpdate();

    virtual bool NeedSyncAI() const;

private:
    void BroadcastShowTo(const VirtualSocketMap_t& VSMap);

public:
    export_lua CActorAttrib& GetAttrib() { return *m_ActorAttrib.get(); }
    export_lua const CActorAttrib& GetAttrib() const { return *m_ActorAttrib.get(); }

    export_lua CActorStatusSet* GetStatus() { return m_pStatusSet.get(); }
    export_lua CSkillFSM& GetSkillFSM() { return *m_SkillFSM.get(); }
    export_lua CEventEntryMap& GetEventMapRef() { return *m_EventMap.get(); }
    export_lua CEventEntryQueue& GetEventQueueRef() { return *m_EventQueue.get(); }
    export_lua CCoolDownSet* GetCDSet() const { return m_pCDSet.get(); }

public:
    //重新计算属性
    export_lua virtual void RecalcAttrib(bool bClearCache = false);

public:
    virtual void OnEnterMap(CSceneBase* pScene) override;
    virtual void OnLeaveMap(uint16_t idTargetMap) override;

public:
    export_lua int32_t BeAttack(CActor*  pAttacker,
                                uint32_t idSkill,
                                uint32_t nHitType,
                                uint32_t nPower,
                                uint32_t nMaxPower,
                                uint32_t nPowerAddition,
                                bool     bCanResilience,
                                bool     bCanReflect,
                                bool     bIgnoreDefence);

    virtual void            OnBeAttack(CActor* pAttacker, int32_t nRealDamage);
    export_lua void         UpdateFight();
    export_lua bool         HitTest(CActor* pTarget, uint32_t nHitType);
    export_lua int32_t      CalcDefence(uint32_t nHitType);
    export_lua virtual bool CanDamage(CActor* pTarget) const { return false; }
    export_lua virtual void BeKillBy(CActor* pAttacker);

public:
    export_lua bool MoveTo(const Vector2& posTarget, bool bCheckMove = true);
    export_lua bool CheckCanMove(const Vector2& posTarget, bool bSet = true);
    export_lua bool CanMove();
    export_lua void FlyTo(const Vector2& pos);

    export_lua bool _CastSkill(uint32_t idSkill, OBJID idTarget, const Vector2& targetPos);

public:
protected:
protected:
    uint32_t m_idCamp = 0; //阵营ID

    bool m_bDelThis = false;

    uint32_t                   m_tLastMoveTime = 0;
    std::unique_ptr<CSkillFSM> m_SkillFSM;

    std::unique_ptr<CActorAttrib> m_ActorAttrib; //属性

    std::unique_ptr<CActorStatusSet> m_pStatusSet;

    std::unique_ptr<CEventEntryMap>   m_EventMap;
    std::unique_ptr<CEventEntryQueue> m_EventQueue;

    std::unordered_set<OBJID>              m_setDealySendShow;
    std::unordered_map<uint32_t, uint32_t> m_DelayAttribChangeMap;

    std::unique_ptr<CCoolDownSet> m_pCDSet;
    CMyTimer                      m_tFight; //脱离战斗倒计时
};
#endif /* ACTOR_H */
