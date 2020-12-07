#include "ActorAttrib.h"
#include "GameMap.h"
#include "Player.h"
#include "PlayerAchievement.h"
#include "SceneService.h"
#include "ScriptManager.h"
#include "UserAttr.h"
#include "config/Cfg_Achievement.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"
void CPlayer::SendAttribToClient()
{
    __ENTER_FUNCTION
    SC_ACTORATTRIB msg;
    for(int32_t i = 0; i < ATTRIB_MAX; i++)
    {
        msg.add_attrib_list(m_ActorAttrib->get(i));
    }

    SendMsg(msg);
    __LEAVE_FUNCTION
}

void CPlayer::RecalcAttrib(bool bClearCache /*= false*/, bool bNotify  /*= true */)
{
    __ENTER_FUNCTION
    if(bClearCache == true)
    {
        // lev给予的基础属性
        auto pData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(GetProf(), GetLev()));
        CHECK(pData);
        m_ActorAttrib->SetBase(pData->GetAbility());
    }

    CActor::RecalcAttrib(bClearCache, bNotify);
    
    SendAttribToClient();

    __LEAVE_FUNCTION
}

void CPlayer::SetPKMode(uint32_t val)
{
    __ENTER_FUNCTION
    switch(val)
    {
        case PKMODE_PEACE:
        case PKMODE_REDNAME:
        case PKMODE_TEAM:
        case PKMODE_GUILD:
        case PKMODE_CAMP:
        case PKMODE_ALL:
        {
            m_nPKMode = val;
        }
        break;
        default:
            return;
            break;
    }

    SC_CHANGE_PKMODE msg;
    msg.set_pkmode(m_nPKMode);
    SendMsg(msg);
    __LEAVE_FUNCTION
}

uint32_t CPlayer::GetHPMax() const
{
    return m_ActorAttrib->get(ATTRIB_HP_MAX);
}
uint32_t CPlayer::GetMPMax() const
{
    return m_ActorAttrib->get(ATTRIB_MP_MAX);
}
uint32_t CPlayer::GetFPMax() const
{
    return m_ActorAttrib->get(ATTRIB_FP_MAX);
}
uint32_t CPlayer::GetNPMax() const
{
    return m_ActorAttrib->get(ATTRIB_NP_MAX);
}

void CPlayer::_SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync /*= SYNC_TRUE*/)
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_LEVEL:
        {
            m_pRecord->Field(TBLD_PLAYER::LEV) = nVal;
            m_pRecord->Update();
            GetAchievement()->CheckAchiCondition(CONDITION_LEVEL, nVal);
        }
        break;
        case PROP_EXP:
        {
            m_pRecord->Field(TBLD_PLAYER::EXP) = nVal;
        }
        break;
        case PROP_MONEY:
        {
            m_pRecord->Field(TBLD_PLAYER::MONEY) = nVal;
            m_pRecord->Update();
            GetAchievement()->CheckAchiCondition(CONDITION_MONEY, nVal);
        }
        break;
        case PROP_MONEYBIND:
        {
            m_pRecord->Field(TBLD_PLAYER::MONEY_BIND) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_GOLD:
        {
            m_pRecord->Field(TBLD_PLAYER::GOLD) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_GOLDBIND:
        {
            m_pRecord->Field(TBLD_PLAYER::GOLD_BIND) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_PKVAL:
        {
            m_pRecord->Field(TBLD_PLAYER::PKVAL) = nVal;
        }
        break;
        case PROP_HONOR:
        {
            m_pRecord->Field(TBLD_PLAYER::HONOR) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_ACHIPOINT:
        {
            m_pRecord->Field(TBLD_PLAYER::ACHIPOINT) = nVal;
            m_pRecord->Update();
            GetAchievement()->CheckAchiCondition(CONDITION_ACHIPOINT, nVal);
        }
        break;
        case PROP_VIPLEV:
        {
            m_pRecord->Field(TBLD_PLAYER::VIPLEV) = nVal;
            m_pRecord->Update();
        }
        break;
        default:
            break;
    }

    CActor::_SetProperty(nType, nVal, nSync);
    __LEAVE_FUNCTION
}

uint32_t CPlayer::GetPropertyMax(uint32_t nType) const
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_EXP:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_MONEY:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_MONEYBIND:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_GOLD:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_GOLDBIND:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_PKVAL:
        {
            return 99999999;
        }
        break;
        case PROP_HONOR:
        {
            return 999999999;
        }
        break;
        case PROP_ACHIPOINT:
        {
            return 999999999;
        }
        break;
        case PROP_VIPLEV:
        {
            return 255;
        }
        break;
        default:
            break;
    }

    return CActor::GetPropertyMax(nType);
    __LEAVE_FUNCTION
    return 0;
}

