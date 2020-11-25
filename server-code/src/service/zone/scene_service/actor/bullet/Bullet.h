#ifndef BULLET_H
#define BULLET_H
#include "Actor.h"
#include "BaseCode.h"

class CBulletType;
export_lua class CBullet : public CActor
{
protected:
    CBullet();

public:
    CreateNewImpl(CBullet);

public:
    virtual ~CBullet();

    bool     Init(OBJID idOwner, const CBulletType* pType, OBJID idTarget, const Vector2& posTarget);
    uint32_t GetTypeID() const { return m_idType; }

public:
    export_lua virtual ActorType GetActorType() const override { return ActorType::ACT_BULLET; }
    export_lua static ActorType  GetActorTypeStatic() { return ActorType::ACT_BULLET; }
    virtual void                 MakeShowData(SC_AOI_NEW& msg) override;
    virtual void                 OnEnterMap(CSceneBase* pScene) override;
    virtual void                 OnLeaveMap(uint16_t idTargetMap) override;

    export_lua virtual bool  CanDamage(CActor* pTarget) const override;
    export_lua virtual void  BeKillBy(CActor* pAttacker) override;
    export_lua virtual bool  IsEnemy(CSceneObject* pTarget) const override;
    export_lua virtual OBJID GetOwnerID() const override { return m_idOwner; }

    void ScheduleApply();
    void DoApply();
    void MoveStep();

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    OBJID              m_idOwner     = 0;
    uint32_t           m_idType      = 0;
    const CBulletType* m_pType       = nullptr;
    OBJID              m_idTarget    = 0;
    uint32_t           m_nApplyTimes = 0;
    Vector2            m_posTarget;
};
#endif /* BULLET_H */
