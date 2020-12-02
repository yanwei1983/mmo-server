#include "ActorAttrib.h"
#include "ActorManager.h"
#include "ActorStatusSet.h"
#include "GameEventDef.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "LoadingThread.h"
#include "MapManager.h"
#include "NetMSGProcess.h"
#include "PetSet.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneService.h"
#include "SkillFSM.h"
#include "config/Cfg_Scene_EnterPoint.pb.h"
#include "config/Cfg_Scene_LeavePoint.pb.h"
#include "config/Cfg_Scene_Reborn.pb.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

bool CPlayer::TryChangeMap(uint32_t nLeavePointIdx)
{
    __ENTER_FUNCTION
    auto pLeaveData = GetCurrentScene()->GetMap()->GetLeavePointByIdx(nLeavePointIdx);
    CHECKF_FMT(pLeaveData, "Can't Find LeaveMap {} On Map {}", GetMapID(), nLeavePointIdx);

    CHECKF(GetPos().distance(Vector2(pLeaveData->x(), pLeaveData->y())) > pLeaveData->range());

    auto pGameMap = MapManager()->QueryMap(pLeaveData->dest_map_id());
    CHECKF_FMT(pGameMap, "Can't Find Map {} When LeaveMap {} On Map {}", pLeaveData->dest_map_id(), GetMapID(), nLeavePointIdx);

    //检查所有通行检查
    auto pEnterData = pGameMap->GetEnterPointByIdx(pLeaveData->dest_enter_point_idx());
    CHECKF_FMT(pEnterData,
               "Can't Find EnterPoint {} On Map {} When LeaveMap {} On Map {}",
               pLeaveData->dest_enter_point_idx(),
               pLeaveData->dest_map_id(),
               GetMapID(),
               nLeavePointIdx);

    if(GetTeamMemberCount() < pEnterData->team_req())
    {
        // send errmsg
        return false;
    }

    if(GetGuildLev() < pEnterData->guild_req())
    {
        // send errmsg
        return false;
    }

    if(GetLev() < pEnterData->lev_req())
    {
        return false;
    }

    if(GetVipLev() < pEnterData->vip_lev_req())
    {
        return false;
    }

    return FlyMap(pEnterData->idmap(), pEnterData->idphase(), 0, pEnterData->x(), pEnterData->y(), pEnterData->range(), pEnterData->face());
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::FlyToPhase(CSceneBase* pTargetScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    CHECKF(GetCurrentScene());
    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_FLYMAP) == true)
        return false;

    //场景在本地
    //延迟调用真正的FlyMap
    CEventEntryCreateParam param;
    param.evType    = EVENTID_FLY_MAP;
    param.cb        = std::bind(&CPlayer::_FlyPhase, this, pTargetScene, fPosX, fPosY, fRange, fFace);
    param.tWaitTime = 0;
    param.bPersist  = false;
    EventManager()->ScheduleEvent(param, GetEventMapRef());

    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::FlyMap(uint16_t idMap, uint16_t idPhaseType, uint16_t _nPhaseType, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    CHECKF(GetCurrentScene());
    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_FLYMAP) == true)
        return false;

    //通过原始Mapid查找所在Zone
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF_FMT(pMap, "can't find map {}", idMap);

    TargetSceneID idTargetScene(pMap->GetZoneID(), idMap, idPhaseType, _nPhaseType);

    CScene* pScene = SceneManager()->QueryScene(idMap);
    if(pScene || pMap->IsZoneMap(SceneService()->GetZoneID()))
    {
        //场景在本地
        //延迟调用真正的FlyMap
        CEventEntryCreateParam param;
        param.evType    = EVENTID_FLY_MAP;
        param.cb        = std::bind(&CPlayer::_FlyMap, this, idTargetScene, fPosX, fPosY, fRange, fFace);
        param.tWaitTime = 0;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());

        return true;
    }
    else
    {
        //切换zone
        CEventEntryCreateParam param;
        param.evType    = EVENTID_FLY_MAP;
        param.cb        = std::bind(&CPlayer::_ChangeZone, this, idTargetScene, fPosX, fPosY, fRange, fFace);
        param.tWaitTime = 0;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());

        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

