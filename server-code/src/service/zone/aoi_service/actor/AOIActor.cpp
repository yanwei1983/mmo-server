#include "AOIActor.h"

#include "AOIPlayer.h"
#include "AOIScene.h"
#include "AOIActorManager.h"

#include "AOIService.h"
#include "server_msg/server_side.pb.h"
CAOIActor::CAOIActor() {}

CAOIActor::~CAOIActor() {}

bool CAOIActor::Init(const ServerMSG::ActorCreate& msg)
{
    __ENTER_FUNCTION
    m_nType = static_cast<ActorType>(msg.actortype());
    SetID(msg.actor_id());
    
    m_nMoveSPD = msg.movespd();
    m_bNeedSyncAI = msg.need_sync_ai();
    SetName(msg.name());
    SetCampID(msg.campid());
    SetOwnerID(msg.ownerid());
    SetTeamID(msg.team_id());
    SetGuildID(msg.guild_id());
    _SetPhaseID(msg.phase_id());
   
    SetPos(Vector2(msg.posx(), msg.posy()));
    
    m_bMustSee = msg.must_see();
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CAOIActor::SetProperty(uint32_t nType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_CAMP:
        {
            SetCampID(nVal);
        }
        break;
        default:
            break;
    }
    __LEAVE_FUNCTION
}

float CAOIActor::GetMoveSpeed() const
{
    __ENTER_FUNCTION
    return float(m_nMoveSPD) / static_cast<float>(DEFAULT_MOVE_RADIO);
    __LEAVE_FUNCTION
    return 0.0f;
}

CAOIActor* CAOIActor::QueryOwner() const
{
    __ENTER_FUNCTION
    if(GetOwnerID() == 0)
        return nullptr;
    return AOIActorManager()->QueryActor(GetOwnerID());
    __LEAVE_FUNCTION
    return nullptr;
}

uint32_t CAOIActor::GetCampID() const
{
    __ENTER_FUNCTION
    if(GetOwnerID() == 0)
        return m_idCamp;
    
    auto pOwner = QueryOwner();
    if(pOwner == nullptr)
        return m_idCamp;

    return pOwner->GetCampID();
    __LEAVE_FUNCTION
    return m_idCamp;
} 

