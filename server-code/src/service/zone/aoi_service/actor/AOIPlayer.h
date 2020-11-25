#ifndef AOIPLAYER_H
#define AOIPLAYER_H
#include "AOIActor.h"
namespace ServerMSG
{
    class ActorCreate;
}

class CAOIPlayer : public CAOIActor
{
    CAOIPlayer();

public:
    ~CAOIPlayer();
    CreateNewImpl(CAOIPlayer);

    virtual ActorType GetActorType() const override { return ActorType::ACT_PLAYER; }
    static ActorType  GetActorTypeStatic() { return ActorType::ACT_PLAYER; }

    void ClearTaskPhase();
    bool CheckTaskPhase(uint64_t idPhase) const;
    void AddTaskPhase(uint64_t idPhase);
    void RemoveTaskPhase(uint64_t idPhase);
    
    export_lua uint32_t GetTeamID() const { return m_idTeam; }
    export_lua void     SetTeamID(uint32_t id) { m_idTeam = id; };
public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    std::unordered_map<uint64_t, uint32_t> m_TaskPhase;
     uint32_t m_idTeam = 0; //TeamID
};
#endif /* AOIPLAYER_H */
