#ifndef AIACTORMANAGER_H
#define AIACTORMANAGER_H

#include "BaseCode.h"
#include "NetworkDefine.h"
#include "game_common_def.h"

class CAIActor;
class CAIMonster;
class CAIPlayer;

export_lua class CAIActorManager : public NoncopyableT<CAIActorManager>
{
    CAIActorManager();
    bool Init();

public:
    CreateNewImpl(CAIActorManager);

public:
    ~CAIActorManager();

    void Destroy();

    CAIActor* QueryActor(OBJID id) const;
    bool      AddActor(CAIActor* pActor);

    bool DelActor(CAIActor* pActor, bool bDelete = true);
    bool DelActorByID(OBJID id, bool bDelete = true);

    void OnTimer();

protected:
private:
    std::unordered_map<OBJID, CAIActor*> m_ActorMap;
};
#endif /* AIACTORMANAGER_H */
