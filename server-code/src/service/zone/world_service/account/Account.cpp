#include "Account.h"

#include "AccountInfo.h"
#include "AccountManager.h"
#include "ActorAttrib.h"
#include "BornPos.h"
#include "GMManager.h"
#include "GameMap.h"
#include "MapManager.h"
#include "SceneID.h"
#include "User.h"
#include "UserAttr.h"
#include "UserManager.h"
#include "WorldService.h"
#include "game_common_def.h"
#include "gamedb.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CAccount, s_heap);

CAccount::CAccount() {}

size_t CAccount::GetRoleAmount() const
{
    return m_setActorInfo.size();
}

const ST_ROLE_INFO* CAccount::QueryRoleByIndex(size_t nIdx)
{
    if(nIdx > m_setActorInfo.size())
        return nullptr;
    return m_setActorInfo[nIdx].get();
}

CAccount::~CAccount()
{
    if(m_pUser)
        UserManager()->RemoveUser(m_pUser);

    m_setActorInfo.clear();
}

bool CAccount::Init(const std::string& open_id, const VirtualSocket& from)
{
    //从数据库读取数据
    auto pDB = WorldService()->GetGameDB();

    if(pDB->EscapeString(m_openid, open_id.c_str(), open_id.size()) == false)
        return false;

    ReloadActorInfo();

    m_Socket = from;

    return true;
}

void CAccount::ReloadActorInfo()
{
    __ENTER_FUNCTION

    m_setActorInfo.clear();

    auto pDB        = WorldService()->GetGameDB();
    auto result_ptr = pDB->QueryCond<TBLD_PLAYER>(fmt::format(FMT_STRING("openid=\'{}\' and del_time=0"), m_openid));
    if(result_ptr)
    {
        for(size_t i = 0; i < result_ptr->get_num_row(); i++)
        {
            auto             db_record_ptr = result_ptr->fetch_row(true);
            ST_ROLE_INFO_PTR pInfo         = std::make_unique<ST_ROLE_INFO>(std::move(db_record_ptr));
            m_setActorInfo.push_back(std::move(pInfo));
        }
    }
    __LEAVE_FUNCTION
}

