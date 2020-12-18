#ifndef TEAMMANAGER_H
#define TEAMMANAGER_H

#include "BaseCode.h"
#include "game_common_def.h"

class CUser;
class CTeam;

class CNetworkMessage;
class CTeamManager : public NoncopyableT<CTeamManager>
{
    CTeamManager();

public:
    CreateNewImpl(CTeamManager);

public:
    ~CTeamManager();

    bool   Init();
    void   Destroy();
    CTeam* CreateTeam(uint64_t idTeam, OBJID idLeader);
    CTeam* QueryTeam(uint64_t idTeam);
    bool   DestoryTeam(uint64_t idTeam);

private:
    std::map<uint64_t, CTeam*> m_setTeam;
};
#endif /* TEAMMANAGER_H */
