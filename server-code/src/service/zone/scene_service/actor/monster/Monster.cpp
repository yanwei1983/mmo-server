#include "Monster.h"

#include "ActorAttrib.h"
#include "ActorManager.h"
#include "CoolDown.h"
#include "DataCount.h"
#include "GameMap.h"
#include "MonsterType.h"
#include "Phase.h"
#include "Player.h"
#include "PlayerTask.h"
#include "Scene.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "TeamInfoManager.h"
#include "msg/zone_service.pb.h"
#include "protomsg_to_cmd.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CMonster, s_heap);

CMonster::CMonster() {}

CMonster::~CMonster()
{
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);
}

bool CMonster::Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint64_t idPhase, uint32_t idCamp)
{
    __ENTER_FUNCTION
    const CMonsterType* pType = MonsterTypeSet()->QueryObj(idMonsterType);
    CHECKF(pType);

    m_idOwner = idOwner;
    m_idGen   = idGen;
    m_pType   = pType;
    SetID(ActorManager()->GenMonsterID());
    SetCampID(idCamp);
    _SetPhaseID(idPhase);
    CHECKF(CActor::Init());

    m_ActorAttrib->SetBase(m_pType->GetAbility());

    if(idOwner != 0)
    {
        CPlayer* pPlayer = ActorManager()->QueryPlayer(idOwner);
        if(pPlayer)
        {
            //是否需要根据玩家的属性,放大怪物的属性

            m_idOwner = idOwner;
        }
    }
    RecalcAttrib(true);
    _SetHP(GetHPMax());
    _SetMP(GetMPMax());

    if(m_pType->GetScriptID())
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScriptID(), SCB_MONSTER_ONBORN, this);
    }

    m_pCDSet.reset(CCoolDownSet::CreateNew());
    CHECKF(m_pCDSet.get());

    return true;
    __LEAVE_FUNCTION
    return false;
}
uint32_t CMonster::GetLev() const
{
    return m_pType->GetLevel();
}
const std::string& CMonster::GetName() const
{
    return m_pType->GetName();
}
uint32_t CMonster::GetTypeID() const
{
    return m_pType->GetID();
}
bool CMonster::IsBoss() const
{
    return m_pType->GetType() == MONSTER_TYPE_BOSS;
}
bool CMonster::IsElit() const
{
    return m_pType->GetType() == MONSTER_TYPE_ELIT;
}

bool CMonster::SendMsg(const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    auto cmd = msg_to_cmd(msg);
    if(cmd == CMD_SC_SKILL_STUN || cmd == CMD_SC_PROPERTY_CHANGE)
    {
        return SceneService()->SendProtoMsgToAIService(msg);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CMonster::CanDamage(CActor* pTarget) const
{
    if(m_idOwner != 0)
    {
        CActor* pOwner = ActorManager()->QueryActor(m_idOwner);
        if(pOwner)
            return pOwner->CanDamage(pTarget);
    }

    return GetCampID() != pTarget->GetCampID();
}

void CMonster::BeKillBy(CActor* pAttacker)
{
    __ENTER_FUNCTION
    CActor::BeKillBy(pAttacker);
    if(m_pType->GetScriptID())
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScriptID(), SCB_MONSTER_ONBEKILL, this);
    }

    CPlayer* pKillerPlayer = nullptr;
    if(pAttacker == nullptr)
    {
        //从HateList取得最高的
        m_HateList.FindIF([this, &pAttacker](ST_HATE_DATA* pHateData) {
            CActor* pActor = ActorManager()->QueryActor(pHateData->idTarget);
            if(pActor == nullptr)
                return false;
            if(GameMath::distance(pActor->GetPos(), GetPos()) > MIN_EXP_SHARED_DIS)
                return false;

            //设置目标
            pAttacker = pActor;
            return true;
        });
    }

    if(pAttacker)
    {
        if(pAttacker->IsPlayer())
        {
            pKillerPlayer = pAttacker->CastTo<CPlayer>();
        }
        else if(pAttacker->IsPet())
        {
            auto pActor = ActorManager()->QueryActor(pAttacker->GetOwnerID());
            if(pActor)
                pKillerPlayer = pActor->CastTo<CPlayer>();
        }
    }

    if(pKillerPlayer)
    {
        //产生经验
        uint32_t kill_exp = m_pType->GetKillExp();
        if(pKillerPlayer->GetTeamID() == 0)
        {
            pKillerPlayer->AwardBattleExp(kill_exp, true);
            pKillerPlayer->GetTaskSet()->OnKillMonster(GetTypeID(), true);
        }
        else
        {
            //遍历队员， 在周围的才算
            auto pTeam = TeamManager()->QueryTeam(pKillerPlayer->GetTeamID());
            if(pTeam)
            {
                std::vector<CPlayer*> setMember{pKillerPlayer};
                uint32_t              nTotalMemberLev = pKillerPlayer->GetLev();
                for(uint32_t i = 0; i < pTeam->GetMemeberAmount(); i++)
                {
                    OBJID idMember = pTeam->GetMemberIDByIdx(i);
                    if(idMember == pKillerPlayer->GetID())
                        continue;
                    CActor* pMember = ActorManager()->QueryActor(idMember);
                    if(pMember == nullptr)
                        continue;
                    ;
                    if(GameMath::distance(pMember->GetPos(), GetPos()) <= MIN_EXP_SHARED_DIS)
                    {
                        setMember.push_back(pMember->CastTo<CPlayer>());
                        nTotalMemberLev += pMember->GetLev();
                    }
                }

                constexpr uint32_t TEAM_EXP_ADJ[] = {
                    100,
                    100,
                    102,
                    104,
                    106,
                    108,
                    110,
                };
                //组队加成
                //组队分享
                kill_exp = MulDiv(kill_exp, TEAM_EXP_ADJ[setMember.size()], 100);
                for(CPlayer* pMember: setMember)
                {
                    uint32_t nExp = MulDiv(kill_exp, pMember->GetLev(), nTotalMemberLev);
                    pMember->AwardBattleExp(nExp, pMember == pKillerPlayer);
                    pKillerPlayer->GetTaskSet()->OnKillMonster(GetTypeID(), pMember == pKillerPlayer);
                }
            }
        }

        pKillerPlayer->GetDataCountSet()->AddCount(DATA_ACC_MONSTERKILL, GetTypeID(), 1, UPDATE_FALSE);
        pKillerPlayer->GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_KILL_MONSTER, 1, UPDATE_FALSE);
        if(IsBoss())
            pKillerPlayer->GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_KILL_BOSS, 1, UPDATE_FALSE);
    }

    //前端尸体保留N秒
    GetCurrentScene()->LeaveMap(this);
    DelThis();
    __LEAVE_FUNCTION
}