bool CAccount::CreateActor(const std::string& name, uint32_t dwProf, uint32_t dwLook)
{
    __ENTER_FUNCTION
    if(GetRoleAmount() >= _MAX_ROLES_PER_ACCOUNT)
    {
        SC_CREATEACTOR msg;
        msg.set_result_code(SC_CREATEACTOR::EC_MAX_ACTOR);
        WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
        return false;
    }
    if(name.size() > _MAX_NAME_SIZE)
    {
        SC_CREATEACTOR msg;
        msg.set_result_code(SC_CREATEACTOR::EC_NAME_TOO_LONG);
        WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
        return false;
    }
    auto pDB = WorldService()->GetGameDB();

    // 角色昵称允许所有任意字符，因此插入数据库之前需要处理：
    char szName[_MAX_NAME_SIZE * 2 + 1];
    if(pDB->EscapeString(szName, name.c_str(), name.size()) == false)
    {
        SC_CREATEACTOR msg;
        msg.set_result_code(SC_CREATEACTOR::EC_NAME_ILLEGAL);
        WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
        return false;
    }

    if(strlen(szName) < _MIN_NAME_SIZE)
    {
        SC_CREATEACTOR msg;
        msg.set_result_code(SC_CREATEACTOR::EC_NAME_TOO_SHORT);
        WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
        return false;
    }

    //敏感字检验
    if(GMManager()->GetGMLevel(m_openid) == 0)
    {
        if(find_name_error(szName) == true)
        {
            SC_CREATEACTOR msg;
            msg.set_result_code(SC_CREATEACTOR::EC_NAME_ILLEGAL);
            WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
            return false;
        }
    }

    CHECKF_V(PROF_WARRIOR == dwProf || PROF_MAGA == dwProf || PROF_PRIEST == dwProf, dwProf);
    uint32_t dwBaseLook = dwLook;

    const CUserAttrData* pLevData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(dwProf, 1));
    if(pLevData == nullptr)
    {
        return false;
    }
    const AttribList_t& AbilityLevel = pLevData->GetAbility();

    const CBornPos* pBornPos = BornPosSet()->RandGet(dwProf);
    CHECKF_V(pBornPos, dwProf);

    const CGameMap* pMap = MapManager()->QueryMap(pBornPos->GetMapID());
    CHECKF_V(pMap, pBornPos->GetMapID());
    Vector2 bornPos(pBornPos->GetPosX(), pBornPos->GetPoxY());
    auto findPos = pMap->FindPosNearby(bornPos, pBornPos->GetRange());
    CHECKF(findPos);
    bornPos = findPos.value();
    SceneIdx bornScene(pMap->GetZoneID(), pBornPos->GetMapID(), 0);

    {
        auto  db_record_ptr = pDB->MakeRecord(TBLD_PLAYER::table_name());
        OBJID idPlayer      = WorldService()->CreatePlayerID();
        db_record_ptr->Field(TBLD_PLAYER::ID).set(idPlayer);
        db_record_ptr->Field(TBLD_PLAYER::WORLDID).set<uint32_t>(WorldService()->GetServerPort().GetWorldID());
        db_record_ptr->Field(TBLD_PLAYER::ORIWORLDID).set(WorldService()->GetServerPort().GetWorldID());
        db_record_ptr->Field(TBLD_PLAYER::OPENID).set(m_openid);
        db_record_ptr->Field(TBLD_PLAYER::NAME).set(szName);
        db_record_ptr->Field(TBLD_PLAYER::PROF).set(dwProf);
        db_record_ptr->Field(TBLD_PLAYER::BASELOOK).set<uint32_t>(0);
        db_record_ptr->Field(TBLD_PLAYER::LEV).set<uint32_t>(1);
        db_record_ptr->Field(TBLD_PLAYER::VIPLEV).set<uint32_t>(0);

        db_record_ptr->Field(TBLD_PLAYER::RECORD_SCENEID).set(bornScene.data64);
        db_record_ptr->Field(TBLD_PLAYER::RECORD_X).set(bornPos.x);
        db_record_ptr->Field(TBLD_PLAYER::RECORD_Y).set(bornPos.y);
        db_record_ptr->Field(TBLD_PLAYER::RECORD_FACE).set(pBornPos->GetFace());
        db_record_ptr->Field(TBLD_PLAYER::HOME_SCENEID).set(bornScene.data64);
        db_record_ptr->Field(TBLD_PLAYER::HOME_X).set(bornPos.x);
        db_record_ptr->Field(TBLD_PLAYER::HOME_Y).set(bornPos.y);
        db_record_ptr->Field(TBLD_PLAYER::HOME_FACE).set(pBornPos->GetFace());

        if(GMManager()->GetGMLevel(m_openid) != 0)
        {
            db_record_ptr->Field(TBLD_PLAYER::MONEY).set<uint64_t>(10000);
            db_record_ptr->Field(TBLD_PLAYER::MONEY_BIND).set<uint64_t>(10000);
            db_record_ptr->Field(TBLD_PLAYER::GOLD).set<uint64_t>(10000);
            db_record_ptr->Field(TBLD_PLAYER::GOLD_BIND).set<uint64_t>(10000);
        }
        else
        {
            db_record_ptr->Field(TBLD_PLAYER::MONEY).set<uint64_t>(0);
            db_record_ptr->Field(TBLD_PLAYER::MONEY_BIND).set<uint64_t>(0);
            db_record_ptr->Field(TBLD_PLAYER::GOLD).set<uint64_t>(0);
            db_record_ptr->Field(TBLD_PLAYER::GOLD_BIND).set<uint64_t>(0);
        }
        db_record_ptr->Field(TBLD_PLAYER::HP).set(AbilityLevel[ATTRIB_HP_MAX]);
        db_record_ptr->Field(TBLD_PLAYER::MP).set(AbilityLevel[ATTRIB_MP_MAX]);

        uint32_t now = TimeGetSecond();
        db_record_ptr->Field(TBLD_PLAYER::CREATE_TIME).set(now);
        db_record_ptr->Field(TBLD_PLAYER::LAST_LOGINTIME).set(now);
        db_record_ptr->Field(TBLD_PLAYER::LAST_LOGOUTTIME).set(now);

        if(db_record_ptr->Update(true) == true)
        {
            LOGLOGIN("Account:{} CreateActor:{}", GetOpenID().c_str(), name.c_str());
            auto pInfo = std::make_unique<ST_ROLE_INFO>(std::move(db_record_ptr));
            m_setActorInfo.push_back(std::move(pInfo));

            //通知前端创建成功

            SC_CREATEACTOR msg;
            msg.set_result_code(SC_CREATEACTOR::EC_SUCC);
            WorldService()->SendMsgToVirtualSocket(m_Socket, msg);

            // todo:DLOG记录玩家创建角色日志
            return true;
        }
        else
        {
            LOGLOGIN("Account:{} CreateActorFail:{}", GetOpenID().c_str(), name.c_str());
            WorldService()->RecyclePlayerID(idPlayer);
            SC_CREATEACTOR msg;
            msg.set_result_code(SC_CREATEACTOR::EC_SAME_NAME);
            WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
        }
        return false;
    }

    __LEAVE_FUNCTION

    return false;
}

