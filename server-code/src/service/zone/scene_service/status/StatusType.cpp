#include "StatusType.h"

bool CStatusType::Init(const Cfg_Status& row)
{
    m_Data = row;
    for(int32_t i = 0; i < m_Data.attrib_change_list_size(); i++)
    {
        m_AttribChangeList.push_back(CActorAttribChange{m_Data.attrib_change_list(i)});
    }
    return true;
}

uint32_t CStatusType::GetID() const
{
    return m_Data.id();
}
uint8_t CStatusType::GetLevel() const
{
    return m_Data.level();
}
uint32_t CStatusType::GetTypeID() const
{
    return m_Data.status_type();
}
uint32_t CStatusType::GetExpireType() const
{
    return m_Data.status_expire_type();
}
uint32_t CStatusType::GetFlag() const
{
    return m_Data.status_flag();
}
int32_t CStatusType::GetPower() const
{
    return m_Data.power();
}
int32_t CStatusType::GetSecs() const
{
    return m_Data.secs();
}
int32_t CStatusType::GetTimes() const
{
    return m_Data.times();
}
int32_t CStatusType::GetMaxTimes() const
{
    return m_Data.max_times();
}
int32_t CStatusType::GetMaxSecs() const
{
    return m_Data.max_secs();
}
OBJID CStatusType::GetScriptID() const
{
    return m_Data.scriptid();
}

const std::vector<CActorAttribChange>& CStatusType::GetAttribChangeList() const
{
    return m_AttribChangeList;
}