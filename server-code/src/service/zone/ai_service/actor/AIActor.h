#ifndef AIACTOR_H
#define AIACTOR_H

#include "BaseCode.h"
#include "SceneObject.h"

class CAIMonster;
class CAIPlayer;
class CAISkillSet;
class CAIActor : public CSceneObject
{
public:
    CAIActor();
    virtual ~CAIActor();

    bool Init();

public:
    export_lua bool IsDead() const { return GetHP() == 0; }

    export_lua uint32_t GetLastMoveTime() const { return m_LastMoveTime; }
    export_lua void     SetLastMoveTime(uint32_t val) { m_LastMoveTime = val; }
    float               GetMovePassedTime() const;

    export_lua uint32_t GetCampID() const { return m_idCamp; }
    export_lua void     SetCampID(uint32_t id) { m_idCamp = id; };
    export_lua uint32_t GetLev() const { return m_nLev; }
    void                SetLev(uint32_t val) { m_nLev = val; }

    export_lua uint32_t GetHP() const { return m_nHP; }
    void                SetHP(uint32_t val) { m_nHP = val; }
    export_lua uint32_t GetHPMax() const { return m_nHPMax; }
    void                SetHPMax(uint32_t val) { m_nHPMax = val; }
    export_lua uint32_t GetMP() const { return m_nMP; }
    void                SetMP(uint32_t val) { m_nMP = val; }
    export_lua uint32_t GetMPMax() const { return m_nMPMax; }
    void                SetMPMax(uint32_t val) { m_nMPMax = val; }
    export_lua uint32_t GetMoveSPD() const { return m_nMoveSPD; }
    void                SetMoveSPD(uint32_t val) { m_nMoveSPD = val; }

    export_lua float GetMoveSpeed() const;

    export_lua const std::string& GetName() const { return m_name; }
    void                          SetName(const std::string& val) { m_name = val; }

    CAISkillSet& GetSkillSet() { return *m_SkillSet.get(); }

    export_lua void SetProperty(uint32_t nType, uint32_t nVal);

public:
    export_lua void         MoveToTarget(const Vector2& posTarget);
    export_lua void         FlyTo(const Vector2& posTarget);
    export_lua void         CastSkill(uint32_t idSkill, OBJID idTarget);
    export_lua virtual void OnCastSkillFinish(uint32_t stun_ms = 0) {}

public:
    virtual void OnDead() {}
    virtual void OnBorn() {}
    virtual void OnUnderAttack(OBJID idTarget, int32_t nDamage) {}

    void OnCastSkill(uint32_t idSkill);

public:
    export_lua virtual bool IsEnemy(CSceneObject* pTarget) const override { return false; }

public:
    virtual void AddToViewList(CSceneObject* pActor) override;
    virtual bool UpdateViewList(bool bForce) override { return true; } //不需要自动更新ViewList，由Scene通知
protected:
    uint32_t m_idCamp = 0; //阵营ID

    uint32_t m_LastMoveTime = 0;

    std::string m_name;
    uint32_t    m_nLev     = 0;
    uint32_t    m_nHP      = 0;
    uint32_t    m_nHPMax   = 0;
    uint32_t    m_nMP      = 0;
    uint32_t    m_nMPMax   = 0;
    uint32_t    m_nMoveSPD = 0;

    std::unique_ptr<CAISkillSet> m_SkillSet;
};
#endif /* AIACTOR_H */
