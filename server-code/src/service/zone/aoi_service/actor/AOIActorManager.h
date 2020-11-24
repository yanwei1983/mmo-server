#ifndef AOIACTORMANAGER_H
#define AOIACTORMANAGER_H

#include "BaseCode.h"
#include "NetworkDefine.h"
#include "game_common_def.h"

class CAOIActor;
class CAOIMonster;
class CAOIPlayer;

class CAOIActorManager : public NoncopyableT<CAOIActorManager>
{
    CAOIActorManager();
    bool Init();

public:
    CreateNewImpl(CAOIActorManager);

public:
    ~CAOIActorManager();

    void Destroy();

    CAOIActor* QueryActor(OBJID id) const;
    bool      AddActor(CAOIActor* pActor);

    bool DelActor(CAOIActor* pActor, bool bDelete = true);
    bool DelActorByID(OBJID id, bool bDelete = true);

    void OnTimer();

protected:
private:
    std::unordered_map<OBJID, CAOIActor*> m_ActorMap;
};
#endif /* AOIACTORMANAGER_H */
