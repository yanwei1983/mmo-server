
#include "ActorAI.h"

#include "AIActor.h"
#include "AIActorManager.h"
#include "AIGroup.h"
#include "AIPathFinder.h"
#include "AIPhase.h"
#include "AIService.h"
#include "AISkill.h"
#include "AIType.h"
#include "GameEventDef.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "HateList.h"
#include "MapManager.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "SkillType.h"
#include "config/Cfg_Scene_Patrol.pb.h"

#include "BehaviorTree.h"


constexpr int32_t MOVE_PER_WAIT_MS = 500; //每500ms向zone发送一次移动消息
constexpr float   DIS_VERY_CLOSE   = 0.01f;
constexpr uint32_t g_ai_tick_lev[]=
{
    1000,
    750,
    500,
    250,
    100,
};

uint32_t get_ai_tick_ms(uint32_t tick_lev)
{
    if(tick_lev >= sizeOfArray(g_ai_tick_lev))
        return tick_lev;
    return g_ai_tick_lev[tick_lev];
}

CActorAI::CActorAI() {}

CActorAI::~CActorAI()
{
    if(GetActor() && GetAIType())
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Finish Succ");
    }
}

bool CActorAI::Init(CAIActor* pActor, const CAIType* pAIType)
{
    __ENTER_FUNCTION
    m_pActor  = pActor;
    m_pAIType = pAIType;
    CHECKF(m_pActor);
    CHECKF(m_pAIType);
    m_pAIPathFinder = std::make_unique<CAIPathFinder_Normal>(pActor);
    m_HateList      = std::make_unique<CHateList>();
    if(GetAIData().follow_path() != 0)
    {
        m_pPathData = GetActor()->GetCurrentScene()->GetMap()->GetPatrolDataByIdx(GetAIData().follow_path());
    }

    if(GetAIData().bt_file().empty() == false)
    {
        m_pBTTree = BTManager()->clone(GetAIData().bt_file());
        CHECKF(m_pBTTree.get());
        m_pContext = std::make_unique<BT::BTContext>(m_pBTTree.get(), ScriptManager()->GetRawPtr(), GetActor(), this, GetActor()->GetID(), GetAIData().ai_debug());
        CHECKF(m_pContext.get());
        AddNextTick(get_ai_tick_ms(GetAIData().tick_lev()), false);
    }
    
    

    

    SetMainTargetID(0);
    m_posBorn = GetActor()->GetPos();

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Init Succ");

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CActorAI::AddNextTick(uint32_t ms, bool bforce)
{
    if(m_Event.IsWaitTrigger() == true && bforce == false)
        return;
    m_Event.Cancel();
    
    CEventEntryCreateParam param;
    param.evType    = EVENTID_MONSTER_AI;
    param.cb        = std::bind(&CActorAI::OnTick, this);
    param.tWaitTime = ms;
    EventManager()->ScheduleEvent(param, m_Event);
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: AddNextTick :{}", ms);
}

void CActorAI::OnUnderAttack(OBJID idTarget, int32_t nDamage)
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: OnUnderAttack target:{} damage:{}", idTarget, nDamage);

    bool find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, GetAIData().script_id(), "OnUnderAttack");
    if(find)
    {
        ScriptManager()->ExecStackScriptFunc<void>(this, idTarget, nDamage);
        return;
    }

    AddHate(idTarget, nDamage);

    __LEAVE_FUNCTION
}

void CActorAI::OnDead() {}

void CActorAI::OnTick()
{
    if(m_bSleep)
        return;
    
    if(m_pBTTree)
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: BTtree Tick");
        m_pBTTree->tick(*m_pContext.get());
        AddNextTick(get_ai_tick_ms(GetAIData().tick_lev()), false);
    }
}

void CActorAI::SetAISleep(bool v)
{
    __ENTER_FUNCTION
    if(m_bSleep == v)
        return;
    if(GetAIData().follow_path() != 0)
        return;
    m_bSleep = v;

    if(m_bSleep == false)
    {
        AddNextTick(get_ai_tick_ms(GetAIData().tick_lev()), false);
    }
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: SetAISleep: {}", v ? "true" : "false");

    __LEAVE_FUNCTION
}

uint32_t CActorAI::GetRandomWalkIdleWaitTime() const
{
    return random_uint32_range(GetAIData().idle_randomwalk_ms_min(), GetAIData().idle_randomwalk_ms_max());
}

uint32_t CActorAI::GetAttackWaitTime() const
{
    return GetAIData().attack_wait_ms();
}

