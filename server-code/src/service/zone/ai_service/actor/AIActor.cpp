#include "AIActor.h"

#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"
#include "AISkill.h"
#include "server_msg/server_side.pb.h"

CAIActor::CAIActor() {}

CAIActor::~CAIActor() {}

bool CAIActor::Init()
{
    __ENTER_FUNCTION
    m_SkillSet = std::make_unique<CAISkillSet>();
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CAIActor::OnCastSkill(uint32_t idSkill)
{
    m_SkillSet->OnCastSkill(idSkill);
}

void CAIActor::SetProperty(uint32_t nType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_HP:
        {
            SetHP(nVal);
        }
        break;
        case PROP_MP:
        {
            SetMP(nVal);
        }
        break;
        case PROP_FP:
        {
        }
        break;
        case PROP_NP:
        {
        }
        break;
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

void CAIActor::CastSkill(uint32_t idSkill, OBJID idTarget)
{
    __ENTER_FUNCTION
    ServerMSG::ActorCastSkill msg;
    msg.set_actor_id(GetID());
    msg.set_target_id(idTarget);
    msg.set_skill_id(idSkill);

    AIService()->SendProtoMsgToScene(msg);
    __LEAVE_FUNCTION
}

float CAIActor::GetMoveSpeed() const
{
    __ENTER_FUNCTION
    return float(m_nMoveSPD) / static_cast<float>(DEFAULT_MOVE_RADIO);
    __LEAVE_FUNCTION
    return 0.0f;
}

void CAIActor::FlyTo(const Vector2& posTarget)
{
    __ENTER_FUNCTION

    ServerMSG::ActorFlyTo msg;
    msg.set_actor_id(GetID());
    msg.set_x(posTarget.x);
    msg.set_y(posTarget.y);
    AIService()->SendProtoMsgToScene(msg);

    __LEAVE_FUNCTION
}

void CAIActor::MoveToTarget(const Vector2& posTarget)
{
    __ENTER_FUNCTION
    Vector2 dir      = (posTarget - m_Pos);
    float   dis      = dir.normalise();
    Vector2 move_pos = m_Pos;
    if(dis < GetMoveSpeed())
    {
        move_pos = posTarget;
    }
    else
    {
        dir = dir * GetMoveSpeed() * GetMovePassedTime();
        move_pos += dir;
    }

    ServerMSG::ActorMove msg;
    msg.set_actor_id(GetID());
    msg.set_x(move_pos.x);
    msg.set_y(move_pos.y);
    AIService()->SendProtoMsgToScene(msg);

    LOGACTORDEBUG(GetID(), "From {} MoveToTargetE {} {}", m_Pos, msg.x(), msg.y());
    SetLastMoveTime(TimeGetMonotonic());
    __LEAVE_FUNCTION
}

float CAIActor::GetMovePassedTime() const
{
    __ENTER_FUNCTION
    auto  pass_monotonic = TimeGetMonotonic() - m_LastMoveTime;
    float pass_time      = pass_monotonic / 1000.0f;
    if(pass_time > 1.0f)
        return 1.0f;
    else
        return pass_time;

    __LEAVE_FUNCTION
    return 1.0f;
}
