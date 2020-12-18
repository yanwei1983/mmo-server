#include "TeamInfoManager.h"

#include "Actor.h"
#include "MsgSceneProcess.h"
#include "Player.h"
#include "SceneService.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CTeamInfo, s_heap);

bool CTeamInfo::Init(OBJID idTeam, uint64_t idLeader)
{
    m_idTeam   = idTeam;
    m_idLeader = idLeader;
    return true;
}

void CTeamInfo::OnSetLeader(OBJID idLeader)
{
    m_idLeader = idLeader;
}

void CTeamInfo::OnAddMember(OBJID idMember)
{
    __ENTER_FUNCTION
    CActor* pActor = ActorManager()->QueryActor(idMember);
    if(pActor)
    {
        CPlayer* pPlayer = pActor->CastTo<CPlayer>();
        pPlayer->SetTeamID(m_idTeam);
    }
    m_setMemberID.push_back(idMember);
    __LEAVE_FUNCTION
}

void CTeamInfo::OnDelMember(OBJID idMember)
{
    __ENTER_FUNCTION
    CActor* pActor = ActorManager()->QueryActor(idMember);
    if(pActor)
    {
        CPlayer* pPlayer = pActor->CastTo<CPlayer>();
        pPlayer->SetTeamID(0);
    }
    m_setMemberID.erase(std::find(m_setMemberID.begin(), m_setMemberID.end(), idMember));
    __LEAVE_FUNCTION
}

void CTeamInfo::OnDestroy()
{
    __ENTER_FUNCTION
    for(OBJID idActor: m_setMemberID)
    {
        CActor* pActor = ActorManager()->QueryActor(idActor);
        if(pActor == nullptr)
            continue;
        ;
        CPlayer* pPlayer = pActor->CastTo<CPlayer>();
        pPlayer->SetTeamID(0);
    }

    __LEAVE_FUNCTION
}

size_t CTeamInfo::GetMemeberAmount() const
{
    return m_setMemberID.size();
}

OBJID CTeamInfo::GetMemberIDByIdx(uint32_t idx)
{
    __ENTER_FUNCTION
    CHECKF_V(idx < m_setMemberID.size(), idx);
    return m_setMemberID[idx];
    __LEAVE_FUNCTION
    return idx;
}

bool CTeamInfo::IsTeamMember(OBJID idActor) const
{
    return std::find(m_setMemberID.begin(), m_setMemberID.end(), idActor) != m_setMemberID.end();
}

bool CTeamInfo::IsTeamLeader(OBJID idActor) const
{
    return m_idLeader == idActor;
}

OBJID CTeamInfo::GetTeamLeaderID() const
{
    return m_idLeader;
}

//////////////////////////////////////////////////////////////////////////////
CTeamInfoManager::CTeamInfoManager() {}

bool CTeamInfoManager::Init()
{
    return true;
}

CTeamInfo* CTeamInfoManager::OnCreateTeam(uint64_t idTeam, uint64_t idLeader)
{
    __ENTER_FUNCTION
    auto pTeam = CTeamInfo::CreateNew(idTeam, idLeader);
    CHECKF(pTeam);
    m_setTeam[idTeam] = pTeam;
    return pTeam;
    __LEAVE_FUNCTION
    return nullptr;
}

void CTeamInfoManager::OnDestoryTeam(uint64_t idTeam)
{
    __ENTER_FUNCTION
    auto it = m_setTeam.find(idTeam);
    if(it == m_setTeam.end())
        return;
    auto pTeamInfo = it->second;
    if(pTeamInfo)
    {
        pTeamInfo->OnDestroy();
    }
    m_setTeam.erase(it);
    __LEAVE_FUNCTION
}

CTeamInfo* CTeamInfoManager::QueryTeam(uint64_t idTeam)
{
    __ENTER_FUNCTION
    auto it = m_setTeam.find(idTeam);
    if(it != m_setTeam.end())
        return it->second;
    __LEAVE_FUNCTION
    return nullptr;
}

ON_SERVERMSG(CSceneService, TeamCreate)
{
    TeamManager()->OnCreateTeam(msg.team_id(), msg.leader_id());
}

ON_SERVERMSG(CSceneService, TeamDestory)
{
    TeamManager()->OnDestoryTeam(msg.team_id());
}

ON_SERVERMSG(CSceneService, TeamQuit)
{
    auto pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->OnDelMember(msg.operator_id());
}

ON_SERVERMSG(CSceneService, TeamKickMember)
{
    auto pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->OnDelMember(msg.kick_id());
}

ON_SERVERMSG(CSceneService, TeamNewLeader)
{
    auto pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->OnSetLeader(msg.new_leader_id());
}

ON_SERVERMSG(CSceneService, TeamAddMember)
{
    auto pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->OnAddMember(msg.member_id());
}

//////////////////////////////////////////////////////////////////////////
