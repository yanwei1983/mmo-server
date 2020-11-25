#ifndef AIFUZZYLOGIC_H
#define AIFUZZYLOGIC_H

#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_SkillFAM.pb.h"
#include "config/Cfg_TargetFAM.pb.h"
#include "fam/rule_set.h"
#include "fam/trapezoid.h"
// Antecedent sets: enemy distance
enum distance_fuzzy_type
{
    disft_close,
    disft_medium,
    disft_far,
};
const fuzzy::Trapezoid distance_fuzzy[] = {fuzzy::Trapezoid{-15, -2.5, 2.5, 15}, fuzzy::Trapezoid{2.5, 15, 15, 30}, fuzzy::Trapezoid{15, 30, 50, 65}};

enum mp_fuzzy_type
{
    mpft_low,
    mpft_ok,
    mpft_lots,
};
const fuzzy::Trapezoid mp_fuzzy[] = {fuzzy::Trapezoid(0, 0, 0.10, 0.20), fuzzy::Trapezoid(0, 0.20, 0.40, 0.60), fuzzy::Trapezoid(0.20, 0.60, 1, 1)};

enum hp_fuzzy_type
{
    hpft_low,
    hpft_ok,
    hpft_lots,
};
const fuzzy::Trapezoid hp_fuzzy[] = {fuzzy::Trapezoid(0, 0, 0.10, 0.20), fuzzy::Trapezoid(0, 0.20, 0.40, 0.60), fuzzy::Trapezoid(0.20, 0.60, 1, 1)};

enum skill_dis_fuzzy_type
{
    skilldis_ft_close,
    skilldis_ft_medium,
    skilldis_ft_far,
};
const fuzzy::Trapezoid skill_dis_fuzzy[] = {fuzzy::Trapezoid{-15, -2.5, 2.5, 15},
                                            fuzzy::Trapezoid{2.5, 15, 15, 30},
                                            fuzzy::Trapezoid{15, 30, 50, 65}};

enum skill_pow_fuzzy_type
{
    skillpow_ft_low,
    skillpow_f_normal,
    skillpow_ft_high,
};
const fuzzy::Trapezoid skill_pow_fuzzy[] = {fuzzy::Trapezoid{0, 0, 20, 50}, fuzzy::Trapezoid(30, 50, 70, 90), fuzzy::Trapezoid(50, 80, 100, 100)};

enum skill_usetime_fuzzy_type
{
    skillusetime_ft_low,
    skillusetime_f_normal,
    skillusetime_ft_high,
};
const fuzzy::Trapezoid skill_usetime_fuzzy[] = {fuzzy::Trapezoid{0, 0, 1, 2}, fuzzy::Trapezoid(3, 5, 7, 9), fuzzy::Trapezoid(5, 8, 100, 100)};

enum skill_cd_fuzzy_type
{
    skillcd_ft_low,
    skillcd_f_normal,
    skillcd_ft_high,
};
const fuzzy::Trapezoid skill_cd_fuzzy[] = {fuzzy::Trapezoid{0, 0, 1, 2}, fuzzy::Trapezoid(3, 5, 7, 9), fuzzy::Trapezoid(5, 8, 100, 100)};

enum skill_usemp_fuzzy_type
{
    skillusemp_ft_low,
    skillusemp_f_normal,
    skillusemp_ft_high,
};
const fuzzy::Trapezoid skill_usemp_fuzzy[] = {fuzzy::Trapezoid{0, 0, 1, 2}, fuzzy::Trapezoid(3, 5, 7, 9), fuzzy::Trapezoid(5, 8, 100, 100)};

enum like_fuzzy_type
{
    likeft_unlike,
    likeft_like,
    likeft_verylike,
};
const fuzzy::Trapezoid like_fuzzy[] = {fuzzy::Trapezoid(0, 0, 20, 50), fuzzy::Trapezoid(30, 50, 70, 90), fuzzy::Trapezoid(50, 80, 100, 100)};

class SkillFAM : public NoncopyableT<SkillFAM>
{

