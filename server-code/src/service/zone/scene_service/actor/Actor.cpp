#include "Actor.h"

#include "ActorAttrib.h"
#include "ActorManager.h"
#include "ActorStatusSet.h"
#include "CoolDown.h"
#include "GameEventDef.h"
#include "GameLog.h"
#include "Monster.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneService.h"
#include "SceneTree.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "SkillFSM.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"
#include "server_share/game_common_def.h"

bool IsObservePropertyChange(uint32_t nType, uint32_t nService)
{
    struct OBSERVE_PROPERTY_T
    {
        OBSERVE_PROPERTY_T()
        {
            auto& ref_ai = data[AI_SERVICE];
            ref_ai.insert(PROP_HP);
            ref_ai.insert(PROP_MP);
            ref_ai.insert(PROP_FP);
            ref_ai.insert(PROP_CAMP);

            auto& ref_aoi = data[AOI_SERVICE];
            ref_aoi.insert(PROP_CAMP);
        }

        std::unordered_map<uint32_t, std::unordered_set<uint32_t>> data;
    };
    static const OBSERVE_PROPERTY_T OBSERVE_PROPERTY;
    auto                            it = OBSERVE_PROPERTY.data.find(nService);
    if(it == OBSERVE_PROPERTY.data.end())
        return false;
    return it->second.count(nType) > 0;
}

CActor::CActor() {}

CActor::~CActor() {}

bool CActor::Init()
{
    __ENTER_FUNCTION
    m_pStatusSet.reset(CreateNew<CActorStatusSet>(this));
    CHECKF(m_pStatusSet.get());
    m_SkillFSM.reset(CreateNew<CSkillFSM>(this));
    CHECKF(m_SkillFSM.get());
    m_ActorAttrib = std::make_unique<CActorAttrib>();
    CHECKF(m_ActorAttrib.get());
    m_EventMap = std::make_unique<CEventEntryMap>();
    CHECKF(m_EventMap.get());
    m_EventQueue = std::make_unique<CEventEntryQueue>();
    CHECKF(m_EventQueue.get());

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CActor::DelThis()
{
    __ENTER_FUNCTION
    m_bDelThis = true;
    CEventEntryCreateParam param;
    param.evType = EVENTID_DEL_ACTOR;
    param.cb     = [idActor = GetID()]() {
        ActorManager()->DelActorByID(idActor);
    };
    param.tWaitTime = 0;
    param.bPersist  = false;
    EventManager()->ScheduleEvent(param, GetEventMapRef());
    __LEAVE_FUNCTION
}

void CActor::AddHide()
{
    __ENTER_FUNCTION
    CSceneObject::AddHide();

    ServerMSG::ActorSetHide msg;
    msg.set_actor_id(GetID());
    msg.set_hide_count(m_nHideCount);
    SceneService()->SendProtoMsgToAOIService(msg);
    __LEAVE_FUNCTION
}

void CActor::RemoveHide()
{
    __ENTER_FUNCTION
    CSceneObject::RemoveHide();

    ServerMSG::ActorSetHide msg;
    msg.set_actor_id(GetID());
    msg.set_hide_count(m_nHideCount);
    SceneService()->SendProtoMsgToAOIService(msg);
    __LEAVE_FUNCTION
}

void CActor::SetCampID(uint32_t id, uint32_t nSync)
{
    __ENTER_FUNCTION
    SetProperty(PROP_CAMP, id, nSync);
    __LEAVE_FUNCTION
}

CActor* CActor::QueryOwner() const
{
    __ENTER_FUNCTION
    if(GetOwnerID() == 0)
        return nullptr;
    return ActorManager()->QueryActor(GetOwnerID());
    __LEAVE_FUNCTION
    return nullptr;
}

void CActor::AddProperty(uint32_t nType, int32_t nVal, uint32_t nSync)
{
    __ENTER_FUNCTION
    uint32_t old_val   = GetProperty(nType);
    uint32_t nMaxVal   = GetPropertyMax(nType);
    uint64_t new_val64 = 0;
    if(nVal < 0)
    {
        // sub
        if(old_val > (uint32_t)-nVal)
            new_val64 = old_val + nVal;
        else
            new_val64 = 0;
    }
    else
    {
        // add
        if(nMaxVal - old_val < (uint32_t)(nVal))
            new_val64 = nMaxVal;
        else
            new_val64 = old_val + nVal;
    }

    _SetProperty(nType, (uint32_t)new_val64, nSync);
    __LEAVE_FUNCTION
}
void CActor::SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync)
{
    uint32_t nMaxVal = GetPropertyMax(nType);
    if(nVal >= nMaxVal)
    {
        _SetProperty(nType, nMaxVal, nSync);
        // send notify
    }
    else
    {
        _SetProperty(nType, nVal, nSync);
    }
}
void CActor::_SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync)
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_HP:
        {
            _SetHP(nVal);
        }
        break;
        case PROP_MP:
        {
            _SetMP(nVal);
        }
        break;
        case PROP_FP:
        {
            _SetFP(nVal);
        }
        break;
        case PROP_NP:
        {
            _SetNP(nVal);
        }
        break;
        case PROP_CAMP:
        {
            m_idCamp = nVal;
        }
        break;
        default:
            break;
    }

    if(nSync == SYNC_TRUE)
    {
        // send msg to self
        SC_PROPERTY_CHANGE msg;
        msg.set_actor_id(GetID());
        auto attr = msg.add_datalist();
        attr->set_actype(nType);
        attr->set_val(nVal);
        SendMsg(msg);
        if(IsObservePropertyChange(nType, AI_SERVICE))
        {
            SceneService()->SendProtoMsgToAIService(msg);
        }
        if(IsObservePropertyChange(nType, AOI_SERVICE))
        {
            SceneService()->SendProtoMsgToAOIService(msg);
        }
    }
    else if(nSync == SYNC_ALL)
    {
        // broadcastmsg to viewplayer and self
        SC_PROPERTY_CHANGE msg;
        msg.set_actor_id(GetID());
        auto attr = msg.add_datalist();
        attr->set_actype(nType);
        attr->set_val(nVal);
        SendRoomMessage(msg);
        if(IsObservePropertyChange(nType, AI_SERVICE))
        {
            SceneService()->SendProtoMsgToAIService(msg);
        }
        if(IsObservePropertyChange(nType, AOI_SERVICE))
        {
            SceneService()->SendProtoMsgToAOIService(msg);
        }
    }
    else if(nSync == SYNC_ALL_DELAY)
    {
        // delady broadcastmsg
        AddDelayAttribChange(nType, nVal);
    }

    __LEAVE_FUNCTION
}

