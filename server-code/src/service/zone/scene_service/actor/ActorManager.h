#ifndef ACTORMANAGER_H
#define ACTORMANAGER_H

#include "BaseCode.h"
#include "IDGenPool.h"
#include "NetworkDefine.h"
#include "game_common_def.h"

class CActor;
class CMonster;
class CPlayer;
// max uint64_t = 1844 6744 0737 0955 1614
// PlayerID
// 9999 4 00 0000 0000 0000
// WorldID Type IDX

// PetID
// 9999 3 00 0000 0000 0000
// WorldID Type ZoneID IDX

// MonsterID
// 0000 1 00 0000 0000 0000
//

// NpcID
// 0000 2 00 0000 0000 0000
//

class CActor;
class CPlayer;

class CActorManager : public NoncopyableT<CActorManager>
{
    CActorManager();
    bool Init();

public:
    CreateNewImpl(CActorManager);

public:
    ~CActorManager();

    void Destroy();

    CActor*  QueryActor(OBJID id) const;
    CPlayer* QueryPlayer(const VirtualSocket& vs) const;
    bool     AddActor(CActor* pActor);

    bool DelActor(CActor* pActor, bool bDelete = true);
    bool DelActorByID(OBJID id, bool bDelete = true);

    size_t GetUserCount() const { return m_PlayerRefMap.size(); }
    size_t GetMonsterCount() const { return m_ActorInfos[ACT_MONSTER].m_ActorCount; }
    size_t GetNpcCount() const { return m_ActorInfos[ACT_NPC].m_ActorCount; }
    size_t GetPetCount() const { return m_ActorInfos[ACT_PET].m_ActorCount; }

    OBJID GenNpcID();
    OBJID GenMonsterID();
    OBJID GenMapItemID();
    OBJID GenBulletID();

    void ForeachPlayer(std::function<void(const std::unordered_map<VirtualSocket, CPlayer*>::value_type&)>&& func)
    {
        std::for_each(m_PlayerRefMap.begin(), m_PlayerRefMap.end(), func);
    }

    void ForeachActor(std::function<void(const std::unordered_map<OBJID, CActor*>::value_type&)>&& func)
    {
        std::for_each(m_ActorMap.begin(), m_ActorMap.end(), func);
    }

protected:
private:
    struct ActorGroupInfo
    {
        uint32_t         m_ActorCount = 0;
        IDGenPool<OBJID> m_idPool;
    };
    std::array<ActorGroupInfo, ACT_MAX> m_ActorInfos;

    std::unordered_map<OBJID, CActor*>          m_ActorMap;
    std::unordered_map<VirtualSocket, CPlayer*> m_PlayerRefMap;
};
#endif /* ACTORMANAGER_H */
