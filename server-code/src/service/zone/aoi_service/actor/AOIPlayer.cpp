#include "AOIPlayer.h"

OBJECTHEAP_IMPLEMENTATION(CAOIPlayer, s_heap);
CAOIPlayer::CAOIPlayer() {}

CAOIPlayer::~CAOIPlayer() {}

void CAOIPlayer::ClearTaskPhase()
{
    m_TaskPhase.clear();
}

bool CAOIPlayer::CheckTaskPhase(uint64_t idPhase) const
{
    auto it = m_TaskPhase.find(idPhase);
    if(it != m_TaskPhase.end())
    {
        return it->second == 0;
    }
    return false;
}

void CAOIPlayer::AddTaskPhase(uint64_t idPhase)
{
    auto& refData = m_TaskPhase[idPhase];
    refData++;
}

void CAOIPlayer::RemoveTaskPhase(uint64_t idPhase)
{
    auto& refData = m_TaskPhase[idPhase];
    refData--;
    if(refData == 0)
    {
        m_TaskPhase.erase(idPhase);
    }
}