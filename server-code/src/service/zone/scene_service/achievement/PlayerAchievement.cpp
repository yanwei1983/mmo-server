#include "PlayerAchievement.h"

#include "AchievementType.h"
#include "Player.h"
#include "PlayerTask.h"
#include "SceneService.h"
#include "config/Cfg_Achievement.pb.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"

CPlayerAchievement::CPlayerAchievement() {}

CPlayerAchievement::~CPlayerAchievement()
{
    m_setFinish.clear();
    m_setAward.clear();
}

bool CPlayerAchievement::Init(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    CHECKF(pPlayer)
    m_pOwner = pPlayer;
    auto pDB = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECKF(pDB);
    auto pResult = pDB->QueryKey<TBLD_ACHIEVEMENT, TBLD_ACHIEVEMENT::USERID>(m_pOwner->GetID());
    if(pResult)
    {
        for(size_t i = 0; i < pResult->get_num_row(); i++)
        {
            auto     row    = pResult->fetch_row(true);
            uint32_t idAchi = row->Field(TBLD_ACHIEVEMENT::ACHIID);
            uint32_t bTake  = row->Field(TBLD_ACHIEVEMENT::TAKE);
            if(bTake == FALSE)
                m_setAward.insert(idAchi);

            m_setFinish[idAchi] = std::move(row);
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerAchievement::CheckAchiCondition(uint32_t nConditionType, uint32_t nVal0, uint32_t nVal1, uint32_t nVal2)
{
    __ENTER_FUNCTION
    auto pVec = AchievementTypeSet()->QueryAchiemenetByCheckType(nConditionType);
    if(pVec == nullptr)
        return false;

    std::vector<CAchievementType*> setModify;
    std::unordered_set<uint32_t>   setModifyGroup;
    for(uint32_t i = 0; i < pVec->size(); i++)
    {
        const auto& pType = (*pVec)[i];
        if(pType->GetProfReq() != 0 && HasFlag(pType->GetProfReq(), m_pOwner->GetProf()) == false)
            continue;
        if(IsFinish(pType->GetID()) == true)
            continue;

        bool bFinish = _CheckAchiCondition(pType, nVal0, nVal1, nVal2);
        if(bFinish)
        {
            FinishAchievement(pType);
            m_pOwner->GetTaskSet()->OnFinishAchi(pType->GetID());
            setModify.push_back(pType);
            if(pType->GetGroupID() != 0)
                setModifyGroup.insert(pType->GetGroupID());
        }
    }

    if(setModify.empty() == false)
    {
    }

    //检查是否有某一组成就全部达成
    for(auto it = setModifyGroup.begin(); it != setModifyGroup.end(); it++)
    {
        auto pGroupVec = AchievementTypeSet()->QueryAchiemenetByGroupID(*it);
        if(pGroupVec == nullptr)
            continue;
        bool bAllFinish = true;
        std::find_if(pGroupVec->begin(), pGroupVec->end(), [this, &bAllFinish](const auto& pData) {
            if(IsFinish(pData->GetID()) == false)
            {
                bAllFinish = false;
                return true;
            }

            return false;
        });

        if(bAllFinish)
            CheckAchiCondition(CONDITION_FINISH_ACHI, *it, 0, 0);
    }

    return setModify.empty() == false;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerAchievement::TakeReward(uint32_t idAchiType)
{
    __ENTER_FUNCTION
    if(idAchiType == 0)
        return TakeAll();
    const CAchievementType* pType = AchievementTypeSet()->GetData(idAchiType);
    CHECKF(pType);

    auto it_take = m_setAward.find(idAchiType);
    if(it_take == m_setAward.end())
        return false;

    auto it = m_setFinish.find(idAchiType);
    if(it == m_setFinish.end())
    {
        return false;
    }
    auto& refRow = it->second;
    if(refRow->Field(TBLD_ACHIEVEMENT::TAKE) == TRUE)
        return false;

    refRow->Field(TBLD_ACHIEVEMENT::TAKE) = TRUE;
    refRow->Update();
    SendAchiToClient(idAchiType, true);
    m_setAward.erase(it_take);
    // give award
    //给予奖励
    if(pType->GetAwardExp() > 0)
        m_pOwner->AwardExp(pType->GetAwardExp());
    if(pType->GetAwardAchiPoint() > 0)
    {
        m_pOwner->AddProperty(PROP_ACHIPOINT, pType->GetAwardAchiPoint());
    }
    //物品
    for(int32_t i = 0; i < pType->GetDataRef().award_list_size(); i++)
    {
        const auto& v = pType->GetDataRef().award_list(i);
        m_pOwner->AwardItem(0, v.type(), v.num(), v.flag());
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerAchievement::TakeAll()
{
    __ENTER_FUNCTION
    for(auto idAchiType: m_setAward)
    {
        const CAchievementType* pType = AchievementTypeSet()->GetData(idAchiType);
        CHECKF(pType);
        auto it = m_setFinish.find(idAchiType);
        if(it == m_setFinish.end())
        {
            return false;
        }
        auto& refRow = it->second;
        if(refRow->Field(TBLD_ACHIEVEMENT::TAKE) == TRUE)
            continue;

        refRow->Field(TBLD_ACHIEVEMENT::TAKE) = TRUE;
        refRow->Update();

        SendAchiToClient(idAchiType, true);

        // give award
        //给予奖励
        if(pType->GetAwardExp() > 0)
            m_pOwner->AwardExp(pType->GetAwardExp());
        if(pType->GetAwardAchiPoint() > 0)
        {
            m_pOwner->AddProperty(PROP_ACHIPOINT, pType->GetAwardAchiPoint());
        }
        //物品
        for(int32_t i = 0; i < pType->GetDataRef().award_list_size(); i++)
        {
            const auto& v = pType->GetDataRef().award_list(i);
            m_pOwner->AwardItem(0, v.type(), v.num(), v.flag());
        }
    }
    m_setAward.clear();
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayerAchievement::SaveAll()
{
    // don't need
    // for(auto&[idAchiType, row] : m_setFinish)
    //{
    //	row->Update();
    //}
}

void CPlayerAchievement::FinishAchievement(const CAchievementType* pType)
{
    __ENTER_FUNCTION
    auto pDB = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECK(pDB);
    auto pRecord                             = pDB->MakeRecord(TBLD_ACHIEVEMENT::table_name());
    pRecord->Field(TBLD_ACHIEVEMENT::ID)     = SceneService()->CreateUID();
    pRecord->Field(TBLD_ACHIEVEMENT::USERID) = m_pOwner->GetID();
    pRecord->Field(TBLD_ACHIEVEMENT::ACHIID) = pType->GetID();
    CHECK(pRecord->Update());
    m_setFinish[pType->GetID()] = std::move(pRecord);
    m_setAward.insert(pType->GetID());
    __LEAVE_FUNCTION
}

void CPlayerAchievement::SyncAll()
{
    SC_ACHI_INFO msg;
    for(const auto& [idAchiType, row]: m_setFinish)
    {
        auto pData = msg.add_finish_list();
        pData->set_achi_id(idAchiType);
        pData->set_take(row->Field(TBLD_ACHIEVEMENT::TAKE));
    }
    m_pOwner->SendMsg(msg);
}

void CPlayerAchievement::SendAchiToClient(uint32_t idAchiType, bool bTake)
{
    SC_ACHI_INFO msg;
    auto         pData = msg.add_finish_list();
    pData->set_achi_id(idAchiType);
    pData->set_take(bTake);
    m_pOwner->SendMsg(msg);
}

bool CPlayerAchievement::IsFinish(uint32_t idAchiType)
{
    return m_setFinish.find(idAchiType) != m_setFinish.end();
}

bool CPlayerAchievement::_CheckAchiCondition(const CAchievementType* pType, uint32_t nVal0, uint32_t nVal1, uint32_t nVal2)
{
    __ENTER_FUNCTION
    switch(pType->GetCheckData().check_type())
    {
        case CONDITION_LEVEL:       //等级>=val0
        case CONDITION_MONEY:       //铜钱>=val0
        case CONDITION_ACHIPOINT:   //成就点>=val0
        case CONDITION_COMBATPOWER: //排行榜战斗力>=val0
        case CONDITION_USER_FRIEND: //好友数量>=val0
        case CONDITION_VIP:         // VIP等级>=val0
        {
            return nVal0 >= pType->GetCheckData().val0();
        }
        break;
        case CONDITION_TASK: // val0编号的任务处于完成状态
        {
            return nVal0 == pType->GetCheckData().val0();
        }
        break;
        case CONDITION_SKILL_LEARN: //技能id=val0(为0=任意)的技能等级>=val1
        {
            if(pType->GetCheckData().val0() != 0 && pType->GetCheckData().val0() != nVal0)
                return false;
            return nVal1 >= pType->GetCheckData().val1();
        }
        break;
        case CONDITION_EQUIPMENT: //是否装备了val0类型的物品,在装备位置val1(为0=任意)位置
        {
            if(nVal0 == pType->GetCheckData().val0())
            {
                uint32_t nIdx = pType->GetCheckData().val1();
                if(nIdx != 0)
                {
                    return nVal1 == nIdx;
                }
                return true;
            }
        }
        break;
        case CONDITION_EQUIPMENT_SUIT: //是否装备了val0(为0=任意)类型的套装,val1个
        {
            if(pType->GetCheckData().val0() != 0 && pType->GetCheckData().val0() != nVal0)
                return false;
            return nVal1 >= pType->GetCheckData().val1();
        }
        break;
        case CONDITION_EQUIPMENT_QUILITY:  //装备上一件品质>=val0的装备,在装备位置val1(为0=任意)位置
        case CONDITION_EQUIPMENT_ADDITION: //装备上一件强化等级>=val0的装备,在装备位置val1(为0=任意)位置
        {
            if(pType->GetCheckData().val1() == 0 || pType->GetCheckData().val1() == nVal1)
                return nVal0 >= pType->GetCheckData().val0();
            return false;
        }
        break;
        case CONDITION_UPGRADE_ADDITION: //强化一件装备成功，强化后的强化等级>=val0
        {
            return nVal0 >= pType->GetCheckData().val0();
        }
        break;
        case CONDITION_FORMULAITEM: //合成一件装备成功，装备类型ID = val0
        case CONDITION_UPGRADEITEM: //升级一件装备成功，装备类型ID = val0
        {
            return nVal0 == pType->GetCheckData().val0();
        }
        break;
        case CONDITION_FINISH_ACHI: // GroupID=val0的成就全部达成
        {
            return nVal0 == pType->GetCheckData().val0();
        }
        break;
        case CONDITION_DATA_COUNT: //累计计数type=val0, key=val1的value>val2
        {
            if(nVal0 == pType->GetCheckData().val0() && nVal1 == pType->GetCheckData().val1())
                return nVal2 > pType->GetCheckData().val2();
        }
        break;
        default:
            break;
    }
    __LEAVE_FUNCTION
    return false;
}
