#ifndef NPC_H
#define NPC_H

#include "Actor.h"
#include "BaseCode.h"

class CNpcType;
export_lua class CNpc : public CActor, NoncopyableT<CNpc>
{
protected:
    CNpc();

public:
    CreateNewImpl(CNpc);

public:
    virtual ~CNpc();

    bool                                  Init(uint32_t idType);
    export_lua uint32_t                   GetTypeID() const { return m_idType; }
    export_lua virtual uint32_t           GetLev() const override;
    export_lua virtual const std::string& GetName() const override;

public:
    export_lua virtual ActorType GetActorType() const override { return ActorType::ACT_NPC; }
    export_lua static ActorType  GetActorTypeStatic() { return ActorType::ACT_NPC; }
    virtual void                 MakeShowData(SC_AOI_NEW& msg) override;
    virtual void                 OnEnterMap(CSceneBase* pScene) override;
    virtual void                 OnLeaveMap(uint16_t idTargetMap) override;

    export_lua virtual uint32_t GetHP() const override { return m_nHP; };
    export_lua virtual void     _SetHP(uint32_t v) override { m_nHP = v; };

public:
    OBJECTHEAP_DECLARATION(s_heap);
    export_lua void ActiveNpc(CPlayer* pPlayer);
    export_lua void _ActiveNpc(CPlayer* pPlayer);

private:
    uint32_t m_idType = 0;
    uint32_t m_nHP    = 0;

    const CNpcType* m_pType = nullptr;
};
#endif /* NPC_H */
