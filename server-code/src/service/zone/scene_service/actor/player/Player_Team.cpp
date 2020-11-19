#include "Player.h"
#include "SceneService.h"
#include "TeamInfoManager.h"
#include "server_msg/server_side.pb.h"

void CPlayer::TeamCreate()
{
    __ENTER_FUNCTION
    if(HasTeam() == true)
        return;

    ServerMSG::TeamCreate msg;
    msg.set_team_id(SceneService()->CreateUID());
    msg.set_leader_id(GetID());
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamInviteMember(OBJID idTarget)
{
    __ENTER_FUNCTION
    if(HasTeam() == false)
        return;

    ServerMSG::TeamInviteMember msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    msg.set_invitee_id(idTarget);
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamAcceptInvite(uint64_t idTeam, OBJID idInviter, bool bResult)
{
    __ENTER_FUNCTION
    if(HasTeam() == true)
        return;

    ServerMSG::TeamAcceptInvite msg;
    msg.set_team_id(idTeam);
    msg.set_inviter_id(idInviter);
    msg.set_invitee_id(GetID());
    msg.set_result(bResult);
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamApplyMember(OBJID idTarget)
{
    __ENTER_FUNCTION
    if(HasTeam() == false)
        return;

    ServerMSG::TeamApplyMember msg;
    msg.set_applicant_id(GetID());
    msg.set_respondent_id(idTarget);
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamAcceptApply(OBJID idApplicant, bool bResult)
{
    __ENTER_FUNCTION
    ServerMSG::TeamAcceptApply msg;
    msg.set_applicant_id(idApplicant);
    msg.set_respondent_id(GetID());
    msg.set_result(bResult);
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamQuit()
{
    __ENTER_FUNCTION
    if(HasTeam() == false)
        return;

    ServerMSG::TeamQuit msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamKickMember(OBJID idMember)
{
    __ENTER_FUNCTION
    if(HasTeam() == false)
        return;
    auto pTeam = TeamManager()->QueryTeam(GetTeamID());
    if(pTeam == nullptr)
        return;
    if(pTeam->IsTeamMember(idMember) == false)
        return;
    if(pTeam->IsTeamLeader(GetID()) == false)
        return;

    ServerMSG::TeamKickMember msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    msg.set_kick_id(idMember);
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}

void CPlayer::TeamChangeLeader(OBJID idMember)
{
    __ENTER_FUNCTION
    if(HasTeam() == false)
        return;
    auto pTeam = TeamManager()->QueryTeam(GetTeamID());
    CHECK(pTeam);
    if(pTeam->IsTeamLeader(GetID()) == false)
        return;

    if(pTeam->IsTeamMember(idMember) == false)
        return;

    ServerMSG::TeamNewLeader msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    msg.set_new_leader_id(idMember);
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}