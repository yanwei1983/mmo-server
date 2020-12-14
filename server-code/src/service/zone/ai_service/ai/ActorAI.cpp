
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

constexpr int32_t MOVE_PER_WAIT_MS = 500; //每500ms向zone发送一次移动消息
constexpr float   DIS_VERY_CLOSE   = 0.01f;

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

    SetMainTarget(0);
    m_posBorn = GetActor()->GetPos();

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Init Succ");

    ToIdle();
    return true;
    __LEAVE_FUNCTION
    return false;
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

const STATE_DATA& CActorAI::GetStateData(uint32_t nState)
{
    CHECK_RETTYPE_FMT(nState < ATT_MAX, STATE_DATA, "nState={}", nState);
    static const STATE_DATA STATE_DATA_ARRAY[] = {
        {"ATT_IDLE", &CActorAI::ProcessIdle},
        {"ATT_ATTACK", &CActorAI::ProcessAttack},
        {"ATT_APPROACH", &CActorAI::ProcessApproach},
        {"ATT_SKILL", &CActorAI::ProcessSkill},
        {"ATT_SKILLWAIT", &CActorAI::ProcessSkillWait},
        {"ATT_ESCAPE", &CActorAI::ProcessEscape},
        {"ATT_GOBACK", &CActorAI::ProcessGoback},
        {"ATT_PRATOL", &CActorAI::ProcessPatrol},
        {"ATT_PRATOLWAIT", &CActorAI::ProcessPatrolWait},
        {"ATT_RANDMOVE", &CActorAI::ProcessRandMove},
    };
    return STATE_DATA_ARRAY[nState];
}

void CActorAI::Process()
{
    __ENTER_FUNCTION
    const auto& state_data = GetStateData(m_nState);
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Process:{}", state_data.name);
    std::invoke(state_data.func, this);
    __LEAVE_FUNCTION
}

uint32_t CActorAI::GetState() const
{
    return m_nState;
}

void CActorAI::ChangeState(uint32_t val)
{
    m_nState = val;
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ChangeState: {}", GetStateData(val).name);
}

void CActorAI::SetAISleep(bool v)
{
    __ENTER_FUNCTION
    if(m_bSleep == v)
        return;

    if(m_nState == ATT_PRATOL || m_nState == ATT_PRATOLWAIT)
        return;

    m_bSleep = v;
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: SetAISleep: {}", v ? "true" : "false");

    if(m_bSleep == true)
    {
        // to sleeep
        m_SearchEnemyEvent.Cancel();
        m_Event.Cancel();
    }
    else
    {
        // unsleep
        ToIdle();
    }
    __LEAVE_FUNCTION
}