void CPlayer::_FlyMap(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //查询是否有对应地图
    uint16_t idPhaseType = idTargetScene.GetPhaseTypeID();
    uint64_t idPhase     = idPhaseType;
    if(idTargetScene.IsSelfPhaseID())
    {
        idPhase = GetID();
    }
    else if(idTargetScene.IsTeamPhaseID())
    {
        // idPhase = GetTeamLeaderID();
    }
    else if(idTargetScene.IsGuildPhaseID())
    {
        // idPhase = GetGuildLeaderID();
    }

    CPhase* pTargetPhase = SceneManager()->CreatePhase(idTargetScene.GetMapID(), idPhase, idPhase);
    if(pTargetPhase == nullptr)
    {
        // log error
        return;
    }

    _FlyPhase(pTargetPhase, fPosX, fPosY, fRange, fFace);
    __LEAVE_FUNCTION
}

void CPlayer::_FlyPhase(CSceneBase* pTargetPhase, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //查询是否有对应地图
    CSceneBase* pOldPhase = GetCurrentScene();
    CHECK(pOldPhase);
    CHECK(pTargetPhase);
    auto idTargetMap = pTargetPhase->GetSceneIdx().GetMapID();
    LOGACTORDEBUG( GetID(), "Player FlyMap:{} pos:{:.2f} {:.2f} {:.2f}",idTargetMap, fPosX, fPosY, fRange);

    pOldPhase->LeaveMap(this, idTargetMap);
    m_pScene = nullptr;

    //只要地图一样就不需要reloading
    if(pOldPhase->GetSceneIdx().GetMapID() == idTargetMap)
    {
        //判断所在位置是否是障碍
        auto findPos = pTargetPhase->FindPosNearby(Vector2(fPosX, fPosY), fRange);
        pTargetPhase->EnterMap(this, findPos.x, findPos.y, fFace);
    }
    else
    {
        //进入新地图
        m_idLoadingScene = pTargetPhase->GetSceneIdx();
        m_fLoadingPosX   = fPosX;
        m_fLoadingPosY   = fPosY;
        m_fLoadingFace   = fFace;

        SC_LOADMAP msg;
        msg.set_scene_idx(pTargetPhase->GetSceneIdx());
        msg.set_mapid(pTargetPhase->GetMapID());
        msg.set_posx(m_fLoadingPosX);
        msg.set_posy(m_fLoadingPosY);
        msg.set_face(m_fLoadingFace);

        SendMsg(msg);
    }

    __LEAVE_FUNCTION
}

void CPlayer::_ChangeZone(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //从当前场景离开
    if(m_pScene == nullptr)
    {
        // log error
    }
    else
    {
        //从老地图离开
        m_pScene->LeaveMap(this, idTargetScene.GetMapID());
    }

    //删除
    ActorManager()->DelActor(this, false);
    m_pEventOnTimer.Clear();

    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType  = LPT_SAVE;
    data.idPlayer      = GetID();
    data.bChangeZone   = true;
    data.socket        = GetSocket();
    data.idTargetScene = idTargetScene;
    data.fPosX         = fPosX;
    data.fPosY         = fPosY;
    data.fRange        = 0.0f;
    data.fFace         = fFace;
    data.pPlayer       = this;
    SceneService()->GetLoadingThread()->AddClosePlayer(std::move(data));
    __LEAVE_FUNCTION
}

