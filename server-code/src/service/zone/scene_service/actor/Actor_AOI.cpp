#include "Actor.h"
#include "ActorManager.h"
#include "GameEventDef.h"
#include "Monster.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneService.h"
#include "SceneTree.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

bool CActor::NeedSyncAI() const
{
    return IsMonster() || IsPlayer();
}
//////////////////////////////////////////////////////////////////////
bool CActor::UpdateViewList(bool bForce)
{
    //通知AOIServer, 位置变更
    ServerMSG::AOIChange ntc;
    ntc.set_scene_idx(GetSceneIdx());
    ntc.set_actor_id(GetID());
    ntc.set_posx(GetPosX());
    ntc.set_posy(GetPosY());

    SceneService()->SendProtoMsgToAOIService(ntc);
    return true;
}

//////////////////////////////////////////////////////////////////////
void CActor::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
    __ENTER_FUNCTION
    // 通知自己对方消失
    CSceneObject::RemoveFromViewList(pActor, idActor, bErase);

    if(pActor)
    {
        if(pActor->GetActorType() == ACT_PLAYER)
        {
            RemoveDelaySendShowTo(pActor->GetID());
        }
    }
    else
    {
        //延迟发送队列里的也可以不要了
        RemoveDelaySendShowTo(idActor);
    }
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////
void CActor::OnBeforeClearViewList(bool bSendMsgToSelf)
{
    __ENTER_FUNCTION
    //发送删除包
    SC_AOI_REMOVE remove_msg_to_other;
    remove_msg_to_other.set_scene_idx(GetSceneIdx());
    remove_msg_to_other.add_idlist(GetID());
    SendRoomMessageExcludeSelf(remove_msg_to_other);

    if(bSendMsgToSelf)
    {
        SC_AOI_REMOVE remove_msg_to_self;
        remove_msg_to_self.set_scene_idx(GetSceneIdx());
        remove_msg_to_self.mutable_idlist()->Add(m_ViewActors.begin(), m_ViewActors.end());
        SendMsg(remove_msg_to_self);
    }

    if(m_setDealySendShow.empty() == false)
    {
        GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
    }
    m_setDealySendShow.clear();
    __LEAVE_FUNCTION
}

void CActor::AddDelaySendShowTo(OBJID id)
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty() == true)
    {
        auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
        if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsWaitTrigger() == false)
        {
            CEventEntryCreateParam param;
            param.evType    = EVENTID_BROCAST_SHOW;
            param.cb        = std::bind(&CActor::SendShowToDealyList, this);
            param.tWaitTime = 0;
            param.bPersist  = false;
            EventManager()->ScheduleEvent(param, GetEventMapRef());
        }
    }
    m_setDealySendShow.insert(id);
    __LEAVE_FUNCTION
}

void CActor::AddDelaySendShowToAllViewPlayer()
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty() == true)
    {
        auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
        if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsWaitTrigger() == false)
        {
            CEventEntryCreateParam param;
            param.evType    = EVENTID_BROCAST_SHOW;
            param.cb        = std::bind(&CActor::SendShowToDealyList, this);
            param.tWaitTime = 0;
            param.bPersist  = false;
            EventManager()->ScheduleEvent(param, GetEventMapRef());
        }
    }
    m_setDealySendShow.insert(m_ViewActorsByType[ACT_PLAYER].begin(), m_ViewActorsByType[ACT_PLAYER].end());
    __LEAVE_FUNCTION
}

void CActor::RemoveDelaySendShowTo(OBJID id)
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty())
        return;

    m_setDealySendShow.erase(id);
    if(m_setDealySendShow.empty())
    {
        GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
    }
    __LEAVE_FUNCTION
}

void CActor::SendShowToDealyList()
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty())
        return;
    BroadcastShowTo(SceneService()->IDList2VSMap(m_setDealySendShow, 0));
    m_setDealySendShow.clear();
    __LEAVE_FUNCTION
}

