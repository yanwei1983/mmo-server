#ifndef TEAMINFOMANAGER_H
#define TEAMINFOMANAGER_H

#include "BaseCode.h"

export_lua class CTeamInfo : public NoncopyableT<CTeamInfo>
{
    CTeamInfo() {}

public:
    CreateNewImpl(CTeamInfo);

public:
    ~CTeamInfo() {}

    bool Init(OBJID idTeam, uint64_t idLeader);

    void OnSetLeader(OBJID idLeader);
    void OnAddMember(OBJID idMember);
    void OnDelMember(OBJID idMember);
    void OnDestroy();

public:
    export_lua size_t GetMemeberAmount() const;
    export_lua OBJID  GetMemberIDByIdx(uint32_t idx);
    export_lua bool   IsTeamMember(OBJID idActor) const;
    export_lua bool   IsTeamLeader(OBJID idActor) const;
    export_lua OBJID  GetTeamLeaderID() const;

private:
    OBJID              m_idTeam   = 0;
    OBJID              m_idLeader = 0;
    std::vector<OBJID> m_setMemberID;

public:
    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CTeamInfoManager : public NoncopyableT<CTeamInfoManager>
{
    CTeamInfoManager();

public:
    CreateNewImpl(CTeamInfoManager);

public:
    ~CTeamInfoManager() {}

    bool       Init();
    CTeamInfo* OnCreateTeam(uint64_t idTeam, uint64_t idLeader);
    void       OnDestoryTeam(uint64_t idTeam);

    export_lua CTeamInfo* QueryTeam(uint64_t idTeam);

private:
    std::map<uint64_t, CTeamInfo*> m_setTeam;
};
#endif /* TEAMINFOMANAGER_H */