void CPlayer::OnChangeZoneSaveFinish(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //已经Save结束
    //发送ChangeZone消息给World， 由World转发该消息给对应的Zone
    ServerMSG::PlayerChangeZone msg;
    msg.set_socket(GetSocket());
    msg.set_idplayer(GetID());
    msg.set_target_scene(idTargetScene);
    msg.set_posx(fPosX);
    msg.set_posy(fPosY);
    msg.set_range(fRange);
    msg.set_face(fFace);

    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);

    SendGameData(idTargetScene);
    // maybe 需要把当前未处理的消息转发到新zone上
    SceneService()->DelSocketMessagePool(GetSocket());
    __LEAVE_FUNCTION
}

void CPlayer::OnLoadMapSucc()
{
    __ENTER_FUNCTION
    CHECK(m_idLoadingScene != 0);
    CHECK(GetCurrentScene() == nullptr);
    auto pPhase = SceneManager()->QueryPhase(m_idLoadingScene);
    CHECK(pPhase);
    LOGACTORDEBUG(GetID(), "CPlayer::OnLoadMapSucc");

    pPhase->EnterMap(this, m_fLoadingPosX, m_fLoadingPosY, m_fLoadingFace);
    __LEAVE_FUNCTION
}

void CPlayer::SendGameData(const TargetSceneID& idTargetScene)
{
    __ENTER_FUNCTION
    ServerMSG::PlayerChangeZone_Data msg;

    //通过World发送
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    __LEAVE_FUNCTION
}
void CPlayer::OnRecvGameData(CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION

    __LEAVE_FUNCTION
}

void CPlayer::OnEnterMap(CSceneBase* pScene)
{
    __ENTER_FUNCTION

    CHECK(GetCurrentScene() == nullptr);
    LOGACTORDEBUG(GetID(), "Player OnEnterMapStart:{} pos:{:.2f} {:.2f}", pScene->GetMapID(), GetPosX(), GetPosY());
    CActor::OnEnterMap(pScene);

    m_idLoadingScene = 0;
    m_fLoadingPosX   = 0.0f;
    m_fLoadingPosY   = 0.0f;
    m_fLoadingFace   = 0.0f;

    if(pScene->GetMap()->HasMapFlag(MAPFLAG_RECORD_DISABLE) == false)
    {
        m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID) = pScene->GetSceneIdx().data64;
        m_pRecord->Field(TBLD_PLAYER::RECORD_X)       = GetPosX();
        m_pRecord->Field(TBLD_PLAYER::RECORD_Y)       = GetPosY();
        m_pRecord->Field(TBLD_PLAYER::RECORD_FACE)    = GetFace();
    }
    //进入地图了， 保存一下当前的一些修改
    m_pRecord->Update(true);

    {
        ServerMSG::ActorCreate aoi_msg;
        aoi_msg.set_actor_id(GetID());
        aoi_msg.set_scene_id(GetSceneIdx());
        aoi_msg.set_actortype(ACT_PLAYER);
        aoi_msg.set_baselook(GetBaseLook());
        aoi_msg.set_prof(GetProf());
        aoi_msg.set_lev(GetLev());
        aoi_msg.set_campid(GetCampID());
        aoi_msg.set_phase_id(GetPhaseID());
        aoi_msg.set_name(GetName());

        aoi_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
        aoi_msg.set_posx(GetPosX());
        aoi_msg.set_posy(GetPosY());
        aoi_msg.set_ownerid(GetOwnerID());
        aoi_msg.set_guild_id(GetGuildID());
        aoi_msg.set_team_id(GetTeamID());
        aoi_msg.set_need_sync_ai(NeedSyncAI());
        SceneService()->SendProtoMsgToAOIService(aoi_msg);

        aoi_msg.set_hp(GetHP());
        aoi_msg.set_hpmax(GetHPMax());
        aoi_msg.set_mp(GetMP());
        aoi_msg.set_mpmax(GetMPMax());
        SceneService()->SendProtoMsgToAIService(aoi_msg);
    }

    {
        ServerMSG::SyncTaskPhase send;
        send.set_player_id(GetID());
        for(const auto& [k, v]: m_TaskPhase)
        {
            send.add_task_phase_id(k);
        }
        SceneService()->SendProtoMsgToAOIService(send);
    }

    LOGACTORDEBUG(GetID(), "CPlayer::OnEnterMapEnd: mapid: {}", GetSceneIdx());

    {
        SC_ENTERMAP msg;
        msg.set_x(GetPosX());
        msg.set_y(GetPosY());
        msg.set_face(GetFace());
        SendMsg(msg);
    }

    __LEAVE_FUNCTION
}