uint32_t CActor::GetHPMax() const
{
    return GetAttrib().get(ATTRIB_HP_MAX);
}
uint32_t CActor::GetMPMax() const
{
    return GetAttrib().get(ATTRIB_MP_MAX);
}
uint32_t CActor::GetFPMax() const
{
    return GetAttrib().get(ATTRIB_FP_MAX);
}
uint32_t CActor::GetNPMax() const
{
    return GetAttrib().get(ATTRIB_NP_MAX);
}

uint32_t CActor::GetPropertyMax(uint32_t nType) const
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_LEVEL:
        {
            return 0xFFFF;
        }
        break;
        case PROP_HP:
        {
            return GetHPMax();
        }
        break;
        case PROP_MP:
        {
            return GetMPMax();
        }
        break;
        case PROP_FP:
        {
            return GetFPMax();
        }
        break;
        case PROP_NP:
        {
            return GetNPMax();
        }
        break;
        case PROP_CAMP:
        {
            return 0xFFFFFFFF;
        }
        break;
        default:
            break;
    }

    __LEAVE_FUNCTION
    return 0;
}

uint32_t CActor::GetProperty(uint32_t nType) const
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_LEVEL:
        {
            return GetLev();
        }
        break;
        case PROP_HP:
        {
            return GetHP();
        }
        break;
        case PROP_MP:
        {
            return GetMP();
        }
        break;
        case PROP_FP:
        {
            return GetFP();
        }
        break;
        case PROP_NP:
        {
            return GetNP();
        }
        break;
        default:
            break;
    }

    __LEAVE_FUNCTION
    return 0;
}

