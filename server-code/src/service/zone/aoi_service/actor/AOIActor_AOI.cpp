#include "AOIActor.h"
#include "AOIScene.h"
#include "AOIPhase.h"
#include "SceneTree.h"
#include "AOIPlayer.h"
#include "AOIService.h"
#include "server_msg/server_side.pb.h"
bool CAOIActor::CanDamage(CAOIActor* pTarget) const
{
    auto pOwner = QueryOwner();
    if(pOwner)
        return pOwner->CanDamage(pTarget);
    

    return GetCampID() != pTarget->GetCampID();
}

bool CAOIActor::IsEnemy(CSceneObject* pActor) const
{
    __ENTER_FUNCTION
    CHECKF(pActor);
    if(this == pActor)
        return false;

    if(m_idOwner != 0)
    {
        CAOIActor* pOwner = QueryOwner();
        if(pOwner)
            return pOwner->CanDamage(static_cast<CAOIActor*>(pActor));
    }

    return GetCampID() != static_cast<CAOIActor*>(pActor)->GetCampID();
    __LEAVE_FUNCTION
    return false;
}

//////////////////////////////////////////////////////////////////////
bool CAOIActor::UpdateViewList(bool bForce)
{   
    CHECKF(GetCurrentScene());
    CHECKF(GetSceneTile());
    float view_change_min = GetCurrentScene()->GetSceneTree()->GetViewChangeMin();
    auto  use_manhattan   = GetCurrentScene()->GetSceneTree()->IsViewManhattanDistance();
    if(bForce || ((use_manhattan) ? GameMath::manhattanDistance(m_LastUpdateViewPos, m_Pos) >= view_change_min
                                  : GameMath::simpleDistance(m_LastUpdateViewPos, m_Pos) >= view_change_min))
    {
        m_LastUpdateViewPos = m_Pos;
        return _UpdateViewList();
    }
    return true;
}

bool CAOIActor::ViewTest(CSceneObject* pActor)const
{
    __ENTER_FUNCTION
    //所有actor 可以看到 位面id与自己一样的的对象
    if(GetPhaseID() == pActor->GetPhaseID())
    {
        //怪物之间，额外通过敌我判断
        if(IsMonster() && pActor->IsMonster())
            return IsEnemy(pActor);
        return true;
    } 

    //如果有Owner，看看Owner能不能看见对方，一般来说Owner能看见，自己就能看见
    CAOIActor* pThisOwner = QueryOwner();
    if(pThisOwner)
    {
        return ViewTest(pActor);
    }
    
    //玩家 可以看到 位面id=自己ID 的其他对象   (专属怪刷新在专属位面内)
    //玩家 可以看到 inTaskList(位面id)的NPC/monster   (任务位面用来展开不同的NPC/monster)
    auto fun_check_player = [](const CSceneObject* pSelf,const CSceneObject* pActor)->bool
    {
        if(pSelf->GetID() == pActor->GetPhaseID())
            return true;
        const CAOIPlayer* pPlayer = pSelf->CastTo<const CAOIPlayer>();
        CHECKF(pPlayer);
        if(pPlayer->CheckTaskPhase(pActor->GetPhaseID()) == true)
            return true;
        return false;
    };
    if(IsPlayer())
    {
        return fun_check_player(this, pActor);
    }
    if(pActor->IsPlayer())
    {
        return fun_check_player(pActor, this);
    }
    __LEAVE_FUNCTION
    return false;
}