void CPlayer::OnLeaveMap(uint16_t idTargetMap)
{
    __ENTER_FUNCTION
    CHECK(GetCurrentScene());
    LOGACTORDEBUG(GetID(), "Player OnLeaveMap:{}", GetCurrentScene()->GetMapID());

    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_RECORD_DISABLE) == false)
    {
        m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID) = GetCurrentScene()->GetSceneIdx().data64;
        m_pRecord->Field(TBLD_PLAYER::RECORD_X)       = GetPosX();
        m_pRecord->Field(TBLD_PLAYER::RECORD_Y)       = GetPosY();
        m_pRecord->Field(TBLD_PLAYER::RECORD_FACE)    = GetFace();
    }

    m_SkillFSM->Stop();
    m_pStatusSet->Stop();
    m_pStatusSet->OnLeaveMap();
    m_pPetSet->CallBack();

    CActor::OnLeaveMap(idTargetMap);
    //从这里开始，Player就没有Scene指针了

    __LEAVE_FUNCTION
}

bool CPlayer::Reborn(uint32_t nRebornType)
{
    __ENTER_FUNCTION

    if(IsDead() == false)
        return false;

    if(GetCurrentScene() == nullptr)
        return false;

    switch(nRebornType)
    {
        case REBORN_HOME: //回城复活
        {
            LOGACTORDEBUG(GetID(), "Player Reborn HOME: {}", GetCurrentScene()->GetMapID());
            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, GetHPMax() / 2, SYNC_ALL_DELAY);
            FlyMap(GetHomeSceneIdx().GetMapID(), 0, 0, GetHomePosX(), GetHomePosY(), 0.0f, GetHomeFace());
        }
        break;
        case REBORN_MAPPOS: //复活点复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_MAPPOS) == true)
                return false;

            const auto& pRebornData = GetCurrentScene()->GetMap()->GetRebornDataByIdx(GetCampID());
            CHECKF_FMT(pRebornData, "can't find RebornData In Map {} WithCamp:{}", GetMapID(), GetCampID());

            LOGACTORDEBUG(GetID(), "Player Reborn MAPPOS:{}", GetCurrentScene()->GetMapID());
            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, MulDiv(GetHPMax(), 2, 3), SYNC_ALL_DELAY);
            if(pRebornData->reborn_this_phase() == true)
            {
                FlyToPhase(GetCurrentScene(),
                           pRebornData->reborn_x(),
                           pRebornData->reborn_y(),
                           pRebornData->reborn_range(),
                           pRebornData->reborn_face());
            }
            else
            {
                FlyMap(pRebornData->reborn_map(),
                       0,
                       0,
                       pRebornData->reborn_x(),
                       pRebornData->reborn_y(),
                       pRebornData->reborn_range(),
                       pRebornData->reborn_face());
            }
        }
        break;
        case REBORN_STANDPOS: //原地复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_STANDPOS) == true)
                return false;
            //元宝检查
            LOGACTORDEBUG(GetID(),"Player Reborn STANDPOS:{}",  GetCurrentScene()->GetMapID());
            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, GetHPMax(), SYNC_ALL_DELAY);
            FlyToPhase(GetCurrentScene(), GetPosX(), GetPosY(), 0.0f, GetFace());
        }
        break;
        default:
        {
            return false;
        }
        break;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}
