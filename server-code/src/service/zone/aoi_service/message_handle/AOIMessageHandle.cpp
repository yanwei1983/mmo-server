#include "AOIActor.h"
#include "AOIActorManager.h"
#include "AOIMessageHandler.h"
#include "AOIPhase.h"
#include "AOIPlayer.h"
#include "AOIScene.h"
#include "AOISceneManagr.h"
#include "AOIService.h"
#include "NetMSGProcess.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_MSG(CAOIService, SC_PROPERTY_CHANGE)
{
    __ENTER_FUNCTION
    CAOIActor* pActor = AOIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    for(int32_t i = 0; i < msg.datalist_size(); i++)
    {
        const auto& data = msg.datalist(i);
        pActor->SetProperty(data.actype(), data.val());
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, AOIChange)
{
    __ENTER_FUNCTION
    CAOIActor* pActor = AOIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->_SetPos(Vector2(msg.posx(), msg.posy()));
    pActor->UpdateViewList(false);
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, SceneCreate)
{
    __ENTER_FUNCTION
    AOISceneManager()->CreateScene(msg.scene_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, PhaseCreate)
{
    __ENTER_FUNCTION
    CAOIScene* pScene = AOISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->CreatePhase(msg.scene_id(), msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, PhaseDestory)
{
    __ENTER_FUNCTION
    CAOIScene* pScene = AOISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->DestoryPhase(msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, ActorCreate)
{
    __ENTER_FUNCTION
    CAOIPhase* pScene = AOISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    CAOIActor* pActor = AOIActorManager()->QueryActor(msg.actor_id());

    CHECK(pActor == nullptr);
    switch(msg.actortype())
    {
        case ACT_PLAYER:
        {
            pActor = CAOIPlayer::CreateNew(msg);
            LOGACTORDEBUG(pActor->GetID(), "Create AOIPlayer id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);
        }
        break;
        default:
        {
            pActor = CAOIActor::CreateNew(msg);
            LOGACTORDEBUG(pActor->GetID(), "Create AOIActor id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);
        }
        break;
    }

    if(pActor)
    {
        pScene->EnterMap(pActor, msg.posx(), msg.posy(), msg.face());
        AOIActorManager()->AddActor(pActor);
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, ActorDestory)
{
    __ENTER_FUNCTION
    CAOIActor* pActor = AOIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    LOGACTORDEBUG(pActor->GetID(), "ActorDestory id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);

    if(pActor->GetCurrentScene())
        pActor->GetCurrentScene()->LeaveMap(pActor);

    AOIActorManager()->DelActor(pActor);
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, ActorSetHide)
{
    __ENTER_FUNCTION
    CAOIActor* pActor = AOIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->SetHideCoude(msg.hide_count());
    LOGACTORDEBUG(pActor->GetID(), "SetHideCoude id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, SyncTaskPhase)
{
    __ENTER_FUNCTION
    auto pActor = AOIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAOIPlayer* pPlayer = pActor->CastTo<CAOIPlayer>();
    CHECK(pPlayer);
    pPlayer->ClearTaskPhase();
    for(int32_t i = 0; i < msg.task_phase_id_size(); i++)
    {
        pPlayer->AddTaskPhase(msg.task_phase_id(i));
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, AddTaskPhase)
{
    __ENTER_FUNCTION
    auto pActor = AOIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAOIPlayer* pPlayer = pActor->CastTo<CAOIPlayer>();
    CHECK(pPlayer);
    pPlayer->AddTaskPhase(msg.task_phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAOIService, RemoveTaskPhase)
{
    __ENTER_FUNCTION
    auto pActor = AOIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAOIPlayer* pPlayer = pActor->CastTo<CAOIPlayer>();
    CHECK(pPlayer);
    pPlayer->RemoveTaskPhase(msg.task_phase_id());
    __LEAVE_FUNCTION
}
