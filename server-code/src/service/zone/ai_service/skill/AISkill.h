#ifndef AISKILL_H
#define AISKILL_H

#include "AIFuzzyLogic.h"
#include "SkillType.h"

class CAISkillData : public NoncopyableT<CAISkillData>
{
    CAISkillData();
    bool Init(uint32_t idSkill);

public:
    CreateNewImpl(CAISkillData);

public:
    ~CAISkillData();

public:
    void StartCoolDown();
    bool IsCoolDown();

    const CSkillType* GetSkillType() const;
    uint32_t          GetSkillTypeID() const;
    
private:
    uint32_t          m_idSkill    = 0;
    const CSkillType* m_pSkillType = nullptr;
    uint64_t          m_tCoolDown  = 0;
};

class CAIMonster;
export_lua class CAISkillSet
{
public:
    CAISkillSet();
    ~CAISkillSet();

    void AddSkill(uint32_t idSkill);

    export_lua bool CastSkill(CAISkillData* pSkill);

    export_lua CAISkillData* ChooseSkill(const SkillFAM* pSkillFAM, double dist, double self_hp, double self_mp, double target_hp) const;
    void          OnCastSkill(uint32_t idSkill);

private:
    CAIMonster*                                 m_pMonster;
    std::unordered_map<uint32_t, CAISkillData*> m_setSkill;
};
#endif /* AISKILL_H */
