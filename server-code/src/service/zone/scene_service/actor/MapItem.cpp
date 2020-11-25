#include "MapItem.h"

#include "ActorAttrib.h"
#include "ActorManager.h"
#include "GameMap.h"
#include "Phase.h"
#include "Scene.h"
#include "SceneService.h"
#include "ScriptManager.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CMapItem, s_heap);
CMapItem::CMapItem()
{
    SetCampID(CAMP_PLAYER);
}

CMapItem::~CMapItem()
{
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);
}

bool CMapItem::Init(uint32_t idType)
{
    __ENTER_FUNCTION
    m_idType = idType;
    SetID(ActorManager()->GenMapItemID());
    CHECKF(CActor::Init());
    return true;

    __LEAVE_FUNCTION
    return false;
}

void CMapItem::MakeShowData(SC_AOI_NEW& msg)
{
    msg.set_scene_idx(GetSceneIdx());

    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_MAPITEM);
    msg.set_prof(m_idType);
    msg.set_lev(GetLev());

    msg.set_campid(GetCampID());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());
}

void CMapItem::OnEnterMap(CSceneBase* pScene)
{
    CActor::OnEnterMap(pScene);

    ServerMSG::ActorCreate aoi_msg;
    aoi_msg.set_actor_id(GetID());
    aoi_msg.set_scene_id(GetSceneIdx());
    aoi_msg.set_actortype(ACT_MAPITEM);
    aoi_msg.set_prof(GetTypeID());
    aoi_msg.set_lev(GetLev());
    aoi_msg.set_phase_id(GetPhaseID());
    aoi_msg.set_campid(GetCampID());
    aoi_msg.set_name(GetName());
    aoi_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    aoi_msg.set_posx(GetPosX());
    aoi_msg.set_posy(GetPosY());
    aoi_msg.set_ownerid(GetOwnerID());
    aoi_msg.set_need_sync_ai(NeedSyncAI());

    SceneService()->SendProtoMsgToAOIService(aoi_msg);

    // aoi_msg.set_hp(GetHP());
    // aoi_msg.set_hpmax(GetHPMax());
    // aoi_msg.set_mp(GetMP());
    // aoi_msg.set_mpmax(GetMPMax());

    // AI暂时不需要MapItem
    // SceneService()->SendProtoMsgToAIService(aoi_msg);
}

void CMapItem::OnLeaveMap(uint16_t idTargetMap)
{
    CActor::OnLeaveMap(idTargetMap);
}
