#ifndef ACTORAI_H
#define ACTORAI_H

#include <functional>
#include <string_view>

#include "BehaviorTree.h"
#include "EventManager.h"
#include "HateList.h"
#include "export_lua.h"

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

    void OnTick();
    
public:
    export_lua void AddNextTick(uint32_t ms, bool bforce); 
    export_lua OBJID SearchEnemy();
    export_lua bool  FindNextEnemy();

    export_lua void ClearHateList();
    export_lua void AddHate(OBJID idTarget, int32_t nHate);
    export_lua uint64_t GetTopHateID() const;
    export_lua int32_t GetHate(uint64_t idTarget)const;
    
    export_lua bool IsTargetLost(OBJID idTarget) const;

    export_lua bool FindEnemyInHateList();
    export_lua bool ForEachInHateList(const std::function<bool(ST_HATE_DATA*)>& func);

    export_lua bool     IsAISleep()const {return m_bSleep;}
    export_lua void     SetAISleep(bool v);
    export_lua uint32_t GetCurSkillID() const { return m_nCurSkillTypeID; }
    export_lua void     SetCurSkillID(uint32_t val) { m_nCurSkillTypeID = val; }

public:
    export_lua uint32_t GetRandomWalkIdleWaitTime() const;
    export_lua uint32_t GetSearchEnemyTime() const;
    export_lua uint32_t GetAttackWaitTime() const;
    
    export_lua float GetRandomWalkRange() const;
    export_lua float GetEscapeRange() const;
    export_lua float GetBattleRange() const;
    export_lua float GetSkillCastRange() const;
    export_lua bool IsInSkillCastRange(float dis)const;


    export_lua void ChooseSkill(CAIActor* pTarget);
    export_lua bool RandomWalk();
    export_lua void Wait(uint32_t wait_ms);
    export_lua void CastSkill();

    export_lua void MoveTo(const Vector2& target_pos);
    export_lua void MoveToTarget();
    export_lua bool IsNearTargetPos() const;
    export_lua bool IsWaitFinish();
    export_lua bool IsSkillFinish();
    export_lua void OnCastSkillFinish(uint32_t skill_id, uint32_t stun_ms);
    
public:
    export_lua OBJID GetMainTargetID() const;
    export_lua CAIActor* GetMainTarget() const;
    export_lua void      SetMainTargetID(OBJID val);
    export_lua CAIActor* GetActor() const;
    export_lua const CAIType* GetAIType() const;
    export_lua CAIPathFinder* PathFind() const { return m_pAIPathFinder.get(); }
    export_lua CAIGroup* GetAIGroup() const { return m_pAIGroup; }

    const Cfg_AIType& GetAIData() const;

    export_lua const Vector2& GetTargetPos() const { return m_posTarget; }
    export_lua const Vector2& GetBornPos() const { return m_posBorn; }
    

public:
    export_lua uint32_t GetPatrolSize() const;
    export_lua Vector2  GetCurPatrolPos() const;
    export_lua uint32_t GetCurPatrolWaitTime() const;
    export_lua uint32_t GetCurPatrolWaitMinMs() const;
    export_lua uint32_t GetCurPatrolWaitMaxMs() const;
    export_lua void     NextPatrol();

private:
    const Cfg_Scene_Patrol_patrol_data* GetCurPatrolData() const;

private:
    CAIActor*      m_pActor  = nullptr;
    const CAIType* m_pAIType = nullptr;

    BT::BTTreePtr    m_pBTTree;
    BT::BTContextPtr m_pContext;

    std::unique_ptr<CAIPathFinder> m_pAIPathFinder;
    const Cfg_Scene_Patrol*        m_pPathData = nullptr;

    CEventEntryPtr m_Event;

    OBJID m_idTarget = 0;
    

    Vector2 m_posTarget;
    Vector2 m_posBorn;

    float m_fTargetDis = 0.0f;

    int32_t  m_nCurPathNode    = 0;
    uint32_t m_nCurSkillTypeID = 0;

    std::unique_ptr<CHateList> m_HateList;

    bool      m_bSleep          = false;
    uint32_t  m_WaitFinishTime  = 0;
    uint32_t  m_SkillFinishTime = 0;
    CAIGroup* m_pAIGroup        = nullptr;
};
#endif /* ACTORAI_H */