bool CActorAI::ToRandMove()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToRandMove");
    ChangeState(ATT_RANDMOVE);

    float dis = GameMath::distance(m_posBorn, GetActor()->GetPos());
    if(dis < GetAIData().idle_randomwalk_range())
    {
        m_posTarget = GetActor()->GetPos() + GameMath::random_vector2(GetAIData().idle_randomwalk_step_min(), GetAIData().idle_randomwalk_step_max());
    }
    else
    {
        m_posTarget = m_posBorn + GameMath::random_vector2(GetAIData().idle_randomwalk_step_min(), GetAIData().idle_randomwalk_step_max());
    }

    AddNextCall(MOVE_PER_WAIT_MS);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToPratol()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToPratol");
    if(m_pPathData == nullptr)
        return false;

    if(m_pPathData->patrol_type() == PARTOL_ONCE && m_nCurPathNode > m_pPathData->data_size())
    {
        return false;
    }
    else
    {
        auto pData = GetCurPratolData();
        if(pData == nullptr)
            return false;
        m_posTarget = Vector2(pData->x(), pData->y());
        AddNextCall(MOVE_PER_WAIT_MS);
        ChangeState(ATT_PRATOL);
        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::_ToIdle()
{
    __ENTER_FUNCTION
    if(GetAIData().follow_path() && m_pPathData)
    {
        // 如果是巡逻怪物，则开始巡逻
        if(ToPratol())
            return true;
    }

    ChangeState(ATT_IDLE);

    if(GetActor()->GetCurrentViewActorCount() == 0)
    {
        SetAISleep(true);
        return false;
    }

    SetAutoSearchEnemy();
    if(GetAIData().idle_randomwalk_ms_max() > 0 && GetActor()->GetMoveSpeed() != 0)
    {
        AddNextCall(random_uint32_range(GetAIData().idle_randomwalk_ms_min(), GetAIData().idle_randomwalk_ms_max()));
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToIdle()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToIdle");

    auto find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, GetAIData().script_id(), "ToIdle");
    if(find)
    {
        if(ScriptManager()->ExecStackScriptFunc<bool>(this) == true)
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: TryExecScript ToIdle Succ");
            return true;
        }
    }

    return _ToIdle();
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToAttack()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToAttack");
    ChangeState(ATT_ATTACK);
    AddNextCall(GetAIData().attack_wait_ms());
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToApproach()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToApproach");
    if(GetActor()->GetMoveSpeed() == 0 || GetAIData().can_approach() == false)
    {
        return ToAttack();
    }

    ChangeState(ATT_APPROACH); // 转换到approach状态
    AddNextCall(200);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToSkill()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToSkill");
    CAIActor* pTarget = AIActorManager()->QueryActor(GetMainTarget());
    if(pTarget == nullptr || pTarget->IsDead() == true)
    { // 目标不存在或已死亡
        SetMainTarget(0);
        return ToAttack();
    }
    const CSkillType* pCurSkillType = SkillTypeSet()->QueryObj(GetCurSkillTypeID());
    if(pCurSkillType == nullptr)
    {
        SetMainTarget(0);
        return ToAttack();
    }

    float dis = GameMath::distance(GetActor()->GetPos(), pTarget->GetPos());
    if(dis < pCurSkillType->GetDistance())
    {
        //距离不正确,找到正确的距离
        m_fTargetDis = random_float(pCurSkillType->GetDistance(), pCurSkillType->GetMaxDistance());
        ToApproach();
        return true;
    }
    else if(dis > pCurSkillType->GetMaxDistance())
    {
        m_fTargetDis = random_float(pCurSkillType->GetDistance(), pCurSkillType->GetMaxDistance());
        return ToApproach();
    }

    LOGAIDEBUG(GetAIData().ai_debug(),
               GetActor()->GetID(),
               "AI: {} castSkill: {} Target:{}",
               GetActor()->GetID(),
               pCurSkillType->GetSkillID(),
               m_idTarget);

    //可以释放
    GetActor()->CastSkill(pCurSkillType->GetSkillID(), m_idTarget);
    ChangeState(ATT_SKILL); // 转换到skill状态
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToEscape(OBJID idTarget)
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToEscape");
    CAIActor* pTarget = AIActorManager()->QueryActor(idTarget);
    if(pTarget == nullptr || pTarget->IsDead() == true)
    { // 目标不存在或已死亡
        return ToAttack();
    }

    auto vDir   = GetActor()->GetPos() - pTarget->GetPos();
    m_posTarget = vDir * random_float(GetAIData().escape_range_min(), GetAIData().escape_range_max());

    ChangeState(ATT_ESCAPE); // 转换到ESCAPE状态
    AddNextCall(MOVE_PER_WAIT_MS);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::_ToGoBack()
{
    __ENTER_FUNCTION
    m_posTarget = m_posRecord;
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToGoBack {}", m_posTarget);
    ChangeState(ATT_GOBACK); // 转换到ESCAPE状态
    AddNextCall(MOVE_PER_WAIT_MS);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToGoBack()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToGoBack");

    auto find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, GetAIData().script_id(), "ToGoBack");
    if(find)
    {
        if(ScriptManager()->ExecStackScriptFunc<bool>(this) == true)
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: TryExecScript ToGoBack Succ");
            return true;
        }
    }

    return _ToGoBack();
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToPatrolWait(uint32_t wait_min, uint32_t wait_max)
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToPatrolWait");
    ChangeState(ATT_PRATOLWAIT); // 转换到ESCAPE状态
    AddNextCall(random_uint32_range(wait_min, wait_max));
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ToSkillFinish(uint32_t stun_ms)
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ToSkillFinish");
    ChangeState(ATT_SKILLWAIT); // 转换到ESCAPE状态
    AddNextCall(stun_ms);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CActorAI::_ProcessAttack(CAIActor* pTarget)
{
    __ENTER_FUNCTION
    float dis = GameMath::distance(GetActor()->GetPos(), pTarget->GetPos());
    if(dis > GetAIData().attack_target_range())
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: ProessAttack target out of range");

        SetMainTarget(0);
        ToAttack();
        return;
    }

    double self_hp    = double(GetActor()->GetHP()) / double(GetActor()->GetHPMax());
    double self_mp    = double(GetActor()->GetMP()) / double(GetActor()->GetMPMax());
    double target_hp  = double(pTarget->GetHP()) / double(pTarget->GetHPMax());
    auto   pSkillData = GetActor()->GetSkillSet().ChooseSkill(m_pAIType->GetSkillFAM(), dis, self_hp, self_mp, target_hp);
    if(pSkillData == nullptr)
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Skill can't choose");

        SetMainTarget(0);
        ToAttack();
        return;
    }

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: {} SelectSkill: {}", GetActor()->GetID(), pSkillData->GetSkillType()->GetID());
    SetCurSkillTypeID(pSkillData->GetSkillType()->GetID());
    ToSkill();
    __LEAVE_FUNCTION
}
void CActorAI::ProcessAttack()
{
    __ENTER_FUNCTION
    if(GetMainTarget() == 0)
    {
        if(FindNextEnemy() == false)
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindNextEnemy fail");
            ToGoBack();
            return;
        }
        else
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindNextEnemy Succ");
        }
    }

    CAIActor* pTarget = AIActorManager()->QueryActor(m_idTarget);
    if(pTarget == nullptr || pTarget->IsDead())
    {
        if(FindNextEnemy() == false)
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindNextEnemy New fail");
            ToGoBack();
            return;
        }
        else
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindNextEnemy New Succ");
            ToAttack();
            return;
        }
    }

    auto find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, GetAIData().script_id(), "ProcessAttack");
    if(find)
    {
        if(ScriptManager()->ExecStackScriptFunc<bool>(this, pTarget) == true)
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: TryExecScript ProcessAttack Succ");
            return;
        }
    }

    _ProcessAttack(pTarget);
    __LEAVE_FUNCTION
}

