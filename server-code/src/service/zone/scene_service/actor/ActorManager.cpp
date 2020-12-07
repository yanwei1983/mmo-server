#include "ActorManager.h"

#include "Actor.h"
#include "Player.h"

CActorManager::CActorManager() {}

CActorManager::~CActorManager()
{
    Destory();
}

bool CActorManager::Init()
{
    __ENTER_FUNCTION
    m_ActorMap.reserve(GUESS_MAX_ACTOR_COUNT);
    m_PlayerRefMap.reserve(GUESS_MAX_PLAYER_COUNT);

    m_ActorInfos[ACT_NPC].m_idPool.start(ACT_NPC * ID_GEN_FACTOR + 1, ID_GEN_FACTOR);
    m_ActorInfos[ACT_MONSTER].m_idPool.start(ACT_MONSTER * ID_GEN_FACTOR + 1, ID_GEN_FACTOR);
    m_ActorInfos[ACT_MAPITEM].m_idPool.start(ACT_MAPITEM * ID_GEN_FACTOR + 1, ID_GEN_FACTOR);
    m_ActorInfos[ACT_BULLET].m_idPool.start(ACT_BULLET * ID_GEN_FACTOR + 1, ID_GEN_FACTOR);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CActorManager::Destory()
{
    __ENTER_FUNCTION
    LOGDEBUG("CActorManager::Destory");
    for(auto it = m_PlayerRefMap.begin(); it != m_PlayerRefMap.end(); it++)
    {
        CPlayer* pPlayer = it->second;
        pPlayer->SaveInfo();
        SAFE_DELETE(pPlayer);
    }
    for(auto it = m_ActorMap.begin(); it != m_ActorMap.end(); it++)
    {
        CActor* pActor = it->second;
        SAFE_DELETE(pActor);
    }
    m_ActorMap.clear();
    m_PlayerRefMap.clear();
    __LEAVE_FUNCTION
}

CActor* CActorManager::QueryActor(OBJID id) const
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return nullptr;

    if(itFind->second->IsDelThis())
        return nullptr;
    return itFind->second;
    __LEAVE_FUNCTION
    return nullptr;
}

CPlayer* CActorManager::QueryPlayer(const VirtualSocket& vs) const
{
    __ENTER_FUNCTION
    auto it = m_PlayerRefMap.find(vs);
    if(it == m_PlayerRefMap.end())
        return nullptr;
    return it->second;
    __LEAVE_FUNCTION
    return nullptr;
}

bool CActorManager::AddActor(CActor* pActor)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(pActor->GetID());
    if(itFind != m_ActorMap.end())
    {
        // log error

        CActor* pOldActor = itFind->second;
        if(pOldActor == pActor)
        {
            return true;
        }
        else
        {
            LOGERROR("Add Actor twice!!!!!!! {} {}", pOldActor->GetID(), pOldActor->GetName().c_str());
            if(pOldActor->GetActorType() == ACT_PLAYER)
            {

                // log error twice
                CPlayer* pOldPlayer = pOldActor->CastTo<CPlayer>();
                auto     it         = m_PlayerRefMap.find(pOldPlayer->GetSocket());
                if(it != m_PlayerRefMap.end())
                {
                    m_PlayerRefMap.erase(it);
                }
            }

            SAFE_DELETE(pOldActor);
        }
    }

    m_ActorMap[pActor->GetID()] = pActor;
    m_ActorInfos[pActor->GetActorType()].m_ActorCount++;
    if(pActor->IsPlayer())
    {
        CPlayer* pPlayer                     = pActor->CastTo<CPlayer>();
        m_PlayerRefMap[pPlayer->GetSocket()] = pPlayer;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorManager::DelActor(CActor* pActor, bool bDelete /* = true*/)
{
    return DelActorByID(pActor->GetID(), bDelete);
}

bool CActorManager::DelActorByID(OBJID id, bool bDelete /* = true*/)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return false;

    CActor* pActor = itFind->second;
    m_ActorMap.erase(itFind);

    if(pActor->IsPlayer())
    {
        CPlayer* pPlayer      = pActor->CastTo<CPlayer>();
        auto     itFindPlayer = m_PlayerRefMap.find(pPlayer->GetSocket());
        if(itFindPlayer != m_PlayerRefMap.end())
        {
            m_PlayerRefMap.erase(itFindPlayer);
        }
    }
    else
    {
        m_ActorInfos[pActor->GetActorType()].m_idPool.put(id);
    }

    m_ActorInfos[pActor->GetActorType()].m_ActorCount--;

    if(bDelete)
        SAFE_DELETE(pActor);

    return true;

    __LEAVE_FUNCTION
    return false;
}

OBJID CActorManager::GenNpcID()
{
    return m_ActorInfos[ACT_NPC].m_idPool.get();
}

OBJID CActorManager::GenMonsterID()
{
    return m_ActorInfos[ACT_MONSTER].m_idPool.get();
}

OBJID CActorManager::GenMapItemID()
{
    return m_ActorInfos[ACT_MAPITEM].m_idPool.get();
}

OBJID CActorManager::GenBulletID()
{
    return m_ActorInfos[ACT_BULLET].m_idPool.get();
}
