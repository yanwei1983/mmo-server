#include "SkillFSM.h"

#include "Actor.h"
#include "ActorManager.h"
#include "ActorStatusSet.h"
#include "Bullet.h"
#include "BulletType.h"
#include "CoolDown.h"
#include "GameLog.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "Player.h"
#include "Scene.h"
#include "SceneBase.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "SkillType.h"
#include "msg/zone_service.pb.h"
CSkillFSM::CSkillFSM() {}

bool CSkillFSM::Init(CActor* pActor)
{
    m_pOwner = pActor;
    return true;
}

CSkillFSM::~CSkillFSM()
{
    m_pEvent.Clear();
}

bool CSkillFSM::CastSkill(uint32_t idSkill, OBJID idTarget, const Vector2& pos)
{
    __ENTER_FUNCTION
    const CSkillType* pSkillType = SkillTypeSet()->QueryObj(idSkill);
    if(pSkillType == nullptr)
        return false;
    CActor* pTarget = ActorManager()->QueryActor(idTarget);
    if(CanIntone(pSkillType, pTarget, pos) == false)
        return false;
    LOGSKILLDEBUG(pSkillType->IsDebug(),
                  "SkillCast ID:{} Skill:{}, Target:{} Pos:{:01f} {:01f}",
                  m_pOwner->GetID(),
                  idSkill,
                  m_idTarget,
                  m_posTarget.x,
                  m_posTarget.y);

    // send room_msg to client
    SC_CASTSKILL msg;
    msg.set_scene_idx(m_pOwner->GetSceneIdx());
    msg.set_actor_id(m_pOwner->GetID());
    msg.set_skill_id(idSkill);
    msg.set_target_id(m_idTarget);
    msg.set_x(m_posTarget.x);
    msg.set_y(m_posTarget.y);
    m_pOwner->SendRoomMessage(msg, true);

    DoIntone(pSkillType);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CSkillFSM::CanIntone(const CSkillType* pSkillType, CActor* pTarget, const Vector2& target_pos)
{
    __ENTER_FUNCTION
    CHECKF(pSkillType);
    if(pSkillType->GetSkillType() == SKILLTYPE_PASSIVE || pSkillType->GetSkillType() == SKILLTYPE_NONE)
        return false;

    if(IsSkillCoolDown(pSkillType->GetCDType()))
    {
        LOGSKILLDEBUG(pSkillType->IsDebug(), "Actor {} SkillCoolDown:{}", m_pOwner->GetID(), pSkillType->GetSkillID());
        return false;
    }

    // Idle状态或者Intone允许再吟唱
    bool bCanIntone = (m_curState == SKILLSTATE_IDLE || (m_curState == SKILLSTATE_INTONE && m_pCurSkillType != nullptr &&
                                                         HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_INTONE_CANREINTONE)));

    if(bCanIntone == false)
    {
        LOGSKILLDEBUG(pSkillType->IsDebug(), "Actor {} SkillCannptIntone:{}", m_pOwner->GetID(), pSkillType->GetSkillID());
        return false;
    }

    if(m_pOwner->GetStatus()->TestStatusByFlag(STATUSFLAG_DISABLE_ATTACK) == true)
        return false;

    switch(pSkillType->GetCastType())
    {
        case SCT_TARGET_SELF:
        {
            m_idTarget  = m_pOwner->GetID();
            m_posTarget = m_pOwner->GetPos();
        }
        break;
        case SCT_TARGET_OTHER:
        {
            CHECKF(pTarget);
            if(pTarget->GetStatus()->TestStatusByFlag(STATUSFLAG_DISABLE_BEATTACK) == true)
                return false;

            m_idTarget  = pTarget->GetID();
            m_posTarget = pTarget->GetPos();
        }
        break;
        case SCT_TARGET_POS:
        {
            if(target_pos.x == 0 && target_pos.y == 0)
                return false;

            m_idTarget  = 0;
            m_posTarget = target_pos;
        }
        break;
    }

    if(pSkillType->GetUseItemType() != 0 && m_pOwner->IsPlayer())
    {
        CPlayer* pPlayer = m_pOwner->CastTo<CPlayer>();
        if(pPlayer->CheckItem(pSkillType->GetUseItemType(), 1) == false)
        {
            LOGSKILLDEBUG(pSkillType->IsDebug(), "Actor {} SkillCheckItemFail:{}", m_pOwner->GetID(), pSkillType->GetSkillID());
            return false;
        }
        if(HasFlag(pSkillType->GetFlag(), SKILLFLAG_SPENDITEM) == true)
        {
            if(pPlayer->SpendItem(LOGITEM_SKILL, pSkillType->GetUseItemType(), 1) == false)
            {
                LOGSKILLDEBUG(pSkillType->IsDebug(), "Actor {} SkillSpendItemFail:{}", m_pOwner->GetID(), pSkillType->GetSkillID());
                return false;
            }
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CSkillFSM::DoIntone(const CSkillType* pSkillType)
{
    __ENTER_FUNCTION
    LOGSKILLDEBUG(pSkillType->IsDebug(), "DoIntone ID:{}", m_pOwner->GetID());

    m_pCurSkillType = pSkillType;
    m_curState      = SKILLSTATE_INTONE;
    if(m_pCurSkillType->GetScirptID() != 0)
    {
        ScriptManager()->TryExecScript<void>(m_pCurSkillType->GetScirptID(), SCB_SKILL_DOINTONE, m_pOwner, m_idTarget, m_posTarget, m_pCurSkillType);
    }

    if(pSkillType->GetIntoneMS() == 0)
    {
        DoLaunch();
    }
    else
    {
        CEventEntryCreateParam param;
        param.evType    = 0;
        param.cb        = std::bind(&CSkillFSM::DoLaunch, this);
        param.tWaitTime = pSkillType->GetIntoneMS();
        param.bPersist  = false;

        EventManager()->ScheduleEvent(param, m_pEvent);
    }
    __LEAVE_FUNCTION
}

bool CSkillFSM::BreakIntone()
{
    __ENTER_FUNCTION
    if(m_curState != SKILLSTATE_INTONE)
        return false;
    CHECKF(m_pCurSkillType);
    if(HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_INTONE_CANBREAK) == false)
        return false;
    LOGSKILLDEBUG(m_pCurSkillType->IsDebug(), "BreakIntone ID:{}", m_pOwner->GetID());
    return _BreakIntone();
    __LEAVE_FUNCTION
    return false;
}

bool CSkillFSM::_BreakIntone()
{
    __ENTER_FUNCTION
    if(m_curState != SKILLSTATE_INTONE)
        return false;

    DoIdle();
    // send room msg
    SC_SKILL_BREAK msg;
    msg.set_scene_idx(m_pOwner->GetSceneIdx());
    msg.set_actor_id(m_pOwner->GetID());
    m_pOwner->SendRoomMessage(msg,true);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CSkillFSM::DoLaunch()
{
    __ENTER_FUNCTION
    LOGSKILLDEBUG(m_pCurSkillType->IsDebug(), "DoLaunch ID:{}", m_pOwner->GetID());
    m_curState    = SKILLSTATE_INTONE;
    m_nApplyTimes = 0;

    if(m_pCurSkillType->GetScirptID() != 0)
        ScriptManager()->TryExecScript<void>(m_pCurSkillType->GetScirptID(), SCB_SKILL_DOLAUNCH, m_pOwner, m_idTarget, m_posTarget, m_pCurSkillType);

    StartSkillCoolDown(m_pCurSkillType->GetCDType(), m_pCurSkillType->GetCDSec());

    if(HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_NOT_UPDATEFIGHTING) == false)
    {
        m_pOwner->UpdateFight();
    }

    if(HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_DAMAGE_BEFORE_LAUNCH) == true)
    {
        SkillEffect();
    }
    ScheduleApply();
    __LEAVE_FUNCTION

}

bool CSkillFSM::BreakLaunch()
{
    __ENTER_FUNCTION
    if(m_curState != SKILLSTATE_LAUNCH)
        return false;
    CHECKF(m_pCurSkillType);
    if(HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_LAUNCH_CANBREAK) == false)
        return false;
    LOGSKILLDEBUG(m_pCurSkillType->IsDebug(), "BreakLaunch ID:{}", m_pOwner->GetID());
    return _BreakLaunch();
    __LEAVE_FUNCTION
    return false;
}
bool CSkillFSM::_BreakLaunch()
{
    __ENTER_FUNCTION
    if(m_curState != SKILLSTATE_LAUNCH)
        return false;

    DoIdle();
    // send room msg
    SC_SKILL_BREAK msg;
    msg.set_scene_idx(m_pOwner->GetSceneIdx());
    msg.set_actor_id(m_pOwner->GetID());
    m_pOwner->SendRoomMessage(msg,true);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CSkillFSM::SetTargetPos(const Vector2& posTarget)
{
    __ENTER_FUNCTION
    if(m_pCurSkillType == nullptr)
        return;
    if(HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_CANCHANGE_DESTPOS) == false)
    {
        return;
    }

    m_posTarget = posTarget;
    __LEAVE_FUNCTION
}