void CActor::SendDelayAttribChage()
{
    __ENTER_FUNCTION
    if(m_DelayAttribChangeMap.empty())
        return;
    bool               send_to_ai  = false;
    bool               send_to_aoi = false;
    SC_PROPERTY_CHANGE msg;
    msg.set_actor_id(GetID());
    for(const auto& [k, v]: m_DelayAttribChangeMap)
    {
        auto pInfo = msg.add_datalist();
        pInfo->set_actype(k);
        pInfo->set_val(v);
        if(send_to_ai == false && IsObservePropertyChange(k, AI_SERVICE))
            send_to_ai = true;
        if(send_to_aoi == false && IsObservePropertyChange(k, AOI_SERVICE))
            send_to_aoi = true;
    }
    SendRoomMessage(msg);

    if(send_to_ai)
    {
        SceneService()->SendProtoMsgToAIService(msg);
    }
    if(send_to_aoi)
    {
        SceneService()->SendProtoMsgToAOIService(msg);
    }

    m_DelayAttribChangeMap.clear();
    __LEAVE_FUNCTION
}

void CActor::AddDelayAttribChange(uint32_t nType, uint32_t nVal)
{
    __ENTER_FUNCTION
    m_DelayAttribChangeMap[nType] = nVal;

    auto pEvent = GetEventMapRef().Query(GameEventType::EVENTID_SEND_ATTRIB_CHANGE);
    if(pEvent == nullptr || pEvent->IsCanceled() || pEvent->IsWaitTrigger() == false)
    {
        // 200毫秒后一次性发送
        CEventEntryCreateParam param;
        param.evType = EVENTID_SEND_ATTRIB_CHANGE;
        param.cb     = [pThis = this]() {
            pThis->SendDelayAttribChage();
        };
        param.tWaitTime = 200;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());
    }
    __LEAVE_FUNCTION
}

void CActor::RecalcAttrib(bool bClearCache /*= false*/, bool bNotify /* = true */)
{
    __ENTER_FUNCTION
    uint32_t nOldHPMax = GetHPMax();

    // process status
    m_ActorAttrib->Apply(bClearCache);
    //广播通知hp变化
    if(GetHPMax() != nOldHPMax)
    {
        if(bNotify)
            AddDelayAttribChange(PROP_HP_MAX, GetHPMax());
    }
    if(GetHP() > GetHPMax())
    {
        SetProperty(PROP_HP, GetHPMax(), (bNotify) ? SYNC_ALL_DELAY : SYNC_FALSE);
    }
    if(GetMP() > GetMPMax())
    {
        SetProperty(PROP_MP, GetMPMax(), (bNotify) ? SYNC_TRUE : SYNC_FALSE);
    }
    __LEAVE_FUNCTION
}

float CActor::GetMoveSpeed() const
{
    return (float)(m_ActorAttrib->get(ATTRIB_MOVESPD)) / (float)DEFAULT_MOVE_RADIO;
}

