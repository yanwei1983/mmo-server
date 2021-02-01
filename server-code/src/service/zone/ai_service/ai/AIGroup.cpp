#include "AIGroup.h"

CAIGroup::CAIGroup() {}

CAIGroup::~CAIGroup() {}

bool CAIGroup::Init(uint32_t group_id, CAIPhase* pScene)
{
    m_GroupID = group_id;
    m_pScene  = pScene;
    return true;
}

void CAIGroup::AddMember(CAIActor* pActor)
{
    m_setMember.insert(pActor);
}

void CAIGroup::RemoveMember(CAIActor* pActor)
{
    m_setMember.erase(pActor);
}

void CAIGroup::Foreach(const std::function<void(CAIActor*)>& func)
{
    __ENTER_FUNCTION
    for(const auto& v: m_setMember)
    {
        func(v);
    }
    __LEAVE_FUNCTION
}

void CAIGroup::FindIF(const std::function<bool(CAIActor*)>& func)
{
    __ENTER_FUNCTION
    for(const auto& v: m_setMember)
    {
        if(func(v) == true)
        {
            return;
        }
    }
    __LEAVE_FUNCTION
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CAIGroupManager::CAIGroupManager() {}

CAIGroupManager::~CAIGroupManager() {}

bool CAIGroupManager::Init(CAIPhase* pScene)
{
    m_pScene = pScene;
    return true;
}

CAIGroup* CAIGroupManager::GetGroup(uint32_t group_id)
{
    auto it = m_setGroup.find(group_id);
    if(it != m_setGroup.end())
    {
        return it->second.get();
    }

    CAIGroup* pGroup = CreateNew<CAIGroup>(group_id, m_pScene);
    CHECKF(pGroup);
    std::unique_ptr<CAIGroup> group_ptr(pGroup);
    m_setGroup.emplace(group_id, std::move(group_ptr));
    return pGroup;
}