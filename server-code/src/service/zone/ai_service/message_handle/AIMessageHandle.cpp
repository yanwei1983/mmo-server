#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMessageHandler.h"
#include "AIMonster.h"
#include "AIPhase.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AISceneManagr.h"
#include "AIService.h"
#include "MonsterGenerator.h"
#include "NetMSGProcess.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_MSG(CAIService, SC_POS_CHANGE)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
    {
        LOGDEBUG("actorid:{} not find", msg.actor_id());
        return;
    }
    CHECK(pActor->GetCurrentScene());
    CHECK(msg.scene_idx() == pActor->GetCurrentScene()->GetSceneIdx());

    pActor->_SetPos(Vector2(msg.posx(), msg.posy()));
    LOGACTORDEBUG(pActor->GetID(), "MoveTo {} {:.2f}, {:.2f}", pActor->GetCurrentScene()->GetMapID(), pActor->GetPosX(), pActor->GetPosY());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, AOIChange)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
    {
        LOGDEBUG("actorid:{} not find", msg.actor_id());
        return;
    }
    CHECK(pActor->GetCurrentScene());
    CHECK(msg.scene_idx() == pActor->GetCurrentScene()->GetSceneIdx());

    for(const auto& id: msg.actor_del())
    {
        CAIActor* pTarget = AIActorManager()->QueryActor(id);
        pActor->RemoveFromViewList(pTarget, id, true);
        if(pTarget)
        {
            pTarget->RemoveFromViewList(pActor, pActor->GetID(), true);
        }
    }
    for(const auto& id: msg.actor_add())
    {
        CAIActor* pTarget = AIActorManager()->QueryActor(id);
        if(pTarget)
        {
            pActor->AddToViewList(pTarget);
            pTarget->AddToViewList(pActor);
        }
    }
    LOGACTORDEBUG(pActor->GetID(),
                  "ViewListChange cur:{} add:{} del:{}",
                  pActor->GetCurrentViewActorCount(),
                  msg.actor_add_size(),
                  msg.actor_del_size());
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_PROPERTY_CHANGE)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    for(int32_t i = 0; i < msg.datalist_size(); i++)
    {
        const auto& data = msg.datalist(i);
        pActor->SetProperty(data.actype(), data.val());
    }
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_CASTSKILL)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkill(msg.skill_id());
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_SKILL_STUN)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkillFinish(msg.skill_id(), msg.stun_ms());
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_SKILL_BREAK)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkillFinish(msg.skill_id(), 0);
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_DAMAGE)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnUnderAttack(msg.attacker_id(), msg.damage());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, SceneCreate)
{
    __ENTER_FUNCTION
    AISceneManager()->CreateScene(msg.scene_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, PhaseCreate)
{
    __ENTER_FUNCTION
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->CreatePhase(msg.scene_id(), msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, PhaseDestory)
{
    __ENTER_FUNCTION
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->DestoryPhase(msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActiveGen)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().ActiveGen(msg.gen_id(), msg.active());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, MonsterGenOnce)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().GenOnce(msg.gen_id(), msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, KillGen)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().KillGen(msg.gen_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorCreate)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());

    CHECK(pActor == nullptr);
    switch(msg.actortype())
    {
        case ACT_MONSTER:
        {
            pActor = CreateNew<CAIMonster>(msg);
            LOGACTORDEBUG(pActor->GetID(), "Create AIMonster id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);
        }
        break;
        case ACT_PLAYER:
        {
            pActor = CreateNew<CAIPlayer>(msg);
            LOGACTORDEBUG(pActor->GetID(), "Create AIPlayer id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);
        }
        break;
        case ACT_PET:
        {
        }
        break;
        case ACT_NPC:
        {
        }
        break;
    }

    if(pActor)
    {
        pScene->EnterMap(pActor, msg.posx(), msg.posy(), msg.face());
        AIActorManager()->AddActor(pActor);
        pActor->OnBorn();
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorDestory)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    LOGACTORDEBUG(pActor->GetID(), "ActorDestory id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);

    if(msg.dead())
    {
        pActor->OnDead();
    }

    if(pActor->GetCurrentScene())
        pActor->GetCurrentScene()->LeaveMap(pActor);

    AIActorManager()->DelActor(pActor);
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorCastSkill_Fail)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);

    pActor->OnCastSkillFinish(msg.skill_id(), 0);
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ServiceReady)
{
    __ENTER_FUNCTION
    LOGDEBUG("AIServer Start Running");
    AIService()->GetEventManager()->Pause(false);
    __LEAVE_FUNCTION
}