uint32_t CActorAI::GetSearchEnemyTime() const
{
    return random_uint32_range(GetAIData().search_enemy_ms_min(), GetAIData().search_enemy_ms_max());
}

uint32_t CActorAI::GetPatrolSize() const
{
    if(m_pPathData == nullptr)
        return 0;
    return m_pPathData->data_size();
}
Vector2 CActorAI::GetCurPatrolPos()const
{
    auto data = GetCurPatrolData();
    if(data == nullptr)
        return GetBornPos();
    return {data->x(), data->y()};
}

uint32_t CActorAI::GetCurPatrolWaitTime() const
{
    return random_uint32_range(GetCurPatrolWaitMinMs(), GetCurPatrolWaitMaxMs());
}

uint32_t CActorAI::GetCurPatrolWaitMinMs()const
{
    auto data = GetCurPatrolData();
    if(data == nullptr)
        return 0;
    return data->wait_ms_min();
}

uint32_t CActorAI::GetCurPatrolWaitMaxMs()const
{
    auto data = GetCurPatrolData();
    if(data == nullptr)
        return 0;
    return data->wait_ms_max();
}

void CActorAI::NextPatrol()
{
    m_nCurPathNode++;
}

const ::Cfg_Scene_Patrol_patrol_data* CActorAI::GetCurPatrolData() const
{
    CHECKF(m_pPathData && m_pPathData->data_size() > 0);
    uint32_t nIdxPath = m_nCurPathNode;

    switch(m_pPathData->patrol_type())
    {
        case PARTOL_ONCE:
        {
            if(m_nCurPathNode >= m_pPathData->data_size() - 1)
                nIdxPath = m_pPathData->data_size() - 1;
        }
        break;
        case PARTOL_RING:
        {
            nIdxPath = m_nCurPathNode % m_pPathData->data_size();
        }
        break;
        case PARTOL_BACK:
        {
            uint32_t nRound = nIdxPath / m_pPathData->data_size();
            if((nRound & 1) == 0)
            {
                //正向
                nIdxPath = m_nCurPathNode % m_pPathData->data_size();
            }
            else
            {
                //反向
                nIdxPath = m_pPathData->data_size() - (m_nCurPathNode % m_pPathData->data_size());
            }
        }
        break;
    }
    CHECKF(nIdxPath < m_pPathData->data_size());
    return &m_pPathData->data(nIdxPath);
}

void CActorAI::ClearHateList()
{
    __ENTER_FUNCTION
    m_HateList->ClearHateList();
    __LEAVE_FUNCTION
}

void CActorAI::AddHate(OBJID idTarget, int32_t nHate)
{
    __ENTER_FUNCTION
    m_HateList->AddHate(idTarget, nHate);
    __LEAVE_FUNCTION
}


uint64_t CActorAI::GetTopHateID() const
{
    uint64_t result = 0;
    m_HateList->FindIF([this, &result](ST_HATE_DATA* pHateData) 
    {
        if(IsTargetLost(pHateData->idTarget) == true)
            return false;
        result = pHateData->idTarget;
        return true;
    });
    return result;
}

int32_t CActorAI::GetHate(uint64_t idTarget)const
{
    auto pHate = m_HateList->GetHate(idTarget);
    if(pHate)
        return pHate->nHate;
    return 0;
}
    
bool CActorAI::IsTargetLost(OBJID idTarget) const
{
    if(GetActor()->IsInViewActorByID(idTarget) == false)
        return true;
    CAIActor* pTarget = AIActorManager()->QueryActor(idTarget);
    if(pTarget == nullptr)
        return true;
    if(pTarget->IsDead())
        return true;
    if(GameMath::distance(pTarget->GetPos(), GetActor()->GetPos()) >= GetAIData().target_range())
        return true;
    return false;
}

