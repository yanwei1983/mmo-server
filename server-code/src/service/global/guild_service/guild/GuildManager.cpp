#include "GuildManager.h"

#include "Guild.h"
#include "GuildService.h"
#include "MsgWorldProcess.h"
#include "User.h"
#include "UserManager.h"
#include "msg/zone_service.pb.h"

CGuildManager::CGuildManager() {}

CGuildManager::~CGuildManager()
{
    Destroy();
}

bool CGuildManager::Init()
{
    __ENTER_FUNCTION

    auto pDB        = GuildService()->GetGlobalDB();
    auto result_ptr = pDB->QueryAll(TBLD_GUILD::table_name());
    if(result_ptr)
    {
        constexpr size_t GUESS_ALL_GUILD_COUNT = 50000;
        size_t           row_count             = result_ptr->get_num_row();
        size_t           reserve_size          = std::max<size_t>(row_count, GUESS_ALL_GUILD_COUNT);
        m_setGuildByName.reserve(reserve_size);
        m_setGuild.reserve(reserve_size);
        constexpr size_t GUESS_MEMBER_PER_GUILD = 30;
        m_setGuildMember.reserve(reserve_size * GUESS_MEMBER_PER_GUILD);

        for(size_t i = 0; i < result_ptr->get_num_row(); i++)
        {
            auto        db_record_ptr = result_ptr->fetch_row(true);
            uint64_t    guild_id      = db_record_ptr->Field(TBLD_GUILD::ID);
            std::string guild_name    = db_record_ptr->Field(TBLD_GUILD::NAME);
            uint64_t    del_time      = db_record_ptr->Field(TBLD_GUILD::DEL_TIME);
            m_setGuildByName.emplace(guild_name, guild_id);

            if(del_time != 0)
            {
                std::unique_ptr<CGuild> pGuild = CGuild::CreateNew(std::move(db_record_ptr));
                if(pGuild)
                {
                    m_setGuild.emplace(pGuild->GetGuildID(), std::move(pGuild));
                }
            }
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CGuildManager::Destroy()
{
    __ENTER_FUNCTION
    m_setGuildMember.clear();
    m_setGuildByName.clear();
    m_setGuild.clear();
    __LEAVE_FUNCTION
}

bool CGuildManager::CreateGuild(const std::string& strGuildName, const GuildMemberInfo& leader_info)
{
    __ENTER_FUNCTION

    CHECKF(m_setGuildMember.count(leader_info.get_name()) == 0);

    auto pDB = GuildService()->GetGlobalDB();
    CHECKF(pDB);

    if(QueryGuildIDByName(strGuildName) == 0)
    {
        return false;
    }
    auto pDBRecord = pDB->MakeRecord(TBLD_GUILD::table_name());
    auto now       = TimeGetSecond();

    pDBRecord->Field(TBLD_GUILD::ID)           = 0;
    pDBRecord->Field(TBLD_GUILD::LEV)          = 1;
    pDBRecord->Field(TBLD_GUILD::NAME)         = strGuildName;
    pDBRecord->Field(TBLD_GUILD::LEADER_ID)    = leader_info.get_member_id();
    pDBRecord->Field(TBLD_GUILD::LEADER_NAME)  = leader_info.get_member_name();
    pDBRecord->Field(TBLD_GUILD::CREATOR_ID)   = leader_info.get_member_id();
    pDBRecord->Field(TBLD_GUILD::CREATOR_NAME) = leader_info.get_member_name();
    pDBRecord->Field(TBLD_GUILD::CREATE_TIME)  = now;
    pDBRecord->Field(TBLD_GUILD::DEL_TIME)     = 0;
    if(pDBRecord->Update(true) == false)
    {
        LOGDEBUG("CGuildManager: CreateGuild Fail. guildname:{}", strGuildName);
        return nullptr;
    }
    uint64_t idGuild = pDBRecord->Field(TBLD_GUILD::ID);
    LOGINFO("CGuildManager: CreateGuild Succ. guild_id:{}, guildname:{}", idGuild, strGuildName);

    std::unique_ptr<CGuild> pGuild(CGuild::CreateNew(std::move(pDBRecord)));
    CHECKF(pGuild);
    m_setGuildByName.emplace(pGuild->GetGuildName(), idGuild);
    m_setGuild.emplace(pGuild->GetGuildID(), std::move(pGuild));

    pGuild->AddMember(leader_info.get_member_id(), GUILD_RANK_LEADER, leader_info);
    return true;
    __LEAVE_FUNCTION
    return false;
}

CGuild* CGuildManager::QueryGuild(uint64_t idGuild)
{
    __ENTER_FUNCTION
    auto it = m_setGuild.find(idGuild);
    if(it != m_setGuild.end())
        return it->second.get();

    __LEAVE_FUNCTION
    return nullptr;
}

uint64_t CGuildManager::QueryGuildIDByName(const std::string& strGuildName) const
{
    __ENTER_FUNCTION
    auto it = m_setGuildByName.find(strGuildName);
    if(it != m_setGuildByName.end())
        return it->second;

    __LEAVE_FUNCTION
    return 0;
}

uint64_t CGuildManager::QueryGuildIDByUserID(uint32_t player_id) const
{
    __ENTER_FUNCTION
    auto it = m_setGuildMember.find(player_id);
    if(it != m_setGuildMember.end())
        return it->second;

    __LEAVE_FUNCTION
    return 0;
}

bool CGuildManager::DestoryGuild(uint64_t idGuild)
{
    __ENTER_FUNCTION
    CGuild* pGuild = QueryGuild(idGuild);
    CHECKF(pGuild);

    pGuild->OnDestroy();
    pGuild->SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_DESTORY, 0, 0);

    m_setGuild.erase(pGuild->GetGuildName());
    m_setGuild.erase(idGuild);

    return true;

    __LEAVE_FUNCTION
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
ON_SERVERMSG(CGuildService, GuildCreate)
{
    GuildManager()->CreateGuild(GuildService()->CreateUID(), msg.guild_name(), msg.leader_id(), msg.leader_name());
}

ON_SERVERMSG(CGuildService, GuildQuit)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->QuitGuild(msg.operator_id());
}

ON_SERVERMSG(CGuildService, GuildKickMember)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->KickMember(msg.operator_id(), msg.kick_id());
}

ON_SERVERMSG(CGuildService, GuildInviteMember)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->InviteMember(msg.operator_id(), msg.invitee_id());
}