void CActorAI::ProcessRandMove()
{
    __ENTER_FUNCTION
    if(GameMath::distance(GetActor()->GetPos(), m_posTarget) <= 0.01f)
    {
        ToIdle();
        return;
    }
    auto result = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
    if(!result)
    {
        //没有找到路径
        ToIdle();
        return;
    }
    GetActor()->MoveToTarget(result.value());
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "From {} MoveToTarget {}", GetActor()->GetPos(), result.value());
    AddNextCall(MOVE_PER_WAIT_MS);
    __LEAVE_FUNCTION
}

void CActorAI::ProcessIdle()
{
    __ENTER_FUNCTION
    ToRandMove();
    __LEAVE_FUNCTION
}

void CActorAI::ProcessSkillWait()
{
    __ENTER_FUNCTION
    ToAttack();
    __LEAVE_FUNCTION
}

void CActorAI::ProcessApproach()
{
    __ENTER_FUNCTION
    CAIActor* pTarget = AIActorManager()->QueryActor(GetMainTarget());
    if(pTarget == nullptr || pTarget->IsDead() == true)
    {
        ToAttack();
        return;
    }

    float dis_born = GameMath::distance(GetActor()->GetPos(), m_posRecord);
    if(dis_born > GetAIData().attack_pursue_selfrange())
    {
        ToGoBack();
        return;
    }

    //从对方的位置，计算反向的距离
    auto  vDir  = GetActor()->GetPos() - pTarget->GetPos();
    float fDis  = vDir.normalise();
    m_posTarget = pTarget->GetPos() + vDir * m_fTargetDis;

    if(GameMath::distance(GetActor()->GetPos(), m_posTarget) > DIS_VERY_CLOSE)
    {
        auto result = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(result)
        {
            GetActor()->MoveToTarget(result.value());
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "From {} MoveToTarget {}", GetActor()->GetPos(), result.value());
            AddNextCall(MOVE_PER_WAIT_MS);
        }
        else
        {
            //没有找到路径
            ToAttack();
        }
    }
    else
    {
        ToSkill();
    }

    __LEAVE_FUNCTION
}

void CActorAI::ProcessEscape()
{
    __ENTER_FUNCTION
    if(GameMath::distance(GetActor()->GetPos(), m_posTarget) > DIS_VERY_CLOSE)
    {
        auto result = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(result)
        {
            GetActor()->MoveToTarget(result.value());
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "From {} MoveToTarget {}", GetActor()->GetPos(), result.value());
            AddNextCall(MOVE_PER_WAIT_MS);
        }
        else
        {
            //没有找到路径
            ToAttack();
        }
    }
    else
    {
        ToAttack();
    }
    __LEAVE_FUNCTION
}

