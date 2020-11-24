#ifndef GUILDMANAGER_H
#define GUILDMANAGER_H

#include <unordered_map>

#include "BaseCode.h"
#include "game_common_def.h"


class CGuild;
class CUser;
class CNetworkMessage;
class CGuildManager : public NoncopyableT<CGuildManager>
{
    CGuildManager();

public:
    CreateNewImpl(CGuildManager);

public:
    ~CGuildManager();

    bool Init();
    void Destory();
    bool CreateGuild(uint64_t idGuild, const std::string& strGuildName, OBJID idLeader, const std::string& strLeaderName);

    CGuild*  QueryGuild(uint64_t idGuild) const;
    uint64_t QueryGuildIDByName(const std::string& strGuildName) const;
    bool     DestoryGuild(uint64_t idGuild);
    uint64_t QueryGuildIDByUserID(uint32_t player_id) const;

    void LinkGuildMember(uint64_t player_id, uint64_t guild_id);
    void UnLinkGuildMember(uint64_t player_id);

private:
    std::unordered_map<uint64_t /*player_id*/, uint64_t /*guild_id*/>     m_setGuildMember;
    std::unordered_map<uint64_t /*guild_id*/, std::unique_ptr<CGuild> >   m_setGuild;
    std::unordered_map<std::string /*guild_name*/, uint64_t /*guild_id*/> m_setGuildByName; //包含del_time=0的guild
};

#endif /* GUILDMANAGER_H */