bool CAOIActor::IsMustAddToViewList(CSceneObject* pSceneObj) const
{
    __ENTER_FUNCTION
    CHECKF(pSceneObj);
    CAOIActor* pActor = static_cast<CAOIActor*>(pSceneObj);
    //自己召唤出来的,必然可见
    if(pActor->GetOwnerID() == GetID())
    {
        return true;
    }
    //私有位面的,必定可见
    if(pActor->GetPhaseID() != 0 && pActor->GetPhaseID() == GetID())
    {
        return true;
    }

    //必须加入视野的, BOSS怪, 组队成员
    if(pActor->IsMonster())
    {
        if(pActor->m_bMustSee)
        {
            return true;
        }

        return false;
    }

    if(pActor->IsPlayer())
    {
        const CAOIPlayer* pPlayer = pActor->CastTo<CAOIPlayer>();
        CHECKF(pPlayer);

        //队友必然可见
        if(IsPlayer())
        {
            const CAOIPlayer* pThisPlayer = CastTo<CAOIPlayer>();
            CHECKF(pThisPlayer);
            return (pThisPlayer->GetTeamID() != 0 && pPlayer->GetTeamID() == pThisPlayer->GetTeamID());
        }
    }
    __LEAVE_FUNCTION
    return false;
}


void CAOIActor::OnAOIProcess(const BROADCAST_SET& setBCAOIActorDel, const BROADCAST_SET& setBCAOIActor, const BROADCAST_SET& setBCAOIActorAdd)
{
    __ENTER_FUNCTION
    // step4: 需要离开视野的角色Remove
    OnAOIProcess_ActorRemoveFromAOI(setBCAOIActorDel);

    // 设置角色广播集=当前广播集-离开视野的差集
    m_ViewActors = setBCAOIActor;
    //////////////////////////////////////////////////////////////////////////
    // step5: 新进入视野的角色和地图物品Add
    OnAOIProcess_ActorAddToAOI(setBCAOIActorAdd);

    SendAOIChangeToOther(setBCAOIActorDel, setBCAOIActorAdd);
    __LEAVE_FUNCTION
}

void CAOIActor::OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCAOIActorDel)
{
    __ENTER_FUNCTION
    //////////////////////////////////////////////////////////////////////////
    // step4: 需要离开视野的角色Remove
    {
        for(const auto& id: setBCAOIActorDel)
        {
            CAOIActor* pActor = static_cast<CAOIActor*>(GetCurrentScene()->QuerySceneObj(id));
            // 通知自己对方消失
            //不需要从自己的m_ViewActors移除,因为等下会一次性移除,
            RemoveFromViewList(pActor, id, false);
            if(pActor)
            {
                // 通知对方自己消失,这里不用处理对方的sync消息，因为scene/ai收到变更消息,也会再执行1次Remove
                pActor->RemoveFromViewList(this, this->GetID(), true);
            }
            
          
        }
    }
    __LEAVE_FUNCTION
}


void CAOIActor::OnAOIProcess_ActorAddToAOI(const BROADCAST_SET& setBCAOIActorAdd)
{
    __ENTER_FUNCTION
    for(const auto& id: setBCAOIActorAdd)
    {
        CAOIActor* pActor = static_cast<CAOIActor*>(GetCurrentScene()->QuerySceneObj(id));
        if(pActor == nullptr)
        {
            continue;
        }

        AddToViewList(pActor);
        pActor->AddToViewList(this);

    }
    __LEAVE_FUNCTION
}

void CAOIActor::SendAOIChangeToOther(const BROADCAST_SET& setBCAOIActorDel, const BROADCAST_SET& setBCAOIActorAdd)
{
    __ENTER_FUNCTION
    if(setBCAOIActorDel.empty() && setBCAOIActorAdd.empty())
        return;
    ServerMSG::AOIChange msg;
    msg.set_scene_idx(GetCurrentScene()->GetSceneIdx());
    msg.set_actor_id(GetID());
    msg.set_posx(GetPosX());
    msg.set_posy(GetPosY());
    msg.mutable_actor_del()->Add(setBCAOIActorDel.begin(), setBCAOIActorDel.end());
    msg.mutable_actor_add()->Add(setBCAOIActorAdd.begin(), setBCAOIActorAdd.end());
    AOIService()->SendProtoMsgToAIService(msg);
    AOIService()->SendProtoMsgToSceneService(msg);
    __LEAVE_FUNCTION
}