#include "Bullet.h"

#include "ActorAttrib.h"
#include "ActorManager.h"
#include "BulletType.h"
#include "GameEventDef.h"
#include "GameMap.h"
#include "Phase.h"
#include "Scene.h"
#include "SceneService.h"
#include "ScriptManager.h"
#include "SkillFSM.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CBullet, s_heap);
CBullet::CBullet()
{
    SetCampID(CAMP_PLAYER);
}

CBullet::~CBullet()
{
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);
}

bool CBullet::Init(OBJID idOwner, const CBulletType* pType, OBJID idTarget, const Vector2& posTarget)
{
    __ENTER_FUNCTION
    CHECKF(pType);
    CHECKF(idOwner);
    m_idOwner = idOwner;
    m_pType   = pType;

    SetID(ActorManager()->GenBulletID());
    CHECKF(CActor::Init());
    m_idTarget  = idTarget;
    m_posTarget = posTarget;

    CActor* pOwner = ActorManager()->QueryActor(m_idOwner);
    if(pOwner)
    {
        _SetPhaseID(pOwner->GetPhaseID());
    }

    if(m_pType->GetMoveSPD() != 0)
    {
        CEventEntryCreateParam param;
        param.evType    = EVENTID_BULLET_MOVESTEP;
        param.cb        = std::bind(&CBullet::MoveStep, this);
        param.tWaitTime = 500;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());
    }
    _SetHP(1);
    m_ActorAttrib->get_base(ATTRIB_HP_MAX) = 1;

    m_ActorAttrib->get_base(ATTRIB_MOVESPD) = pType->GetMoveSPD();
    ScheduleApply();
    return true;

    __LEAVE_FUNCTION
    return false;
}

void CBullet::MakeShowData(SC_AOI_NEW& msg)
{
    msg.set_scene_idx(GetSceneIdx());

    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_BULLET);
    msg.set_prof(m_idType);
    msg.set_lev(GetLev());

    msg.set_campid(GetCampID());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());
}

void CBullet::OnEnterMap(CSceneBase* pScene)
{
    CActor::OnEnterMap(pScene);
    
    ServerMSG::ActorCreate aoi_msg;
    aoi_msg.set_actor_id(GetID());
    aoi_msg.set_scene_id(GetSceneIdx());
    aoi_msg.set_actortype(ACT_BULLET);
    aoi_msg.set_prof(GetTypeID());
    aoi_msg.set_lev(GetLev());
    aoi_msg.set_phase_id(GetPhaseID());
    aoi_msg.set_campid(GetCampID());
    aoi_msg.set_name(GetName());
    
    aoi_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    aoi_msg.set_posx(GetPosX());
    aoi_msg.set_posy(GetPosY());
    aoi_msg.set_ownerid(GetOwnerID());
    SceneService()->SendProtoMsgToAOIService(aoi_msg);

    //aoi_msg.set_hp(GetHP());
    //aoi_msg.set_hpmax(GetHPMax());
    //aoi_msg.set_mp(GetMP());
    //aoi_msg.set_mpmax(GetMPMax());
    //AI暂时不需要Bullet
    //SceneService()->SendProtoMsgToAIService(aoi_msg);
    
}

void CBullet::OnLeaveMap(uint16_t idTargetMap)
{
    CActor::OnLeaveMap(idTargetMap);
}

bool CBullet::CanDamage(CActor* pTarget) const
{
    __ENTER_FUNCTION
    CActor* pOwner = ActorManager()->QueryActor(m_idOwner);
    if(pOwner)
        return pOwner->CanDamage(pTarget);
    __LEAVE_FUNCTION
    return false;
}

void CBullet::BeKillBy(CActor* pAttacker) {}

bool CBullet::IsEnemy(CSceneObject* pTarget) const
{
    __ENTER_FUNCTION
    CActor* pOwner = ActorManager()->QueryActor(m_idOwner);
    if(pOwner)
        return pOwner->IsEnemy(pTarget);
    __LEAVE_FUNCTION
    return false;
}

void CBullet::ScheduleApply()
{
    __ENTER_FUNCTION
    if(m_pType->GetApplyTimes() == 0)
        return;

    if(m_nApplyTimes < m_pType->GetApplyTimes())
    {
        uint32_t               next_apply_time = m_pType->GetApplyMS() + m_pType->GetApplyAdjMS() * m_nApplyTimes;
        CEventEntryCreateParam param;
        param.evType    = EVENTID_BULLET_APPLY;
        param.cb        = std::bind(&CBullet::DoApply, this);
        param.tWaitTime = next_apply_time;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());
        m_nApplyTimes++;
    }
    else
    {
        DelThis();
    }
    __LEAVE_FUNCTION
}

void CBullet::DoApply()
{
    __ENTER_FUNCTION
    if(m_pType->GetSkillID())
    {
        CSkillFSM::SkillEffectInRange(GetOwnerID(), m_pType->GetSkillID(), m_idTarget, GetPos(), m_nApplyTimes);
    }

    ScheduleApply();
    __LEAVE_FUNCTION
}

void CBullet::MoveStep()
{
    __ENTER_FUNCTION
    if(m_pType->GetTargetType() == BULLET_TARGET_ACTOR)
    {
        CActor* pTarget = ActorManager()->QueryActor(m_idTarget);
        if(pTarget != nullptr)
        {
            m_posTarget = pTarget->GetPos();
        }
    }

    Vector2 dir  = m_posTarget - GetPos();
    float   fDis = dir.normalise();
    if(fDis < GetMoveSpeed() * 0.5f)
    {
        MoveTo(m_posTarget, false);
        DoApply();
    }
    else
    {
        Vector2 newPos = GetPos() + dir.normalisedCopy() * GetMoveSpeed() * 0.5f;
        MoveTo(newPos, false);
    }
    __LEAVE_FUNCTION
}
