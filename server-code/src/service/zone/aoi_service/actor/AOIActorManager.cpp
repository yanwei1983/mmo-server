#include "AOIActorManager.h"

#include "AOIActor.h"
#include "AOIPhase.h"
#include "AOIPlayer.h"

CAOIActorManager::CAOIActorManager() {}

CAOIActorManager::~CAOIActorManager()
{
    Destroy();
}

bool CAOIActorManager::Init()
{
    __ENTER_FUNCTION
    m_ActorMap.reserve(GUESS_MAX_ACTOR_COUNT);
    return true;

    __LEAVE_FUNCTION
    return false;
}

void CAOIActorManager::Destroy()
{
    __ENTER_FUNCTION
    for(auto it = m_ActorMap.begin(); it != m_ActorMap.end(); it++)
    {
        CAOIActor* pActor = it->second;
        SAFE_DELETE(pActor);
    }
    m_ActorMap.clear();

    __LEAVE_FUNCTION
}

CAOIActor* CAOIActorManager::QueryActor(OBJID id) const
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return nullptr;

    return itFind->second;

    __LEAVE_FUNCTION
    return nullptr;
}

bool CAOIActorManager::AddActor(CAOIActor* pActor)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(pActor->GetID());
    if(itFind != m_ActorMap.end())
    {
        // log error

        CAOIActor* pOldActor = itFind->second;
        if(pOldActor == pActor)
        {
            return true;
        }
        else
        {
            LOGERROR("Add Actor twice!!!!!!! {} {}", pOldActor->GetID(), pOldActor->GetName().c_str());
            if(pOldActor->GetCurrentScene())
            {
                pOldActor->GetCurrentScene()->LeaveMap(pOldActor);
            }
            SAFE_DELETE(pOldActor);
        }
    }

    m_ActorMap[pActor->GetID()] = pActor;
    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CAOIActorManager::DelActor(CAOIActor* pActor, bool bDelete /* = true*/)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(pActor->GetID());
    if(itFind == m_ActorMap.end())
        return false;

    m_ActorMap.erase(itFind);

    if(bDelete)
        SAFE_DELETE(pActor);
    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CAOIActorManager::DelActorByID(OBJID id, bool bDelete /* = true*/)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return false;

    CAOIActor* pActor = itFind->second;
    m_ActorMap.erase(itFind);
    if(bDelete)
        SAFE_DELETE(pActor);

    return true;

    __LEAVE_FUNCTION
    return false;
}

void CAOIActorManager::OnTimer() {}