bool CanSkillAttackActor(const CSkillType* pSkillType, CActor* pAttacker, CActor* pTarget)
{
    __ENTER_FUNCTION
    CHECKF(pTarget);
    auto damage_flag = pSkillType->GetDamageFlag();
    if(pTarget->IsMonster() && HasFlag(damage_flag, STF_TARGET_MONSTER) == false)
        return false;
    if(pTarget->IsPlayer() && HasFlag(damage_flag, STF_TARGET_PLAYER) == false)
        return false;
    if(pTarget->IsPet() && HasFlag(damage_flag, STF_TARGET_PET) == false)
        return false;
    if(pTarget->IsDead() && HasFlag(damage_flag, STF_TARGET_DEAD) == false)
        return false;

    if(pAttacker)
    {
        if(pAttacker->IsDead())
            return false;
        if(pAttacker->IsEnemy(pTarget) == false && HasFlag(damage_flag, STF_TARGET_FRIEND) == false)
            return false;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CSkillFSM::FindTarget(CActor* pOwner, const CSkillType* pSkillType, OBJID idTarget, const Vector2& posTarget, std::vector<CActor*>& vecTarget)
{
    __ENTER_FUNCTION
    CHECK(pSkillType);
    CHECK(pOwner);

    switch(pSkillType->GetDamageShape())
    {
        case STF_TARGET_SINGLE:
        {
            CActor* pActor = ActorManager()->QueryActor(idTarget);
            if(pActor)
            {
                if(CanSkillAttackActor(pSkillType, pOwner, pActor) == true)
                {
                    vecTarget.push_back(pActor);
                }
            }
        }
        break;
        case STF_TARGET_RECT:
        {
            FloatRect rect;
            rect.left   = posTarget.x - pSkillType->GetRange() * 0.5f;
            rect.right  = posTarget.x + pSkillType->GetRange() * 0.5f;
            rect.top    = posTarget.y - pSkillType->GetRange() * 0.5f;
            rect.bottom = posTarget.y + pSkillType->GetRange() * 0.5f;

            pOwner->ForeachViewActorList([rect, &vecTarget, pOwner, pSkillType](OBJID idActor) {
                CActor* pActor = ActorManager()->QueryActor(idActor);
                if(pActor)
                {
                    if(CanSkillAttackActor(pSkillType, pOwner, pActor) == true)
                    {
                        if(GameMath::isIntersect(rect, pActor->GetPos(), 0) == true)
                        {
                            vecTarget.push_back(pActor);
                        }
                    }
                }
            });
        }
        break;
        case STF_TARGET_RECT_DIR:
        {
            auto Face       = GameMath::rotate(GameMath::VECTOR2_NORTH, pOwner->GetFace());
            auto FaceNormal = GameMath::rotate(GameMath::VECTOR2_NORTH, pOwner->GetFace() + 90.0f);
            pOwner->ForeachViewActorList([&vecTarget, Face, FaceNormal, pOwner = pOwner, pSkillType = pSkillType](OBJID idActor) {
                CActor* pActor = ActorManager()->QueryActor(idActor);
                if(pActor)
                {
                    if(CanSkillAttackActor(pSkillType, pOwner, pActor) == true)
                    {
                        if(GameMath::Intersection2D::isInABBox(pOwner->GetPos(),
                                                               Face,
                                                               pActor->GetPos(),
                                                               FaceNormal,
                                                               pSkillType->GetWidth() * 0.5f,
                                                               pSkillType->GetRange()) == true)
                        {
                            vecTarget.push_back(pActor);
                        }
                    }
                }
            });
        }
        break;
        case STF_TARGET_ROUND:
        {
            float fSkillRange  = pSkillType->GetRange();
            float fRangeSquare = fSkillRange * fSkillRange;
            pOwner->ForeachViewActorList([&vecTarget, fRangeSquare, pOwner = pOwner, pSkillType = pSkillType](OBJID idActor) {
                CActor* pActor = ActorManager()->QueryActor(idActor);
                if(pActor)
                {
                    if(CanSkillAttackActor(pSkillType, pOwner, pActor) == true)
                    {
                        if(GameMath::simpleDistance(pOwner->GetPos(), pActor->GetPos()) < fRangeSquare)
                        {
                            vecTarget.push_back(pActor);
                        }
                    }
                }
            });
        }
        break;
        case STF_TARGET_FAN:
        {
            float fSkillRange  = pSkillType->GetRange();
            float fRangeSquare = fSkillRange * fSkillRange;
            auto  Face         = GameMath::rotate(GameMath::VECTOR2_NORTH, pOwner->GetFace());

            pOwner->ForeachViewActorList([fRangeSquare, &vecTarget, Face, pOwner, pSkillType](OBJID idActor) {
                CActor* pActor = ActorManager()->QueryActor(idActor);
                if(pActor)
                {
                    if(CanSkillAttackActor(pSkillType, pOwner, pActor) == true)
                    {
                        if(GameMath::simpleDistance(pOwner->GetPos(), pActor->GetPos()) < fRangeSquare)
                        {
                            if(GameMath::Intersection2D::isInFOV(pOwner->GetPos(), Face, pActor->GetPos(), pSkillType->GetWidth()) == true)
                            {
                                vecTarget.push_back(pActor);
                            }
                        }
                    }
                }
            });
        }
        break;
        case STF_TARGET_CUSTOM:
        {
            if(pSkillType->GetScirptID() == 0)
                return;

            pOwner->ForeachViewActorList([&vecTarget, pOwner, pSkillType, posTarget](OBJID idActor) {
                CActor* pActor = ActorManager()->QueryActor(idActor);
                if(pActor)
                {
                    if(CanSkillAttackActor(pSkillType, pOwner, pActor) == true)
                    {
                        if(ScriptManager()
                               ->TryExecScript<bool>(pSkillType->GetScirptID(), SCB_SKILL_ISTARGET, pOwner, posTarget, pSkillType, pActor) == true)
                        {
                            vecTarget.push_back(pActor);
                        }
                    }
                }
            });
        }
        break;
    }

    __LEAVE_FUNCTION
}

void _SkillEffectInRange(CActor* pOwner, const CSkillType* pSkillType, OBJID idTarget, const Vector2& posTarget, uint32_t nApplyTimes)
{
    __ENTER_FUNCTION
    CHECK(pOwner);
    SC_SKILL_EFFACT send_msg;
    send_msg.set_scene_idx(pOwner->GetSceneIdx());
    send_msg.set_actor_id(pOwner->GetID());
    send_msg.set_skill_id(pSkillType->GetSkillID());
    pOwner->SendRoomMessage(send_msg);

    std::vector<CActor*> vecTarget;
    CSkillFSM::FindTarget(pOwner, pSkillType, idTarget, posTarget, vecTarget);
    CSkillFSM::DoMultiDamage(pOwner, pSkillType, idTarget, posTarget, vecTarget);
    CSkillFSM::AddBullet(pOwner, pSkillType->GetBulletTypeID(), idTarget, posTarget, vecTarget);
    CSkillFSM::AttachStatus(pOwner, pSkillType, vecTarget);
    if(pSkillType->GetScirptID() != 0)
        ScriptManager()->TryExecScript<void>(pSkillType->GetScirptID(), SCB_SKILL_DOAPPLY, pOwner, idTarget, posTarget, pSkillType, nApplyTimes);
    __LEAVE_FUNCTION
}

void CSkillFSM::SkillEffectInRange(OBJID idCaster, uint32_t idSkillType, OBJID idTarget, const Vector2& posTarget, uint32_t nApplyTimes)
{
    __ENTER_FUNCTION
    CActor*           pOwner     = ActorManager()->QueryActor(idCaster);
    const CSkillType* pSkillType = SkillTypeSet()->QueryObj(idSkillType);
    CHECK(pSkillType);
    _SkillEffectInRange(pOwner, pSkillType, idTarget, posTarget, nApplyTimes);
    __LEAVE_FUNCTION
}

void CSkillFSM::SkillEffect()
{
    __ENTER_FUNCTION
    LOGSKILLDEBUG(m_pCurSkillType->IsDebug(), "SkillEffect ID:{}", m_pOwner->GetID());

    //技能作用
    _SkillEffectInRange(m_pOwner, m_pCurSkillType, m_idTarget, m_posTarget, m_nApplyTimes);
    if(m_pCurSkillType)
        m_pOwner->GetStatus()->OnSkill(m_pCurSkillType->GetSkillID());
    __LEAVE_FUNCTION
}

void CSkillFSM::DoApply()
{
    __ENTER_FUNCTION
    SkillEffect();
    ScheduleApply();
    __LEAVE_FUNCTION
}

void CSkillFSM::ScheduleApply()
{
    __ENTER_FUNCTION
    if(m_nApplyTimes < m_pCurSkillType->GetApplyTimes())
    {
        uint32_t next_apply_time = m_pCurSkillType->GetApplyMS() + m_pCurSkillType->GetApplyAdjMS() * m_nApplyTimes;

        CEventEntryCreateParam param;
        param.evType    = 0;
        param.cb        = std::bind(&CSkillFSM::DoApply, this);
        param.tWaitTime = next_apply_time;
        param.bPersist  = false;

        EventManager()->ScheduleEvent(param, m_pEvent);
        m_nApplyTimes++;
    }
    else
    {
        if(HasFlag(m_pCurSkillType->GetFlag(), SKILLFLAG_DAMAGE_AFTER_LAUNCH) == true)
        {
            SkillEffect();
        }
        DoStun();
    }
    __LEAVE_FUNCTION
}

void CSkillFSM::DoStun()
{
    __ENTER_FUNCTION
    CHECK(m_pCurSkillType);
    LOGSKILLDEBUG(m_pCurSkillType->IsDebug(), "DoStun ID:{}", m_pOwner->GetID());
    SC_SKILL_STUN msg;
    msg.set_scene_idx(m_pOwner->GetSceneIdx());
    msg.set_actor_id(m_pOwner->GetID());
    msg.set_skill_id(m_pCurSkillType->GetSkillID());
    msg.set_stun_ms(m_pCurSkillType->GetStunMS());
    m_pOwner->SendMsg(msg);

    m_curState = SKILLSTATE_STUN;
    if(m_pCurSkillType->GetStunMS() == 0)
    {
        DoIdle();
    }
    else
    {
        CEventEntryCreateParam param;
        param.evType    = 0;
        param.cb        = std::bind(&CSkillFSM::DoIdle, this);
        param.tWaitTime = m_pCurSkillType->GetIntoneMS();
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, m_pEvent);
    }
    __LEAVE_FUNCTION
}

void CSkillFSM::DoIdle()
{
    __ENTER_FUNCTION
    if(m_pCurSkillType)
    {
        LOGSKILLDEBUG(m_pCurSkillType->IsDebug(), "DoIdle ID:{}", m_pOwner->GetID());
    }

    m_pCurSkillType = nullptr;
    m_curState      = SKILLSTATE_IDLE;
    m_pEvent.Cancel();
    __LEAVE_FUNCTION
}

void CSkillFSM::StartSkillCoolDown(uint32_t cdType, uint32_t cd_sec)
{
    __ENTER_FUNCTION
    if(m_pOwner->GetCDSet())
        m_pOwner->GetCDSet()->StartCoolDown(COOLDOWN_TYPE_SKILL, cdType, cd_sec);
    __LEAVE_FUNCTION
}

void CSkillFSM::ClearSkillCoolDown(uint32_t cdType)
{
    __ENTER_FUNCTION
    if(m_pOwner->GetCDSet())
        m_pOwner->GetCDSet()->ClearCoolDown(COOLDOWN_TYPE_SKILL, cdType);
    __LEAVE_FUNCTION
}

bool CSkillFSM::IsSkillCoolDown(uint32_t cdType) const
{
    __ENTER_FUNCTION
    if(m_pOwner->GetCDSet())
        return m_pOwner->GetCDSet()->IsCoolDown(COOLDOWN_TYPE_SKILL, cdType, TimeGetMillisecond());
    __LEAVE_FUNCTION
    return false;
}

void CSkillFSM::DoMultiDamage(CActor*                     pOwner,
                              const CSkillType*           pSkillType,
                              OBJID                       idTarget,
                              const Vector2&              posTarget,
                              const std::vector<CActor*>& vecTarget)
{
    __ENTER_FUNCTION
    CHECK(pOwner);
    CHECK(pSkillType);
    if(vecTarget.empty() == false)
    {
        SC_SKILL_DAMAGE msg;
        msg.set_actor_id(pOwner->GetID());
        msg.set_skill_id(pSkillType->GetSkillID());
        for(size_t i = 0; i < vecTarget.size(); i++)
        {
            CActor* pActor  = vecTarget[i];
            int32_t nDamage = DoDamage(pOwner, pSkillType, pActor, idTarget, posTarget);
            if(nDamage != DR_NOTARGET && nDamage != DR_NODAMAGE)
            {
                auto refData = msg.add_damagelist();
                refData->set_target_id(pActor->GetID());
                refData->set_damage(nDamage);

                if(msg.damagelist_size() > 64)
                {
                    pOwner->SendMsg(msg);
                    msg.clear_damagelist();
                }
            }
        }
        if(msg.damagelist_size() > 0)
            pOwner->SendMsg(msg);
    }
    __LEAVE_FUNCTION
}

int32_t CSkillFSM::DoDamage(CActor* pOwner, const CSkillType* pSkillType, CActor* pTarget, OBJID idTarget, const Vector2& posTarget)
{
    __ENTER_FUNCTION
    CHECK_RET(pOwner, DR_NOTARGET);
    CHECK_RET(pSkillType, DR_NOTARGET);
    CHECK_RET(pTarget, DR_NOTARGET)

    if(HasFlag(pSkillType->GetFlag(), SKILLFLAG_IGNORE_HITRATE) == false && pOwner->HitTest(pTarget, pSkillType->GetHitType()) == false)
    {
        //命中失败
        return DR_MISS;
    }

    if(pSkillType->GetScirptID() != 0)
        ScriptManager()->TryExecScript<void>(pSkillType->GetScirptID(), SCB_SKILL_DODAMAGE, pOwner, idTarget, posTarget, pSkillType, pTarget);

    int32_t nDamage = DR_NODAMAGE;

    if(HasFlag(pSkillType->GetFlag(), SKILLFLAG_IGNORE_DAMAGE) == false)
    {
        uint32_t power          = pSkillType->GetPower();
        uint32_t max_power      = pSkillType->GetMaxPower();
        uint32_t power_addition = pSkillType->GetPowerAddition();
        bool     bCanResilience = HasFlag(pSkillType->GetFlag(), SKILLFLAG_CANNOT_RESILIENCE) == false;
        bool     bCanReflect    = HasFlag(pSkillType->GetFlag(), SKILLFLAG_CANNOT_REFLECT) == false;
        bool     bIgnoreDefence = HasFlag(pSkillType->GetFlag(), SKILLFLAG_IGNORE_DEFENCE) == true;
        //根据距离计算伤害
        if(HasFlag(pSkillType->GetFlag(), SKILLFLAG_DAMAGE_BY_DIST) == true)
        {
            float fDis = GameMath::distance(pTarget->GetPos(), pOwner->GetPos());
            power      = float(power) * fDis / pSkillType->GetRange();
        }

        nDamage = pTarget->BeAttack(pOwner,
                                    pSkillType->GetSkillID(),
                                    pSkillType->GetHitType(),
                                    power,
                                    max_power,
                                    power_addition,
                                    bCanResilience,
                                    bCanReflect,
                                    bIgnoreDefence);
    }

    if(HasFlag(pSkillType->GetFlag(), SKILLFLAG_BREAK_INTONE) == true)
    {
        pTarget->GetSkillFSM()._BreakIntone();
    }
    if(HasFlag(pSkillType->GetFlag(), SKILLFLAG_BREAK_LAUNCH) == true)
    {
        pTarget->GetSkillFSM()._BreakLaunch();
    }

    return nDamage;
    __LEAVE_FUNCTION
    return 0;
}

void CSkillFSM::AttachStatus(CActor* pOwner, const CSkillType* pSkillType, const std::vector<CActor*>& vecTarget)
{
    __ENTER_FUNCTION
    const auto& rowData = pSkillType->GetDataRef();
    for(int i = 0; i < rowData.detach_status_id_size(); i++)
    {
        const auto& status_id = rowData.detach_status_id(i);
        for(size_t i = 0; i < vecTarget.size(); i++)
        {
            CActor* pActor = vecTarget[i];
            pActor->GetStatus()->DetachStatus(status_id);
        }
    }

    for(int i = 0; i < rowData.detach_status_type_size(); i++)
    {
        const auto& status_type = rowData.detach_status_type(i);
        for(size_t i = 0; i < vecTarget.size(); i++)
        {
            CActor* pActor = vecTarget[i];
            pActor->GetStatus()->DetachStatusByType(status_type);
        }
    }

    for(int i = 0; i < rowData.detach_status_flag_size(); i++)
    {
        const auto& status_flag = rowData.detach_status_flag(i);
        for(size_t i = 0; i < vecTarget.size(); i++)
        {
            CActor* pActor = vecTarget[i];
            pActor->GetStatus()->DetachStatusByFlag(status_flag);
        }
    }

    for(int i = 0; i < rowData.attach_status_size(); i++)
    {
        const auto& status_id = rowData.attach_status(i);
        for(size_t i = 0; i < vecTarget.size(); i++)
        {
            CActor* pActor = vecTarget[i];
            pActor->GetStatus()->AttachStatus(status_id, pOwner->GetID());
        }
    }
    __LEAVE_FUNCTION
}

void CSkillFSM::AddBullet(CActor* pOwner, uint32_t idBulletType, OBJID idTarget, const Vector2& posTarget, const std::vector<CActor*>& vecTarget)
{
    __ENTER_FUNCTION
    if(idBulletType == 0)
        return;

    CHECK(pOwner);
    const Vector2&     posBorn = pOwner->GetPos();
    const CBulletType* pType   = BulletTypeSet()->QueryObj(idBulletType);
    CHECK(pType);

    switch(pType->GetEmitType())
    {
        case EMIT_NUM:
        {
            for(size_t i = 0; i < pType->GetNum(); i++)
            {
                CBullet* pBullet = CBullet::CreateNew(pOwner->GetID(), pType, idTarget, posTarget);
                if(pBullet)
                {
                    ActorManager()->AddActor(pBullet);
                    pOwner->GetCurrentScene()->EnterMap(pBullet, posBorn.x, posBorn.y, 0.0f);
                }
            }
        }
        break;
        case EMIT_PRETARGET_ONE:
        {
            for(auto pTarget: vecTarget)
            {
                CBullet* pBullet = CBullet::CreateNew(pOwner->GetID(), pType, pTarget->GetID(), pTarget->GetPos());
                if(pBullet)
                {
                    ActorManager()->AddActor(pBullet);
                    pOwner->GetCurrentScene()->EnterMap(pBullet, posBorn.x, posBorn.y, 0.0f);
                }
            }
        }
        break;
        case EMIT_PRETARGET_NUM:
        {

            for(auto pTarget: vecTarget)
            {
                for(size_t i = 0; i < pType->GetNum(); i++)
                {
                    CBullet* pBullet = CBullet::CreateNew(pOwner->GetID(), pType, pTarget->GetID(), pTarget->GetPos());
                    if(pBullet)
                    {
                        ActorManager()->AddActor(pBullet);
                        pOwner->GetCurrentScene()->EnterMap(pBullet, posBorn.x, posBorn.y, 0.0f);
                    }
                }
            }
        }
        break;
    }
    __LEAVE_FUNCTION
}

void CSkillFSM::Stop()
{
    DoIdle();
}