void CActorAI::ProcessGoback()
{
    __ENTER_FUNCTION
    if(GameMath::distance(GetActor()->GetPos(), m_posTarget) > DIS_VERY_CLOSE)
    {
        auto result = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(result)
        {
            GetActor()->MoveToTarget(result.value());
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "From {} MoveToTarget {}", GetActor()->GetPos(), result.value());
            AddNextCall(MOVE_PER_WAIT_MS);
        }
        else
        {
            //没有找到路径
            // flyto
            GetActor()->FlyTo(m_posTarget);
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "From {} FlyTo {}", GetActor()->GetPos(), m_posTarget);
            AddNextCall(MOVE_PER_WAIT_MS);
        }
    }
    else
    {
        ToIdle();
    }
    __LEAVE_FUNCTION
}

void CActorAI::ProcessSkill()
{
    //等待技能释放完成的消息收到
}

void CActorAI::ProcessPatrol()
{
    __ENTER_FUNCTION
    if(GameMath::distance(GetActor()->GetPos(), m_posTarget) > DIS_VERY_CLOSE)
    {
        auto result = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(!result)
        {
            //没有找到路径
            ToIdle();
            return;
        }
        GetActor()->MoveToTarget(result.value());
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "From {} MoveToTarget {}", GetActor()->GetPos(), result.value());
        AddNextCall(MOVE_PER_WAIT_MS);
    }
    else
    {
        auto pData = GetCurPratolData();
        if(pData == nullptr)
        {
            ToIdle();
            return;
        }
        if(pData->wait_ms_min() == 0 && pData->wait_ms_max() == 0)
            ToPatrolWait(GetAIData().patrol_wait_ms_min(), GetAIData().patrol_wait_ms_max());
        else
            ToPatrolWait(pData->wait_ms_min(), pData->wait_ms_max());
    }
    __LEAVE_FUNCTION
}

void CActorAI::ProcessPatrolWait()
{
    __ENTER_FUNCTION
    ProcessPatrol();
    __LEAVE_FUNCTION
}

const ::Cfg_Scene_Patrol_patrol_data* CActorAI::GetCurPratolData()
{
    CHECKF(m_pPathData && m_pPathData->data_size() > 0);
    uint32_t nIdxPath = m_nCurPathNode;

    switch(m_pPathData->patrol_type())
    {
        case PARTOL_ONCE:
        {
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

void CActorAI::OrderAttack(OBJID idTarget)
{
    __ENTER_FUNCTION
    //判断ai类型
    if(m_pAIType->GetDataRef().ai_type() == AITYPE_NONE)
        return;
    if(m_nState == ATT_GOBACK || m_nState == ATT_ESCAPE)
        return;
    if(m_nState == ATT_PRATOL || m_nState == ATT_PRATOLWAIT || m_nState == ATT_RANDMOVE || m_nState == ATT_IDLE)
    {
        m_posRecord = GetActor()->GetPos();
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: m_posRecord {}", m_posRecord);
    }

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: OrderAttack: {}", idTarget);

    SetMainTarget(idTarget);
    ToAttack();
    m_SearchEnemyEvent.Cancel();
    __LEAVE_FUNCTION
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
    float fHate = m_HateList->AddHate(idTarget, nHate);
    if(m_idTarget != 0)
    {
        auto pMainHateData = m_HateList->GetHate(m_idTarget);
        CHECK(pMainHateData);

        float fMainTargetHate = pMainHateData->fHate;
        if(fHate > fMainTargetHate * 1.5f)
        {
            // change enemy
            OrderAttack(idTarget);
        }
    }
    else
    {
        OrderAttack(idTarget);
    }
    __LEAVE_FUNCTION
}

bool CActorAI::FindEnemyInHateList()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindEnemyInHateList");

    int32_t nCount = 0;
    m_HateList->FindIF([pThis = this, &nCount](ST_HATE_DATA* pHateData) {
        nCount++;
        CAIActor* pActor = AIActorManager()->QueryActor(pHateData->idTarget);
        if(pActor == nullptr)
            return false;
        if(pActor->IsDead())
            return false;
        if(GameMath::distance(pActor->GetPos(), pThis->GetActor()->GetPos()) >= pThis->GetAIData().attack_target_range())
            return false;

        //设置目标
        pThis->SetMainTarget(pActor->GetID());
        return true;
    });

    if(nCount == 0)
    {
        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindEnemyInHateList hatelist empty");
        return false;
    }

    return GetMainTarget() != 0;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::ForEachInHateList(const std::function<bool(ST_HATE_DATA*)>& func)
{
    __ENTER_FUNCTION
    m_HateList->FindIF([pThis = this](ST_HATE_DATA* pHateData) {
        CAIActor* pActor = AIActorManager()->QueryActor(pHateData->idTarget);
        if(pActor == nullptr)
            return false;
        if(GameMath::distance(pActor->GetPos(), pThis->GetActor()->GetPos()) >= pThis->GetAIData().attack_target_range())
            return false;

        //设置目标
        pThis->SetMainTarget(pActor->GetID());
        return true;
    });

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::_FindNextEnemy()
{
    __ENTER_FUNCTION
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
            SetMainTarget(ID_NONE);
            return GetMainTarget() != ID_NONE;
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
            SetMainTarget(idNewTarget);
            return GetMainTarget() != ID_NONE;
        }
    }
    __LEAVE_FUNCTION
    return false;
}

bool CActorAI::FindNextEnemy()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: FindNextEnemy Start");
    SetMainTarget(0);

    auto find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, GetAIData().script_id(), "FindNextEnemy");
    if(find)
    {
        if(ScriptManager()->ExecStackScriptFunc<bool>(this) == true)
        {
            LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: TryExecScript FindNextEnemy Succ");
            return true;
        }
    }
    return _FindNextEnemy();
    __LEAVE_FUNCTION
    return false;
}

