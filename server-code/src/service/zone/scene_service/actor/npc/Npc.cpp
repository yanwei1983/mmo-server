#include "Npc.h"

#include "ActorAttrib.h"
#include "ActorManager.h"
#include "GameMap.h"
#include "NpcType.h"
#include "Phase.h"
#include "Player.h"
#include "PlayerDialog.h"
#include "PlayerTask.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "TaskType.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CNpc, s_heap);
CNpc::CNpc()
{
    SetCampID(CAMP_PLAYER);
}

CNpc::~CNpc()
{
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);
}

bool CNpc::Init(uint32_t idType)
{
    m_idType = idType;
    m_pType  = NpcTypeSet()->QueryObj(idType);
    CHECKF(m_pType);
    CHECKF(CActor::Init());

    SetID(ActorManager()->GenNpcID());
    SetCampID(m_pType->GetCampID());
    _SetPhaseID(m_pType->GetPhaseID());
    m_ActorAttrib->SetBase(m_pType->GetAbility());

    RecalcAttrib(true, false);
    _SetHP(GetHPMax());
    _SetMP(GetMPMax());

    return true;
}

uint32_t CNpc::GetLev() const
{
    return m_pType->GetLevel();
}
const std::string& CNpc::GetName() const
{
    return m_pType->GetName();
}

void CNpc::MakeShowData(SC_AOI_NEW& msg)
{
    msg.set_scene_idx(GetSceneIdx());

    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_NPC);
    msg.set_prof(m_idType);
    msg.set_lev(GetLev());
    msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    msg.set_campid(GetCampID());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());
}

void CNpc::OnEnterMap(CSceneBase* pScene)
{
    CActor::OnEnterMap(pScene);

    ScriptManager()->TryExecScript<void>(SCRIPT_NPC, m_pType->GetScriptID(), "OnBorn", this);

    ServerMSG::ActorCreate aoi_msg;
    aoi_msg.set_actor_id(GetID());
    aoi_msg.set_scene_id(GetSceneIdx());
    aoi_msg.set_actortype(ACT_NPC);
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
    // SceneService()->SendProtoMsgToAIService(aoi_msg);
}

void CNpc::OnLeaveMap(uint16_t idTargetMap)
{
    CActor::OnLeaveMap(idTargetMap);
}

void CNpc::_ActiveNpc(CPlayer* pPlayer)
{
    __ENTER_FUNCTION

    //检查挂在改NPC身上有多少个任务
    auto pVecAccept = TaskTypeSet()->QueryTaskByAcceptNpcID(GetTypeID());
    auto pVecSubmit = TaskTypeSet()->QueryTaskBySubmitNpcID(GetTypeID());
    //检查这些任务有哪些是可以接的
    std::vector<CTaskType*> setShowTask;
    if(pVecSubmit)
    {
        for(auto pTaskType: *pVecSubmit)
        {
            if(pPlayer->GetTaskSet()->CanSubmit(pTaskType) == true)
            {
                setShowTask.push_back(pTaskType);
            }
        }
    }
    if(pVecAccept)
    {
        for(auto pTaskType: *pVecAccept)
        {
            if(pPlayer->GetTaskSet()->CanAccept(pTaskType) == true)
            {
                setShowTask.push_back(pTaskType);
            }
        }
    }

    if(setShowTask.empty() && HasFlag(m_pType->GetTypeFlag(), NPC_TYPE_FLAG_SHOP))
    {
        //直接打开商店界面
        auto dialog = pPlayer->GetDialog();
        dialog->DialogBegin("");
        dialog->DialogSend(m_pType->GetShopID());
    }
    if(setShowTask.size() == 1 && HasFlag(m_pType->GetTypeFlag(), NPC_TYPE_FLAG_SHOP) == false)
    {
        //如果只有一个任务，直接跳等待接任务的界面
        auto pTaskType = setShowTask.front();
        pPlayer->GetTaskSet()->ShowTaskDialog(pTaskType->GetID(), GetID());
    }
    else
    {
        auto dialog = pPlayer->GetDialog();
        dialog->DialogBegin(m_pType->GetName());
        dialog->DialogAddText(m_pType->GetDialogText());
        if(HasFlag(m_pType->GetTypeFlag(), NPC_TYPE_FLAG_SHOP))
        {
            dialog->DialogAddLink(DIALOGLINK_TYPE_LIST, m_pType->GetShopLinkName(), DIALOG_FUNC_OPENSHOP, m_pType->GetShopID(), "", GetID());
        }

        for(auto pTaskType: setShowTask)
        {
            dialog->DialogAddLink(DIALOGLINK_TYPE_LIST, pTaskType->GetName(), DIALOG_FUNC_SHOWTASK, pTaskType->GetScriptID(), "", GetID());
        }

        dialog->DialogSend(DIALOGTYPE_NORMAL);
    }
    __LEAVE_FUNCTION
}

void CNpc::ActiveNpc(CPlayer* pPlayer)
{
    __ENTER_FUNCTION

    auto find = ScriptManager()->QueryScriptFunc(SCRIPT_AI, m_pType->GetScriptID(), "OnActive");
    if(find)
    {
        ScriptManager()->ExecStackScriptFunc<void>(this);
        
        return;
        
    }  

    _ActiveNpc(pPlayer);
    __LEAVE_FUNCTION
}