ON_SERVERMSG(CGuildService, GuildAcceptInvite)
{

    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    pGuild->AcceptInvite(msg.inviter_id(), msg.invitee_id(), msg.result());
}

ON_SERVERMSG(CGuildService, GuildApplyMember)
{
    CUser* pApplicant = UserManager()->QueryUser(msg.applicant_id());
    if(pApplicant == nullptr)
    {
        return;
    }
    CUser* pUser = UserManager()->QueryUser(msg.respondent_id());
    if(pUser == nullptr)
    {
        // send err msg to sender
        return;
    }

    if(pUser->GetGuildID() != 0)
    {
        CGuild* pGuild = GuildManager()->QueryGuild(pUser->GetGuildID());
        CHECK(pGuild);
        pGuild->ApplyMember(msg.applicant_id());
    }
    else
    {
        //发送申请给队长
        SC_GUILDAPPLYMEMBER msg;
        msg.set_applicant_id(msg.applicant_id());
        msg.set_applicant_name(pApplicant->GetName());
        pUser->SendMsg(msg);
    }
}

ON_SERVERMSG(CGuildService, GuildAcceptApply)
{

    CUser* pUser = UserManager()->QueryUser(msg.respondent_id());
    if(pUser == nullptr)
    {
        return;
    }
    CUser* pApplicant = UserManager()->QueryUser(msg.applicant_id());
    if(pApplicant == nullptr)
    {
        // send err msg to sender
        return;
    }
    if(pApplicant->GetGuildID() != 0)
    {
        // send err msg to sender
        return;
    }

    if(msg.result() == false)
    {
        // send err msg to sender
        return;
    }

    if(pUser->GetGuildID() != 0)
    {
        CGuild* pGuild = GuildManager()->QueryGuild(pUser->GetGuildID());
        CHECK(pGuild);
        pGuild->AcceptApply(msg.applicant_id(), msg.respondent_id(), msg.result());
    }
    else
    {
        // send err msg to sender
    }
}

ON_SERVERMSG(CGuildService, GuildNewLeader)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->SetLeader(msg.operator_id(), msg.new_leader_id(), msg.new_leader_name());
}