OBJID CActorAI::GetMainTarget() const
{
    return m_idTarget;
}

void CActorAI::SetMainTarget(OBJID val)
{
    __ENTER_FUNCTION
    if(m_idTarget == val)
        return;

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: {} SetMainTarget: {} To {}", GetActor()->GetID(), m_idTarget, val);
    m_idTarget = val;
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

void CActorAI::AddNextCall(uint32_t ms)
{
    __ENTER_FUNCTION
    m_Event.Cancel();
    CEventEntryCreateParam param;
    param.evType    = EVENTID_MONSTER_AI;
    param.cb        = std::bind(&CActorAI::Process, this);
    param.tWaitTime = ms;
    EventManager()->ScheduleEvent(param, m_Event);

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: AddNextCall: {}", ms);
    __LEAVE_FUNCTION
}

void CActorAI::SetAutoSearchEnemy()
{
    __ENTER_FUNCTION
    if(GetAIType()->GetDataRef().ai_type() != AITYPE_ACTIVE)
        return;

    m_SearchEnemyEvent.Cancel();
    uint32_t ms = random_uint32_range(GetAIType()->GetDataRef().search_enemy_ms_min(), GetAIType()->GetDataRef().search_enemy_ms_max());

    CEventEntryCreateParam param;
    param.evType    = EVENTID_MONSTER_AI_SEARCHENEMY;
    param.cb        = std::bind(&CActorAI::_SearchEnemy_CallBack, this);
    param.tWaitTime = ms;
    EventManager()->ScheduleEvent(param, m_SearchEnemyEvent);

    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: Next AutoSearchEnemy:{}", ms);
    __LEAVE_FUNCTION
}

void CActorAI::_SearchEnemy_CallBack()
{
    __ENTER_FUNCTION
    LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: callback AutoSearchEnemy");
    OBJID idActor = SearchEnemy();
    if(idActor == ID_NONE)
    {
        SetAutoSearchEnemy();
    }
    else
    {
        OrderAttack(idActor);
    }
    __LEAVE_FUNCTION
}

OBJID CActorAI::_SearchEnemy()
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
        if(GameMath::distance(pActor->GetPos(), GetActor()->GetPos()) >= m_pAIType->GetDataRef().search_enemy_range())
            continue;

        //设置目标

        return idActor;
    }
    __LEAVE_FUNCTION
    return ID_NONE;
}

OBJID CActorAI::SearchEnemy()
{
    __ENTER_FUNCTION

    auto find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, GetAIData().script_id(), "SearchEnemy");
    if(find)
    {
        OBJID idTarget = ScriptManager()->ExecStackScriptFunc<OBJID>(this);

        LOGAIDEBUG(GetAIData().ai_debug(), GetActor()->GetID(), "AI: TryExecScript SearchEnemy Succ");
        return idTarget;
    }

    return _SearchEnemy();

    __LEAVE_FUNCTION
    return ID_NONE;
}