void CActor::SendShowTo(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    if(pPlayer == nullptr)
        return;

    SC_AOI_NEW msg;
    MakeShowData(msg);
    pPlayer->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CActor::OnReciveAOIUpdate(const BROADCAST_SET& setBCActorDel, const BROADCAST_SET& setBCActorAdd)
{
    __ENTER_FUNCTION
    // step4: 需要离开视野的角色Remove
    OnAOIProcess_ActorRemoveFromAOI(setBCActorDel);

    // 设置角色广播集=当前广播集-离开视野的差集
    OnAOIProcess_PosUpdate();

    //////////////////////////////////////////////////////////////////////////
    // step5: 新进入视野的角色和地图物品Add
    OnAOIProcess_ActorAddToAOI(setBCActorAdd);

    __LEAVE_FUNCTION
}

void CActor::OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel)
{
    __ENTER_FUNCTION
    if(GetActorType() == ACT_PLAYER)
    {
        //通知自己删除del列表
        SC_AOI_REMOVE remove_msg;
        remove_msg.set_scene_idx(GetSceneIdx());
        remove_msg.mutable_idlist()->Add(setBCActorDel.begin(), setBCActorDel.end());
        SendMsg(remove_msg);
    }
    //////////////////////////////////////////////////////////////////////////
    // step4: 需要离开视野的角色Remove
    {
        BROADCAST_SET setBCActorDelPlayer;
        for(const auto& id: setBCActorDel)
        {
            CActor* pActor = static_cast<CActor*>(GetCurrentScene()->QuerySceneObj(id));
            // 通知自己对方消失
            RemoveFromViewList(pActor, id, true);
            if(pActor)
            {
                // 通知对方自己消失
                pActor->RemoveFromViewList(this, this->GetID(), true);
                //如果目标是Player，需要收到删除我的广播
                if(pActor->GetActorType() == ACT_PLAYER)
                {
                    setBCActorDelPlayer.insert(id);
                }
            }
        }

        //通知Del列表删除自己
        if(setBCActorDelPlayer.empty() == false)
        {
            SC_AOI_REMOVE ntc_aoiInfo;
            ntc_aoiInfo.set_scene_idx(GetSceneIdx());
            ntc_aoiInfo.add_idlist(GetID());

            auto setSocketMap = SceneService()->IDList2VSMap(setBCActorDelPlayer, 0);
            SceneService()->SendProtoMsgTo(setSocketMap, ntc_aoiInfo);
        }
    }

    __LEAVE_FUNCTION
}

void CActor::OnAOIProcess_PosUpdate()
{
    __ENTER_FUNCTION
    //发送移动同步
    SC_AOI_UPDATE ntc;
    ntc.set_scene_idx(GetSceneIdx());
    ntc.set_actor_id(GetID());
    ntc.set_posx(GetPosX());
    ntc.set_posy(GetPosY());

    SendRoomMessage(ntc);
    __LEAVE_FUNCTION
}

void CActor::OnAOIProcess_ActorAddToAOI(const BROADCAST_SET& setBCActorAdd)
{
    __ENTER_FUNCTION
    BROADCAST_SET setBCActorAddPlayer;
    for(const auto& id: setBCActorAdd)
    {
        CActor* pActor = static_cast<CActor*>(GetCurrentScene()->QuerySceneObj(id));
        if(pActor == nullptr)
        {
            continue;
        }

        AddToViewList(pActor);
        pActor->AddToViewList(this);

        // 如果自己是Player， 加入我视野的对象， 需要将他们的信息发送给我
        if(GetActorType() == ACT_PLAYER)
        {
            pActor->AddDelaySendShowTo(GetID());
        }

        //如果目标是Player，需要收到我的show广播
        if(pActor->GetActorType() == ACT_PLAYER)
        {
            setBCActorAddPlayer.insert(id);
        }
    }

    //合并延迟发送Show的队列一起发送
    if(setBCActorAddPlayer.size() > 0 || m_setDealySendShow.size() > 0)
    {
        if(m_setDealySendShow.empty())
        {
            BroadcastShowTo(SceneService()->IDList2VSMap(setBCActorAddPlayer, 0));
        }
        else if(setBCActorAddPlayer.empty())
        {
            BroadcastShowTo(SceneService()->IDList2VSMap(m_setDealySendShow, 0));
            m_setDealySendShow.clear();
            GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
        }
        else
        {
            m_setDealySendShow.insert(setBCActorAddPlayer.begin(), setBCActorAddPlayer.end());
            BroadcastShowTo(SceneService()->IDList2VSMap(m_setDealySendShow, 0));
            m_setDealySendShow.clear();
            GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
        }
    }
    __LEAVE_FUNCTION
}