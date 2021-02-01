#ifndef AIMONSTER_H
#define AIMONSTER_H

#include "AIActor.h"
namespace ServerMSG
{
    class ActorCreate;
}
class CActorAI;
class CMonsterType;
class CAIType;
export_lua class CAIMonster : public CAIActor
{
    CAIMonster();
    bool Init(const ServerMSG::ActorCreate& msg);

public:
    CreateNewImpl(CAIMonster);
    ~CAIMonster();

    virtual ActorType GetActorType() const override { return ActorType::ACT_MONSTER; }
    static ActorType  GetActorTypeStatic() { return ActorType::ACT_MONSTER; }

    export_lua void         SetAISleep(bool bSleep);
    virtual void OnDead() override;
    virtual void OnBorn() override;
    virtual void OnUnderAttack(OBJID idTarget, int32_t nDamage) override;
    export_lua virtual bool IsEnemy(CSceneObject* pTarget) const override;

    export_lua uint32_t            GetGenID() const { return m_idGen; }
    const CMonsterType* Type() const { return m_pType; }

    virtual void OnCastSkillFinish(uint32_t skill_id, uint32_t stun_ms) override;

    export_lua virtual OBJID GetOwnerID() const override { return m_idOwner; }
    export_lua void          SetOwnerID(OBJID val) { m_idOwner = val; }

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    uint32_t            m_idGen;
    OBJID               m_idOwner;
    const CMonsterType* m_pType;
    const CAIType*      m_pAIType;
    CActorAI*           m_pAI;
};
#endif /* AIMONSTER_H */
