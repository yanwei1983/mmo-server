#include "Monster.h"
#include "MsgSceneProcess.h"
#include "Phase.h"
#include "Player.h"
#include "SceneManager.h"
#include "SceneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_SERVERMSG(CSceneService, MonsterGen)
{
    __ENTER_FUNCTION

    CPhase* pPhase = SceneManager()->QueryPhase(msg.scene_id());
    CHECK_FMT(pPhase, "msg.scene_id:{}", msg.scene_id());

    CreateMonsterParam param;
    param.idMonsterType = msg.monster_type();
    param.idGen         = msg.gen_id();
    param.idPhase       = msg.phase_id();
    param.idCamp        = msg.camp_id();
    param.idOwner       = 0;
    param.pos           = Vector2(msg.posx(), msg.posy());
    param.face          = random_float(0.0f, 1.0f);

    CMonster* pMonster = pPhase->CreateMonster(param);
    CHECK(pMonster);

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CSceneService, MonsterGenMulti)
{
    __ENTER_FUNCTION

    CPhase* pPhase = SceneManager()->QueryPhase(msg.scene_id());
    CHECK(pPhase);

    CreateMonsterParam param;
    param.idMonsterType = msg.monster_type();
    param.idGen         = msg.gen_id();
    param.idPhase       = msg.phase_id();
    param.idCamp        = msg.camp_id();
    param.idOwner       = 0;
    param.pos           = Vector2(msg.posx(), msg.posy());
    param.face          = random_float(0.0f, 1.0f);

    bool bSucc = pPhase->CreateMultiMonster(param, msg.num(), msg.range());
    CHECK(bSucc);

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CSceneService, MonsterDestory)
{
    __ENTER_FUNCTION

    for(int32_t i = 0; i < msg.monster_id_size(); i++)
    {
        ActorManager()->DelActorByID(msg.monster_id(i));
    }

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CSceneService, ActorMove)
{
    __ENTER_FUNCTION

    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    pActor->FaceTo(Vector2(msg.x(), msg.y()));
    pActor->MoveTo(Vector2(msg.x(), msg.y()));

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CSceneService, ActorFlyTo)
{
    __ENTER_FUNCTION

    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    pActor->FlyTo(Vector2(msg.x(), msg.y()));

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CSceneService, ActorCastSkill)
{
    __ENTER_FUNCTION

    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    if(pActor->_CastSkill(msg.skill_id(), msg.target_id(), pActor->GetPos()) == false)
    {
        ServerMSG::ActorCastSkill_Fail send_msg;
        send_msg.set_actor_id(msg.actor_id());
        send_msg.set_skill_id(msg.skill_id());
        SceneService()->SendProtoMsgToAIService(send_msg);
    }

    __LEAVE_FUNCTION
}
