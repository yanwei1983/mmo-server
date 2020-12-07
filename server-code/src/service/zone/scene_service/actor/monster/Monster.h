#ifndef MONSTER_H
#define MONSTER_H

#include "Actor.h"
#include "HateList.h"

class CMonsterType;
export_lua class CMonster : public CActor
{
protected:
    CMonster();
    bool Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint64_t idPhase, uint32_t idCamp);

public:
    CreateNewImpl(CMonster);

public:
    virtual ~CMonster();

public:
    export_lua bool SendMsg(const proto_msg_t& msg) const override;

    export_lua virtual ActorType GetActorType() const override { return ActorType::ACT_MONSTER; }
    export_lua static ActorType  GetActorTypeStatic() { return ActorType::ACT_MONSTER; }

    export_lua virtual uint32_t           GetLev() const override;
    export_lua virtual const std::string& GetName() const override;
    export_lua uint32_t                   GetTypeID() const;

    export_lua virtual uint32_t GetHP() const override { return m_nHP; };
    export_lua virtual uint32_t GetMP() const override { return m_nMP; };
    export_lua virtual OBJID    GetOwnerID() const override { return m_idOwner; }

    export_lua virtual void _SetHP(uint32_t v) override { m_nHP = v; };
    export_lua virtual void _SetMP(uint32_t v) override { m_nMP = v; };
    export_lua virtual bool CanDamage(CActor* pTarget) const override;
    export_lua virtual void BeKillBy(CActor* pAttacker) override;
    virtual void            OnBeAttack(CActor* pAttacker, int32_t nRealDamage) override;

    virtual void OnEnterMap(CSceneBase* pScene) override;
    virtual void OnLeaveMap(uint16_t idTargetMap) override;

    export_lua bool IsBoss() const;
    export_lua bool IsElit() const;

    export_lua virtual bool IsEnemy(CSceneObject* pActor) const override;
    void                    SetAISleep(bool bVal) {}
    virtual void            MakeShowData(SC_AOI_NEW& msg) override;

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    uint32_t m_nHP = 0;
    uint32_t m_nMP = 0;

    uint32_t            m_idGen;
    OBJID               m_idOwner;
    const CMonsterType* m_pType = nullptr;
    CHateList           m_HateList;
};
#endif /* MONSTER_H */
