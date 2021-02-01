#include "AISkill.h"

#include "AIMonster.h"
#include "AIService.h"

CAISkillData::CAISkillData() {}

CAISkillData::~CAISkillData() {}

bool CAISkillData::Init(uint32_t idSkill)
{
    m_idSkill    = idSkill;
    m_pSkillType = SkillTypeSet()->QueryObj(idSkill);
    CHECKF_M(m_pSkillType, fmt::format(FMT_STRING("Skill {} Not Find"), idSkill).c_str());

    return true;
}

void CAISkillData::StartCoolDown()
{
    m_tCoolDown = TimeGetMillisecond() + GetSkillType()->GetCDSec();
}

bool CAISkillData::IsCoolDown()
{
    return m_tCoolDown > (uint64_t)TimeGetMillisecond();
}

const CSkillType* CAISkillData::GetSkillType() const
{
    return m_pSkillType;
}

uint32_t CAISkillData::GetSkillTypeID() const
{
    return m_idSkill;
}

CAISkillSet::CAISkillSet() {}

CAISkillSet::~CAISkillSet()
{
    for(auto& pair_val: m_setSkill)
    {
        SAFE_DELETE(pair_val.second);
    }
    m_setSkill.clear();
}

void CAISkillSet::AddSkill(uint32_t idSkill)
{
    __ENTER_FUNCTION
    auto pSkilLData = CreateNew<CAISkillData>(idSkill);
    if(pSkilLData)
        m_setSkill[idSkill] = pSkilLData;

    __LEAVE_FUNCTION
}

bool CAISkillSet::CastSkill(CAISkillData* pSkill)
{
    __ENTER_FUNCTION
    if(pSkill == nullptr)
        return false;
    if(pSkill->IsCoolDown())
        return false;
    if(m_pMonster->GetMP() < pSkill->GetSkillType()->GetUseMP())
        return false;
    // send cast skill to ai
    pSkill->StartCoolDown();
    return true;

    __LEAVE_FUNCTION
    return false;
}

CAISkillData* CAISkillSet::ChooseSkill(const SkillFAM* pSkillFAM, double dist, double self_hp, double self_mp, double target_hp) const
{
    __ENTER_FUNCTION
    //遍历所有技能
    double        cur_val  = 0.0;
    CAISkillData* pCurData = nullptr;
    for(const auto& [skillid, pData]: m_setSkill)
    {
        if(pData->IsCoolDown())
            continue;

        double util_value = 0.0f;
        if(pSkillFAM)
        {
            double skill_dis     = pData->GetSkillType()->GetDistance();
            double skill_pow     = pData->GetSkillType()->GetPower();
            double skill_mp      = pData->GetSkillType()->GetUseMP();
            double skill_usetime = pData->GetSkillType()->GetTotalMS();
            double skill_cdtime  = pData->GetSkillType()->GetCDSec();
            util_value = pSkillFAM->calculate(dist, self_hp, self_mp, target_hp, skill_dis, skill_pow, skill_usetime, skill_cdtime, skill_mp);
        }
        else
        {
            util_value = random_float(0.0f, 1.0f);
        }
        if(pCurData == nullptr || cur_val < util_value)
        {
            cur_val  = util_value;
            pCurData = pData;
        }
    }

    return pCurData;

    __LEAVE_FUNCTION

    return nullptr;
}

void CAISkillSet::OnCastSkill(uint32_t idSkill)
{
    __ENTER_FUNCTION
    auto it = m_setSkill.find(idSkill);
    if(it == m_setSkill.end())
        return;
    auto pData = it->second;
    pData->StartCoolDown();
    __LEAVE_FUNCTION
}
