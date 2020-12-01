#ifndef ACTORAI_H
#define ACTORAI_H

#include <functional>
#include <string_view>

#include "EventManager.h"
#include "HateList.h"
#include "export_lua.h"
//最后还是考虑使用最简答的单层状态机来构建AI
//在攻击决策模块中,适量的使用模糊逻辑,来增加随机性

//状态转移图
// ATT_PRATOL <> ATT_PRATOLWAIT
// VV
// ATT_IDLE <> ATT_RANDMOVE
// VV
// ATT_GOBACK
// VV
// ATT_ATTACK <> ATT_SKILL <> ATT_APPROACH
// VV
// ATT_ESCAPE

export_lua enum AITaskType {
    ATT_IDLE = 0,   // idle
    ATT_ATTACK,     //攻击决策
    ATT_APPROACH,   //移动到距离目标N米处
    ATT_SKILL,      //释放某种技能
    ATT_SKILLWAIT,  // wait for skill finish
    ATT_ESCAPE,     //逃离
    ATT_GOBACK,     //快速移动/无敌移动到目标点
    ATT_PRATOL,     // prtrol from the path
    ATT_PRATOLWAIT, // prtrol from the path
    ATT_RANDMOVE,   // random move
    ATT_MAX,
};

class CActorAI;
struct STATE_DATA
{
    std::string_view name;
    typedef void (CActorAI::*ProcessFunc)();
    ProcessFunc func;
};

class CAIActor;
class CAIPathFinder;
class CAIGroup;
class CAIType;

struct ST_HATE_DATA;
class Cfg_AIType;
class Cfg_Scene_Patrol;
class Cfg_Scene_Patrol_patrol_data;

export_lua class CActorAI : public NoncopyableT<CActorAI>
{
    CActorAI();
    bool Init(CAIActor* pActor, const CAIType* pAIType);

public:
    CreateNewImpl(CActorAI);

public:
    ~CActorAI();

    void OnUnderAttack(OBJID idTarget, int32_t nDamage);
    void OnDead();

    export_lua void OrderAttack(OBJID idTarget);

    export_lua OBJID SearchEnemy();
    export_lua OBJID _SearchEnemy();
    export_lua bool  FindNextEnemy();
    export_lua bool  _FindNextEnemy();
    export_lua void  ClearHateList();
    export_lua void  AddHate(OBJID idTarget, int32_t nHate);

    export_lua bool FindEnemyInHateList();
    export_lua bool ForEachInHateList(const std::function<bool(ST_HATE_DATA*)>& func);
    void            Process();

    export_lua uint32_t GetState() const;
    export_lua void     ChangeState(uint32_t val);

    export_lua void     SetAISleep(bool v);
    export_lua uint32_t GetCurSkillTypeID() const { return m_nCurSkillTypeID; }
    export_lua void     SetCurSkillTypeID(uint32_t val) { m_nCurSkillTypeID = val; }

    static const STATE_DATA& GetStateData(uint32_t nState);

public:
    // 随机移动
    export_lua bool ToRandMove();
    // 巡逻路径
    export_lua bool ToPratol();
    // 转到idle状态
    export_lua bool ToIdle();
    export_lua bool _ToIdle();
    // 攻击决策
    export_lua bool ToAttack();
    // 转到前进状态
    export_lua bool ToApproach();
    // 转到攻击状态
    export_lua bool ToSkill();
    export_lua bool ToSkillFinish(uint32_t stun_ms);

    // 转到逃跑状态
    export_lua bool ToEscape(OBJID idTarget);

    // 回出生点
    export_lua bool ToGoBack();
    export_lua bool _ToGoBack();
    // 巡逻等待
    export_lua bool ToPatrolWait(uint32_t wait_min, uint32_t wait_max);

protected:
    void ProcessAttack();
    export_lua void _ProcessAttack(CAIActor* pTarget);
    void ProcessRandMove();
    //! 空闲状态处理
    void ProcessIdle();
    //! 前进状态处理
    void ProcessApproach();
    //! 逃跑状态处理
    void ProcessEscape();
    //! 返回出生点处理
    void ProcessGoback();
    //! 攻击状态处理
    void ProcessSkill();
    void ProcessSkillWait();
    //! 巡逻状态处理
    void ProcessPatrol();
    void ProcessPatrolWait();

public:
public:
    export_lua OBJID GetMainTarget() const;
    export_lua void  SetMainTarget(OBJID val);
    export_lua CAIActor* GetActor() const;
    export_lua const CAIType* GetAIType() const;
    export_lua CAIPathFinder* PathFind() const { return m_pAIPathFinder.get(); }
    export_lua CAIGroup* GetAIGroup() const { return m_pAIGroup; }

    const Cfg_AIType&                   GetAIData() const;
    const Cfg_Scene_Patrol_patrol_data* GetCurPratolData();

private:
    void AddNextCall(uint32_t ms);

    void SetAutoSearchEnemy();
    void _SearchEnemy_CallBack();

private:
    uint32_t                       m_nState  = 0;
    CAIActor*                      m_pActor  = nullptr;
    const CAIType*                 m_pAIType = nullptr;
    std::unique_ptr<CAIPathFinder> m_pAIPathFinder;
    OBJID                          m_idTarget = 0;
    Vector2                        m_posTarget;
    Vector2                        m_posRecord;
    Vector2                        m_posBorn;
    float                          m_fTargetDis = 0.0f;

    int32_t                 m_nCurPathNode    = 0;
    uint32_t                m_nCurSkillTypeID = 0;
    const Cfg_Scene_Patrol* m_pPathData       = nullptr;

    CEventEntryPtr m_Event;
    CEventEntryPtr m_SearchEnemyEvent;

    std::unique_ptr<CHateList> m_HateList;

    bool m_bSleep = false;

    CAIGroup* m_pAIGroup = nullptr;
};
#endif /* ACTORAI_H */