uint32_t CPlayer::GetProperty(uint32_t nType) const
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_EXP:
        {
            return GetExp();
        }
        break;
        case PROP_MONEY:
        {
            return GetMoney();
        }
        break;
        case PROP_MONEYBIND:
        {
            return GetMoneyBind();
        }
        break;
        case PROP_GOLD:
        {
            return GetGold();
        }
        break;
        case PROP_GOLDBIND:
        {
            return GetGoldBind();
        }
        break;
        case PROP_PKVAL:
        {
            return GetPKVal();
        }
        break;
        case PROP_HONOR:
        {
            return GetHonor();
        }
        break;
        case PROP_ACHIPOINT:
        {
            return GetAchiPoint();
        }
        break;
        case PROP_VIPLEV:
        {
            return GetAchiPoint();
        }
        break;
        default:
            break;
    }

    return CActor::GetProperty(nType);
    __LEAVE_FUNCTION
    return 0;
}

bool CPlayer::CheckMoney(uint32_t nMoneyType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nMoneyType)
    {
        case MT_MONEY:
        {
            return GetMoney() >= nVal;
        }
        break;
        case MT_MONEYBIND:
        {
            return GetMoneyBind() >= nVal;
        }
        break;
        case MT_GOLD:
        {
            return GetGold() >= nVal;
        }
        break;
        case MT_GOLDBIND:
        {
            return GetGoldBind() >= nVal;
        }
        break;
        default:
            break;
    }
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::SpendMoney(uint32_t nMoneyType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nMoneyType)
    {
        case MT_MONEY:
        {
            if(GetMoney() >= nVal)
                SetProperty(PROP_MONEY, GetMoney() - nVal, SYNC_TRUE);
            else
                return false;
        }
        break;
        case MT_MONEYBIND:
        {
            if(GetMoneyBind() >= nVal)
            {
                SetProperty(PROP_MONEYBIND, GetMoneyBind() - nVal, SYNC_TRUE);
            }
            else if((uint64_t)GetMoneyBind() + (uint64_t)GetMoney() >= nVal)
            {
                AddProperty(PROP_MONEY, -(nVal - GetMoneyBind()), SYNC_TRUE);
                SetProperty(PROP_MONEYBIND, 0, SYNC_TRUE);
            }
            else
                return false;
        }
        break;
        case MT_GOLD:
        {
            if(GetGold() >= nVal)
                SetProperty(PROP_GOLD, GetGold() - nVal, SYNC_TRUE);
            else
                return false;
        }
        break;
        case MT_GOLDBIND:
        {
            if(GetGoldBind() >= nVal)
            {
                SetProperty(PROP_GOLDBIND, GetGoldBind() - nVal, SYNC_TRUE);
            }
            else if((uint64_t)GetGoldBind() + (uint64_t)GetGold() >= nVal)
            {
                AddProperty(PROP_GOLD, -(nVal - GetGoldBind()), SYNC_TRUE);
                SetProperty(PROP_GOLDBIND, 0, SYNC_TRUE);
            }
            else
                return false;
        }
        break;
        default:
            break;
    }

    //记录日志

    //即刻回写
    m_pRecord->Update(true);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::AwardMeony(uint32_t nMoneyType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nMoneyType)
    {
        case MT_MONEY:
        {
            AddProperty(PROP_MONEY, nVal, SYNC_TRUE);
        }
        break;
        case MT_MONEYBIND:
        {
            AddProperty(PROP_MONEYBIND, nVal, SYNC_TRUE);
        }
        break;
        case MT_GOLD:
        {
            AddProperty(PROP_GOLD, nVal, SYNC_TRUE);
        }
        break;
        case MT_GOLDBIND:
        {
            AddProperty(PROP_GOLDBIND, nVal, SYNC_TRUE);
        }
        break;
        default:
            break;
    }

    //记录日志

    //即刻回写
    m_pRecord->Update(true);

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayer::AwardExp(uint32_t nExp)
{
    __ENTER_FUNCTION
    nExp = MulDiv(nExp, GetAttrib().get(ATTRIB_EXP_ADJ) + 10000, 10000);

    uint64_t nCurExp = GetExp() + (uint64_t)nExp;
    //计算升级
    auto pData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(GetProf(), GetLev()));
    while(pData && nCurExp > pData->GetLevUpNeedExp())
    {
        nCurExp -= pData->GetLevUpNeedExp();
        AddProperty(PROP_LEVEL, 1, SYNC_ALL_DELAY);
        OnLevUp(GetLev());
        pData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(GetProf(), GetLev()));
    }

    SetProperty(PROP_EXP, (uint32_t)nCurExp, SYNC_TRUE);

    __LEAVE_FUNCTION
}

void CPlayer::AwardBattleExp(uint32_t nExp, bool bKillBySelf)
{
    AwardExp(nExp);
}