    SkillFAM()
        : m_rule_set("SkillFAM", fuzzy::Implication::MAMDANI)
    {
    }
    bool Init(const Cfg_SkillFAM& row)
    {
        m_ID = row.id();
        // Set up our rules.
        uint32_t dis_idx       = row.dis();
        uint32_t hp_idx        = row.self_hp();
        uint32_t mp_idx        = row.self_mp();
        uint32_t target_hp_idx = row.target_hp();
        uint32_t skill_dis     = row.skill_dis();
        uint32_t skill_pow     = row.skill_pow();
        uint32_t skill_usetime = row.skill_usetime();
        uint32_t skill_cd      = row.skill_cd();
        uint32_t skill_mp      = row.skill_mp();
        uint32_t like_idx      = row.like();

        fuzzy::Conjunction oper_type = (fuzzy::Conjunction)(row.oper_type());

        m_rule_set.add(fuzzy::Rule{std::vector<fuzzy::Trapezoid>{distance_fuzzy[dis_idx],
                                                                 hp_fuzzy[hp_idx],
                                                                 mp_fuzzy[mp_idx],
                                                                 hp_fuzzy[target_hp_idx],
                                                                 skill_dis_fuzzy[skill_dis],
                                                                 skill_pow_fuzzy[skill_pow],
                                                                 skill_usetime_fuzzy[skill_usetime],
                                                                 skill_cd_fuzzy[skill_cd],
                                                                 skill_usemp_fuzzy[skill_mp]},
                                   oper_type,
                                   like_fuzzy[like_idx]});
        return true;
    }

public:
    CreateNewImpl(SkillFAM);

public:
    ~SkillFAM() {}

    using PB_T = Cfg_SkillFAM;
    void Merge(SkillFAM* pData) { pData->m_rule_set.merge(m_rule_set); }

    static uint32_t GetKey(const Cfg_SkillFAM& row) { return row.id(); }
    uint32_t        GetID() { return m_ID; }

    template<class... Args>
    double calculate(Args&&... args) const
    {
        return m_rule_set.calculate(std::vector<double>{std::forward<Args>(args)...});
    }

private:
    uint32_t m_ID;

    // The rules that will tie the logic together
    fuzzy::RuleSet m_rule_set;
};
DEFINE_GAMEMAPDATA(CSkillFAMSet, SkillFAM);

class TargetFAM : public NoncopyableT<TargetFAM>
{

    TargetFAM()
        : m_rule_set("TargetFAM", fuzzy::Implication::MAMDANI)
    {
    }
    bool Init(const Cfg_TargetFAM& row)
    {
        m_ID = row.id();
        // Set up our rules.
        uint32_t dis_idx       = row.dis();
        uint32_t hp_idx        = row.hp();
        uint32_t target_hp_idx = row.target_hp();
        uint32_t like_idx      = row.like();

        fuzzy::Conjunction oper_type = (fuzzy::Conjunction)(row.oper_type());
        m_rule_set.add(fuzzy::Rule{std::vector<fuzzy::Trapezoid>{distance_fuzzy[dis_idx], hp_fuzzy[hp_idx], hp_fuzzy[target_hp_idx]},
                                   oper_type,
                                   like_fuzzy[like_idx]});
        return true;
    }

public:
    CreateNewImpl(TargetFAM);

public:
    ~TargetFAM() {}

    using PB_T = Cfg_TargetFAM;
    void Merge(TargetFAM* pData) const { pData->m_rule_set.merge(m_rule_set); }

    static uint32_t GetKey(const Cfg_TargetFAM& row) { return row.id(); }
    uint32_t        GetID() { return m_ID; }

    template<class... Args>
    double calculate(Args&&... args) const
    {
        return m_rule_set.calculate(std::vector<double>{std::forward<Args>(args)...});
    }

private:
    uint32_t m_ID;
    // The rules that will tie the logic together
    fuzzy::RuleSet m_rule_set;
};
DEFINE_GAMEMAPDATA(CTargetFAMSet, TargetFAM);

#endif /* AIFUZZYLOGIC_H */