bool CActorAI::FindEnemyInHateList()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindEnemyInHateList");

    uint64_t result = 0;
    int32_t nCount = 0;
    m_HateList->FindIF([this, &nCount, &result](ST_HATE_DATA* pHateData) 
    {
        nCount++;
        if(IsTargetLost(pHateData->idTarget) == true)
            return false;

        result = pHateData->idTarget;
        return true;
    });

    if(result != 0)
    {
        SetMainTargetID(result);
    }

    return GetMainTargetID() != 0;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ForEachInHateList(const std::function<bool(ST_HATE_DATA*)>& func)
{
    __ENTER_FUNCTION
    m_HateList->FindIF([this,func](ST_HATE_DATA* pHateData)
    {
        if(GetActor()->IsInViewActorByID(pHateData->idTarget) == false)
            return false;
        CAIActor* pTarget = AIActorManager()->QueryActor(pHateData->idTarget);
        if(pTarget == nullptr)
            return false;
        if(pTarget->IsDead())
            return false;
        auto dis = GameMath::distance(pTarget->GetPos(), GetActor()->GetPos());
        if(dis >= GetAIData().target_range())
            return false;

        
        return func(pHateData);
    });

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CActorAI::ChooseSkill(CAIActor* pTarget)
{
    __ENTER_FUNCTION
    float dis = GameMath::distance(GetActor()->GetPos(), pTarget->GetPos());

    double self_hp    = double(GetActor()->GetHP()) / double(GetActor()->GetHPMax());
    double self_mp    = double(GetActor()->GetMP()) / double(GetActor()->GetMPMax());
    double target_hp  = double(pTarget->GetHP()) / double(pTarget->GetHPMax());
    auto   pSkillData = GetActor()->GetSkillSet().ChooseSkill(m_pAIType->GetSkillFAM(), dis, self_hp, self_mp, target_hp);
    if(pSkillData == nullptr)
    {
        return;
    }

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: SelectSkill: {}",pSkillData->GetSkillTypeID());
    SetCurSkillID(pSkillData->GetSkillTypeID());
    __LEAVE_FUNCTION
}

float CActorAI::GetRandomWalkRange() const
{
    return GetAIData().idle_randomwalk_range();
}

float CActorAI::GetEscapeRange() const
{
    return random_float(GetAIData().escape_range_min(), GetAIData().escape_range_max());
}

float CActorAI::GetBattleRange() const
{
    return GetAIData().battle_range();
}

bool CActorAI::IsInSkillCastRange(float dis)const
{
    auto skill_id = GetCurSkillID();
    if(skill_id == 0)
        return false;

    auto pSkillType = SkillTypeSet()->QueryObj(skill_id);
    return dis >= pSkillType->GetDistance() && dis <= pSkillType->GetMaxDistance();
}

float CActorAI::GetSkillCastRange() const
{
    auto skill_id = GetCurSkillID();
    if(skill_id == 0)
        return 0.0f;

    auto pSkillType = SkillTypeSet()->QueryObj(skill_id);
    return random_float(pSkillType->GetDistance(), pSkillType->GetMaxDistance());
}


bool CActorAI::RandomWalk()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: RandomWalk");

    float dis = GameMath::distance(m_posBorn, GetActor()->GetPos());
    if(dis < GetRandomWalkRange())
    {
        auto target_pos = GetActor()->GetPos() + GameMath::random_vector2(GetAIData().idle_randomwalk_step_min(), GetAIData().idle_randomwalk_step_max());
        MoveTo(target_pos);
    }
    else
    {
        auto target_pos = m_posBorn + GameMath::random_vector2(GetAIData().idle_randomwalk_step_min(), GetAIData().idle_randomwalk_step_max());
        MoveTo(target_pos);
    }

    
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CActorAI::Wait(uint32_t wait_ms)
{
    m_WaitFinishTime = TimeGetMillisecond() + wait_ms;
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Wait: {}", wait_ms);
}

bool CActorAI::IsWaitFinish()
{
    return TimeGetMillisecond() > m_WaitFinishTime;
}

void CActorAI::MoveTo(const Vector2& target_pos)
{
    m_posTarget = target_pos;
    MoveToTarget();
}

void CActorAI::MoveToTarget()
{
    if(m_posTarget == Vector2::ZERO())
        return;
    GetActor()->MoveToTarget(m_posTarget);
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: MoveTo{}", m_posTarget);
    constexpr uint32_t MOVE_PER_WAIT_MS = 500; //每500ms向zone发送一次移动消息
    AddNextTick(MOVE_PER_WAIT_MS, false);
}

bool CActorAI::IsSkillFinish()
{   
    return TimeGetMillisecond() > m_SkillFinishTime;
}
void CActorAI::CastSkill()
{
    __ENTER_FUNCTION
    if(IsSkillFinish() == false)
        return;
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: CastSkill {} target:{}", GetCurSkillID(), GetMainTargetID());
    GetActor()->CastSkill(GetCurSkillID(), GetMainTargetID());
    m_SkillFinishTime = 0x7FFFFFFF;
    __LEAVE_FUNCTION
}
void CActorAI::OnCastSkillFinish(uint32_t skill_id, uint32_t stun_ms)
{   
    __ENTER_FUNCTION
    if(skill_id == GetCurSkillID())
    {
        m_SkillFinishTime = TimeGetMillisecond() + stun_ms;
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: CastSkillSucc {} finish_time:{}", GetCurSkillID(), m_SkillFinishTime);
    }
    __LEAVE_FUNCTION
}

bool CActorAI::IsNearTargetPos() const
{
    __ENTER_FUNCTION
    if(m_posTarget == Vector2::ZERO())
        return true;
    auto dis = GameMath::distance(m_posTarget, GetActor()->GetPos());
    if(dis < 0.01f)
    {
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

OBJID CActorAI::SearchEnemy()
{
    __ENTER_FUNCTION
    for(OBJID idActor: m_pActor->_GetViewList())
    {
        //找到第一个在范围内的敌人
        CAIActor* pActor = AIActorManager()->QueryActor(idActor);
        if(pActor == nullptr)
            continue;
        if(pActor->IsDead())
            continue;
        if(GetActor()->IsEnemy(pActor) == false)
            continue;
        auto dis = GameMath::distance(pActor->GetPos(), GetActor()->GetPos());
        if(dis >= m_pAIType->GetDataRef().search_enemy_range())
            continue;
        if(dis >= m_pAIType->GetDataRef().target_range())
            continue;

        //设置目标

        return idActor;
    }
    __LEAVE_FUNCTION
    return ID_NONE;
}


bool CActorAI::FindNextEnemy()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindNextEnemy Start");
    SetMainTargetID(0);
    if(FindEnemyInHateList() == true)
    {
        return true;
    }

    //遍历周围的敌人
    if(m_pAIType->GetDataRef().ai_type() != AITYPE_ACTIVE)
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ai_type() != AITYPE_ACTIVE stop FindNextEnemy");
        return false;
    }

    if(m_pAIType->GetTargetFAM() == nullptr)
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: UseNormal FindNextEnemy");
        OBJID idTarget = SearchEnemy();
        if(idTarget != ID_NONE)
        {
            SetMainTargetID(idTarget);
            return idTarget != ID_NONE;
        }
    }
    else
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: UseTargetTAM FindNextEnemy");

        double self_hp     = (double)m_pActor->GetHP() / (double)m_pActor->GetHPMax();
        double max_fDom    = -1.0f;
        OBJID  idNewTarget = ID_NONE;
        for(OBJID idActor: m_pActor->_GetViewList())
        {
            //找到第一个在范围内的敌人
            CAIActor* pTarget = AIActorManager()->QueryActor(idActor);
            if(pTarget == nullptr)
                continue;
            if(pTarget->IsDead())
                continue;
            if(GetActor()->IsEnemy(pTarget) == false)
                continue;

            float dis = GameMath::distance(pTarget->GetPos(), GetActor()->GetPos());
            if(dis >= m_pAIType->GetDataRef().search_enemy_range())
                continue;
            double target_hp = (double)pTarget->GetHP() / (double)pTarget->GetHPMax();

            double fDom = m_pAIType->GetTargetFAM()->calculate(dis, self_hp, target_hp);
            if(fDom > max_fDom)
            {
                max_fDom    = fDom;
                idNewTarget = idActor;
            }
        }

        if(idNewTarget != ID_NONE)
        {
            SetMainTargetID(idNewTarget);
            return idNewTarget != ID_NONE;
        }
    }
    __LEAVE_FUNCTION
    return false;
}

CAIActor* CActorAI::GetMainTarget() const
{
    if(GetMainTargetID() == 0)
        return nullptr;
    
    if(GetActor()->IsInViewActorByID(GetMainTargetID()) == false)
        return nullptr;

    CAIActor* pTarget = AIActorManager()->QueryActor(GetMainTargetID());
    if(pTarget == nullptr)
        return nullptr;

    return pTarget;
}

OBJID CActorAI::GetMainTargetID() const
{
    return m_idTarget;
}

void CActorAI::SetMainTargetID(OBJID val)
{
    __ENTER_FUNCTION
    if(m_idTarget == val)
        return;

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "SetMainTargetID: {} To {}", m_idTarget, val);
    m_idTarget = val;
    m_nCurSkillTypeID = 0;
    __LEAVE_FUNCTION
}

CAIActor* CActorAI::GetActor() const
{
    return m_pActor;
}

const CAIType* CActorAI::GetAIType() const
{
    return m_pAIType;
}

const Cfg_AIType& CActorAI::GetAIData() const
{
    return GetAIType()->GetDataRef();
}