void CMonster::OnBeAttack(CActor* pAttacker, int32_t nRealDamage)
{
    __ENTER_FUNCTION

    if(pAttacker)
        m_HateList.AddHate(pAttacker->GetID(), nRealDamage);

    CActor::OnBeAttack(pAttacker, nRealDamage);
    if(m_pType->GetScriptID())
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScriptID(), SCB_MONSTER_ONBEATTACK, this, pAttacker, nRealDamage);
    }

    __LEAVE_FUNCTION
}

void CMonster::OnEnterMap(CSceneBase* pScene)
{
    __ENTER_FUNCTION
    CActor::OnEnterMap(pScene);

    ServerMSG::ActorCreate aoi_msg;
    aoi_msg.set_actor_id(GetID());
    aoi_msg.set_scene_id(GetSceneIdx());
    aoi_msg.set_actortype(ACT_MONSTER);
    aoi_msg.set_prof(GetTypeID());
    aoi_msg.set_lev(GetLev());
    aoi_msg.set_campid(GetCampID());
    aoi_msg.set_phase_id(GetPhaseID());
    aoi_msg.set_name(GetName());
    
    aoi_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    aoi_msg.set_posx(GetPosX());
    aoi_msg.set_posy(GetPosY());
    aoi_msg.set_monster_gen_id(m_idGen);
    aoi_msg.set_ownerid(GetOwnerID());
    aoi_msg.set_need_sync_ai(NeedSyncAI());
    aoi_msg.set_must_see(IsBoss());
    SceneService()->SendProtoMsgToAOIService(aoi_msg);



    aoi_msg.set_hp(GetHP());
    aoi_msg.set_hpmax(GetHPMax());
    aoi_msg.set_mp(GetMP());
    aoi_msg.set_mpmax(GetMPMax());
    SceneService()->SendProtoMsgToAIService(aoi_msg);

    
    __LEAVE_FUNCTION
}

void CMonster::OnLeaveMap(uint16_t idTargetMap)
{
    __ENTER_FUNCTION
    CActor::OnLeaveMap(idTargetMap);

    __LEAVE_FUNCTION
}

bool CMonster::IsEnemy(CSceneObject* pActor) const
{
    __ENTER_FUNCTION
    CHECKF(pActor);
    if(this == pActor)
        return false;

    if(m_idOwner != 0)
    {
        CActor* pOwner = QueryOwner();
        if(pOwner)
            return pOwner->CanDamage(static_cast<CActor*>(pActor));
    }

    return GetCampID() != static_cast<CActor*>(pActor)->GetCampID();
    __LEAVE_FUNCTION
    return false;
}

void CMonster::MakeShowData(SC_AOI_NEW& msg)
{
    __ENTER_FUNCTION
    msg.set_scene_idx(GetSceneIdx());
    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_MONSTER);
    msg.set_prof(GetTypeID());
    msg.set_lev(GetLev());

    msg.set_campid(GetCampID());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());
    __LEAVE_FUNCTION
}
