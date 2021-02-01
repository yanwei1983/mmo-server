#include "SkillManager.h"

#include "Player.h"
#include "PlayerAchievement.h"
#include "SceneService.h"
#include "SkillData.h"
#include "SkillType.h"
#include "config/Cfg_Achievement.pb.h"
#include "gamedb.h"

CPlayerSkillManager::CPlayerSkillManager() {}

CPlayerSkillManager::~CPlayerSkillManager()
{
    __ENTER_FUNCTION
    m_setSkillData.clear();
    __LEAVE_FUNCTION
}

bool CPlayerSkillManager::Init(CPlayer* pOwner)
{
    __ENTER_FUNCTION
    m_pOwner = pOwner;
    auto pDB = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECKF(pDB);
    auto pResult = pDB->QueryKey<TBLD_SKILL, TBLD_SKILL::USERID>(m_pOwner->GetID());
    if(pResult)
    {
        for(size_t i = 0; i < pResult->get_num_row(); i++)
        {
            auto row = pResult->fetch_row(true);

            CSkillData* pData = CreateNew<CSkillData>(m_pOwner, std::move(row));
            if(pData)
            {
                m_setSkillData[pData->GetSkillSort()].reset(pData);
                const CSkillType* pSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(pData->GetSkillSort(), pData->GetSkillLev()));
                if(pSkillType && pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
                {
                    const auto& refList = pSkillType->GetAttrib();
                    m_pOwner->GetAttrib().Store(refList);
                }
            }
            else
            {
                // logerror
            }
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerSkillManager::LearnSkill(uint32_t idSkillSort)
{
    __ENTER_FUNCTION
    auto pSkillData = _QuerySkill(idSkillSort);
    if(pSkillData != nullptr)
        return false;

    const CSkillType* pSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(idSkillSort, 1));
    if(pSkillType == nullptr)
        return false;

    //检查各种学习需求
    if(CheckSkillReq(pSkillType) == false)
    {
        return false;
    }

    //学习技能
    CSkillData* pData = CreateNew<CSkillData>(m_pOwner, idSkillSort, 1);
    if(pData)
    {
        m_setSkillData[pData->GetSkillSort()].reset(pData);
        if(pSkillType && pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
        {
            const auto& refList = pSkillType->GetAttrib();
            m_pOwner->GetAttrib().Store(refList);
        }

        m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_SKILL_LEARN, idSkillSort, 1);

        // lua call onSkillLearn

        //通知前端
        return true;
    }
    else
    {
        // logerror
        return false;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CPlayerSkillManager::CastSkill(uint32_t idSkillSort, OBJID idTarget, const Vector2& pos)
{
    __ENTER_FUNCTION
    auto pSkillData = _QuerySkill(idSkillSort);
    if(pSkillData == nullptr)
        return false;

    return m_pOwner->_CastSkill(CSkillType::MakeID(idSkillSort, pSkillData->GetSkillLev()), idTarget, pos);
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerSkillManager::CheckSkillReq(const CSkillType* pSkillType)
{
    CHECKF(pSkillType);
    if(HasFlag(m_pOwner->GetProf(), pSkillType->GetProfReq()) == false)
    {
        return false;
    }
    if(m_pOwner->GetLev() < pSkillType->GetLearnLevel())
    {
        return false;
    }
    return true;
}

bool CPlayerSkillManager::UpgradeSkill(uint32_t idSkillSort)
{
    __ENTER_FUNCTION
    auto pSkillData = _QuerySkill(idSkillSort);
    if(pSkillData == nullptr)
        return false;

    const CSkillType* pSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(idSkillSort, pSkillData->GetSkillLev() + 1));
    if(pSkillType == nullptr)
        return false;

    //检查各种学习需求
    if(CheckSkillReq(pSkillType) == false)
    {
        return false;
    }
    //属性
    if(pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
    {
        const CSkillType* pOldSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(idSkillSort, pSkillData->GetSkillLev()));
        if(pOldSkillType)
        {
            const auto& refList = pOldSkillType->GetAttrib();
            m_pOwner->GetAttrib().Remove(refList);
        }
    }

    pSkillData->SetSkillLev(pSkillData->GetSkillLev() + 1, UPDATE_TRUE);

    if(pSkillType && pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
    {
        const auto& refList = pSkillType->GetAttrib();
        m_pOwner->GetAttrib().Store(refList);
    }

    m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_SKILL_LEARN, idSkillSort, pSkillData->GetSkillLev());
    return true;
    __LEAVE_FUNCTION
    return false;
}

CSkillData* CPlayerSkillManager::_QuerySkill(uint32_t idSkillSort) const
{
    __ENTER_FUNCTION
    auto it = m_setSkillData.find(idSkillSort);
    if(it == m_setSkillData.end())
        return nullptr;
    return it->second.get();
    __LEAVE_FUNCTION
    return nullptr;
}
