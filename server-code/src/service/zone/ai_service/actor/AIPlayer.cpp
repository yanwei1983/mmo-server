#include "AIPlayer.h"

#include "server_msg/server_side.pb.h"
OBJECTHEAP_IMPLEMENTATION(CAIPlayer, s_heap);
CAIPlayer::CAIPlayer() {}

CAIPlayer::~CAIPlayer() {}

bool CAIPlayer::Init(const ServerMSG::ActorCreate& msg)
{
    __ENTER_FUNCTION
    CHECKF(CAIActor::Init());

    SetID(msg.actor_id());
    SetLev(msg.lev());
    SetMoveSPD(msg.movespd());
    SetName(msg.name());
    SetCampID(msg.campid());
    _SetPhaseID(msg.phase_id());
    SetHP(msg.hp());
    SetHPMax(msg.hpmax());
    SetMP(msg.mp());
    SetMPMax(msg.mpmax());
    _SetPos(Vector2(msg.posx(), msg.posy()));

    return true;
    __LEAVE_FUNCTION
    return false;
}
