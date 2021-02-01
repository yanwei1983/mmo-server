#ifndef AIPLAYER_H
#define AIPLAYER_H
#include "AIActor.h"
namespace ServerMSG
{
    class ActorCreate;
}

export_lua class CAIPlayer : public CAIActor
{
    CAIPlayer();
    bool Init(const ServerMSG::ActorCreate& msg);

public:
    CreateNewImpl(CAIPlayer);

public:
    ~CAIPlayer();

    virtual ActorType GetActorType() const override { return ActorType::ACT_PLAYER; }
    static ActorType  GetActorTypeStatic() { return ActorType::ACT_PLAYER; }

public:
    OBJECTHEAP_DECLARATION(s_heap);
};
#endif /* AIPLAYER_H */
