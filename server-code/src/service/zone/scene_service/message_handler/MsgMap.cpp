
#include "ActorManager.h"
#include "MsgSceneProcess.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneService.h"
#include "msg/zone_service.pb.h"

ON_PLAYERMSG(CS_LOADMAP_SUCC)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() != nullptr)
        return;

    pPlayer->OnLoadMapSucc();
    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_CHANGEMAP)
{
    __ENTER_FUNCTION

    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->GetCurrentScene()->GetSceneIdx() != msg.scene_idx())
        return;
    pPlayer->TryChangeMap(msg.leavepoint_idx());
    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_MOVE)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->GetCurrentScene()->GetSceneIdx() != msg.scene_idx())
        return;

    if(pPlayer->MoveTo(Vector2(msg.x(), msg.y())) == false)
    {
        // send kick back to client
        LOGACTORDEBUG(pPlayer->GetID(), "try CS_MOVE x:{}, y:{} fail, curx:{} cury:{}", msg.x(), msg.y(), pPlayer->GetPosX(), pPlayer->GetPosY());
        return;
    }
    pPlayer->SetFace(msg.face());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_REBORN)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->Reborn(msg.reborn_type());
    __LEAVE_FUNCTION
}

////////////////////////////