bool CActor::CheckCanMove(const Vector2& posTarget, bool bSet)
{
    __ENTER_FUNCTION
    if(GetCurrentScene()->IsPassDisable(posTarget.x, posTarget.y, GetActorType()) == true)
    {
        LOGACTORTRACE(GetID(), "move to from x:{} y:{} to x:{} y:{} pass_disable", GetPos().x, GetPos().y, posTarget.x, posTarget.y);
        return false;
    }
    uint32_t now = TimeGetMonotonic();
    if(bSet)
        SetLastMoveTime(now);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActor::CanMove()
{
    return GetStatus()->TestStatusByFlag(STATUSFLAG_DISABLE_MOVE) == false;
}

bool CActor::ForceMoveTo(const Vector2& posTarget)
{
    __ENTER_FUNCTION
    _SetPos(posTarget);

    SC_POS_CHANGE msg;
    msg.set_scene_idx(GetSceneIdx());
    msg.set_actor_id(GetID());
    msg.set_posx(GetPosX());
    msg.set_posy(GetPosY());
    SendMsg(msg);
    if(NeedSyncAI())
        SceneService()->SendProtoMsgToAIService(msg);

    UpdateViewList(false);

    m_pStatusSet->OnMove();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActor::MoveTo(const Vector2& posTarget, bool bCheckMove)
{
    __ENTER_FUNCTION
    if(CanMove() == false)
        return false;
    if(bCheckMove && CheckCanMove(posTarget) == false)
        return false;

    _SetPos(posTarget);

    SC_POS_CHANGE msg;
    msg.set_scene_idx(GetSceneIdx());
    msg.set_actor_id(GetID());
    msg.set_posx(GetPosX());
    msg.set_posy(GetPosY());
    SendMsg(msg);
    if(NeedSyncAI())
        SceneService()->SendProtoMsgToAIService(msg);

    UpdateViewList(false);

    m_pStatusSet->OnMove();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActor::_CastSkill(uint32_t idSkill, OBJID idTarget, const Vector2& targetPos)
{
    __ENTER_FUNCTION
    return m_SkillFSM->CastSkill(idSkill, idTarget, targetPos);
    __LEAVE_FUNCTION
    return false;
}

void CActor::OnBeAttack(CActor* pAttacker, int32_t nRealDamage)
{
    __ENTER_FUNCTION
    LOGACTORDEBUG(GetID(), "OnBeAttack: Attacker:{} Damage:{}", (pAttacker) ? pAttacker->GetID() : 0, nRealDamage);

    AddProperty(PROP_HP, -nRealDamage, SYNC_ALL_DELAY);

    m_pStatusSet->OnBeAttack(pAttacker, nRealDamage);
    __LEAVE_FUNCTION
}

int32_t CActor::BeAttack(CActor*  pAttacker,
                         uint32_t idSkill,
                         uint32_t nHitType,
                         uint32_t nPower,
                         uint32_t nMaxPower,
                         uint32_t nPowerAddition,
                         bool     bCanResilience,
                         bool     bCanReflect,
                         bool     bIgnoreDefence)
{
    CHECKF(pAttacker);
    __ENTER_FUNCTION
    //判断是否可以伤害该对象
    if(GetStatus()->TestStatusByFlag(STATUSFLAG_DISABLE_BEATTACK) == true)
        return DR_NOTARGET;
    if(pAttacker->CanDamage(this) == false)
        return DR_NOTARGET;
    if(IsDead() == true)
        return DR_NOTARGET;

    //根据伤害类型，取得防御
    int32_t nDefence = 0;
    if(bIgnoreDefence == false)
    {
        nDefence = CalcDefence(nHitType);
    }
    //伤害=攻击-防御
    int32_t nPowerBase = random_uint32_range(nPower, nMaxPower);
    int32_t nDamage    = nPowerBase + nPowerAddition - nDefence;

    //计算伤害减免
    nDamage = MulDiv(nDamage, GetAttrib().get(ATTRIB_DAMAGE_ADJ) + 10000, 10000);

    //计算伤害反弹
    if(bCanReflect)
    {
        int32_t nReflectRate = GetAttrib().get(ATTRIB_DAMAGE_REFLECT_RATE);
        if(nReflectRate > 0 && random_hit(nReflectRate / 10000.0f))
        {
            //计算反弹伤害
            int32_t nReflectDamage = MulDiv(nDamage, GetAttrib().get(ATTRIB_DAMAGE_REFLECT_ADJ) + 10000, 10000);
            if(pAttacker)
            {
                pAttacker->BeAttack(this, 0, nHitType, 0, 0, nReflectDamage, true, false, false);
            }
        }
    }

    //扣血
    int32_t nRealDamage = std::min<uint32_t>(nDamage, GetHP());
    if(nHitType == HIT_HEAL)
        nRealDamage = -nRealDamage;

    SC_DAMAGE msg;
    msg.set_actor_id(GetID());
    msg.set_attacker_id(pAttacker ? pAttacker->GetID() : 0);
    msg.set_damage(nRealDamage);
    msg.set_hittype(nHitType);
    msg.set_crtical((uint32_t)nPowerBase == nMaxPower);
    SendMsg(msg);

    OnBeAttack(pAttacker, nRealDamage);

    if(pAttacker)
    {
        pAttacker->GetStatus()->OnAttack(this, idSkill, nRealDamage);
    }

    //死亡
    if(GetHP() == 0)
    {
        BeKillBy(pAttacker);
    }
    return nRealDamage;
    __LEAVE_FUNCTION
    return 0;
}

void CActor::UpdateFight()
{
    const int32_t FIGHT_CD_SEC = 5;
    m_tFight.Startup(FIGHT_CD_SEC);
}

bool CActor::HitTest(CActor* pTarget, uint32_t nHitType)
{
    CHECKF(pTarget);
    __ENTER_FUNCTION

    switch(nHitType)
    {
        case HIT_WEAPON:
        {
            int32_t nHit   = GetAttrib().get(ATTRIB_HIT);
            int32_t nDodge = pTarget->GetAttrib().get(ATTRIB_DODGE);
            return random_hit((float)nHit / float(nHit + nDodge));
        }
        break;
        case HIT_MAGE:
        {
            return true;
        }
        break;
        default:
            return false;
    }
    __LEAVE_FUNCTION
    return false;
}

int32_t CActor::CalcDefence(uint32_t nHitType)
{
    __ENTER_FUNCTION
    switch(nHitType)
    {
        case HIT_WEAPON:
        {
            int32_t nMinDef = GetAttrib().get(ATTRIB_MIN_DEF);
            int32_t nMaxDef = GetAttrib().get(ATTRIB_MAX_DEF);
            return random_uint32_range(nMinDef, nMaxDef);
        }
        break;
        case HIT_MAGE:
        {
            int32_t nMinDef = GetAttrib().get(ATTRIB_MIN_MDEF);
            int32_t nMaxDef = GetAttrib().get(ATTRIB_MAX_MDEF);
            return random_uint32_range(nMinDef, nMaxDef);
        }
        break;
        default:
            return 0;
    }
    __LEAVE_FUNCTION
    return 0;
}

void CActor::BeKillBy(CActor* pAttacker)
{
    __ENTER_FUNCTION
    SC_DEAD msg;
    msg.set_actor_id(GetID());
    msg.set_attacker_id(pAttacker ? pAttacker->GetID() : 0);
    SendRoomMessage(msg);

    LOGACTORDEBUG(GetID(), "BeKillBy:{} Attacker:{}", GetID(), pAttacker ? pAttacker->GetID() : 0);
    if(m_pScene && m_pScene->GetScriptID())
    {
        ScriptManager()->TryExecScript<void>(SCRIPT_MAP, m_pScene->GetScriptID(), "OnActorBeKill", this, pAttacker);
    }

    GetStatus()->OnDead(pAttacker);
    GetStatus()->AttachStatus({.id_status_type = STATUSTYPE_DEAD, .lev = 1, .id_caster = GetID(), .expire_type = STATUSEXPIRETYPE_NEVER});

    __LEAVE_FUNCTION
}

void CActor::BroadcastShowTo(const VirtualSocketMap_t& VSMap)
{
    __ENTER_FUNCTION
    SC_AOI_NEW msg;
    MakeShowData(msg);
    SceneService()->SendProtoMsgTo(VSMap, msg);
    if(m_pStatusSet->size() > 0)
    {
        SC_STATUS_LIST status_msg;
        m_pStatusSet->FillStatusMsg(status_msg);
        SceneService()->SendProtoMsgTo(VSMap, msg);
    }

    __LEAVE_FUNCTION
}

void CActor::BroadcastMessageTo(const proto_msg_t& msg, const VirtualSocketMap_t& setSocketMap)
{
    __ENTER_FUNCTION
    //如果有需要发送new数据的,这里要优先发送一次
    SendShowToDealyList();
    SceneService()->SendProtoMsgTo(setSocketMap, msg);
    __LEAVE_FUNCTION
}