void CAccount::SelectActor(size_t nIdx)
{
    __ENTER_FUNCTION
    if(m_setActorInfo.size() < nIdx)
        return;
    LOGLOGIN("Account:{} SelectActor:{}", GetOpenID().c_str(), nIdx);
    if(m_pUser != nullptr)
    {
        // kick player first
        ExitZone(false);
    }
    auto& pInfo = m_setActorInfo[nIdx];

    m_pUser = UserManager()->CreateUser(this, pInfo.get());
    if(m_pUser == nullptr)
    {
        // send error to client
        return;
    }
    m_pUser->EnterZone();
    __LEAVE_FUNCTION
}

void CAccount::DelActor(size_t nIdx)
{
    __ENTER_FUNCTION
    if(m_setActorInfo.size() < nIdx)
        return;
    LOGLOGIN("Account:{} DelActor:{}", GetOpenID().c_str(), nIdx);
    if(m_pUser != nullptr)
    {
        // kick player first
        ExitZone(false);
    }
    auto& pInfo  = m_setActorInfo[nIdx];
    OBJID idUser = pInfo->GetID();
    OBJID idMate = pInfo->GetMateID();

    //通知伴侣
    {
        CUser* pMate = UserManager()->QueryUser(idMate);
        if(pMate)
        {
            pMate->ClearMate();
        }
        else
        {
            //强行设定伴侣的MateID = 0;
            auto pDB = WorldService()->GetGameDB();
            pDB->AsyncExec(fmt::format(FMT_STRING("UPDATE {} SET mate_id=0,mate_name='' WHERE id={} LIMIT 1"), TBLD_PLAYER::table_name(), idMate));
        }
        //发送邮件
    }

    //通知组队系统
    //通知工会系统
    //通知拍卖行系统

    pInfo->ClearMate();
    pInfo->SetDelTime(TimeGetSecond());

    ReloadActorInfo();
    __LEAVE_FUNCTION
}

void CAccount::ExitZone(bool bReload)
{
    __ENTER_FUNCTION
    if(m_pUser == nullptr)
        return;
    LOGLOGIN("Account:{} ExitZone", GetOpenID().c_str());

    //将该角色退出
    m_pUser->Logout();

    UserManager()->RemoveUser(m_pUser);

    //重新读取角色信息
    if(bReload)
    {
        ReloadActorInfo();
        //发送角色信息
        SendActorInfo();
    }

    __LEAVE_FUNCTION
}

void CAccount::KickOut()
{
    ExitZone();
    ServerMSG::SocketClose kick_msg;
    kick_msg.set_vs(GetSocket());
    WorldService()->SendProtoMsgToZonePort(GetSocket().GetServerPort(), kick_msg);
}

void CAccount::SendActorInfo()
{
    __ENTER_FUNCTION
    SC_ACTORINFOLIST msg;
    for(size_t i = 0; i < m_setActorInfo.size(); i++)
    {
        auto* pInfo      = msg.add_list();
        auto& pActorInfo = m_setActorInfo[i];
        pInfo->set_name(pActorInfo->GetName());
        pInfo->set_prof(pActorInfo->GetProf());
        pInfo->set_baselook(pActorInfo->GetBaseLook());
        pInfo->set_lev(pActorInfo->GetLev());
        // pInfo->set_recordmap(pActorInfo->GetRecordMapID());
        // pInfo->set_homemap(pActorInfo->GetHomeMapID());
    }
    WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
    __LEAVE_FUNCTION
}

void CAccount::SendWaitInfo()
{
    SC_WAITINFO msg;
    WorldService()->SendMsgToVirtualSocket(m_Socket, msg);
}

void CAccount::SetWait(bool bWait)
{
    m_bWait = bWait;
}

bool CAccount::IsWait()
{
    return m_bWait;
}
