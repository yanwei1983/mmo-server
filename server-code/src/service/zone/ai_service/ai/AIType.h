#ifndef AITYPE_H
#define AITYPE_H

#include "AIFuzzyLogic.h"
#include "AIService.h"
#include "config/Cfg_AIType.pb.h"

enum AIType
{
    AITYPE_NONE    = 0, //木桩
    AITYPE_PASSIVE = 1, //被动
    AITYPE_ACTIVE  = 2, //主动
};


class CAIType : public NoncopyableT<CAIType>
{
    CAIType() {}
    bool Init(const Cfg_AIType& row)
    {
        m_Data       = row;
        m_pTargetFAM = TargetFAMSet()->QueryObj(row.targetfam_id());
        m_pSkillFAM  = SkillFAMSet()->QueryObj(row.skillfam_id());
        return true;
    }

public:
    CreateNewImpl(CAIType);

public:
    ~CAIType() {}

    using PB_T = Cfg_AIType;
    uint32_t GetID() const { return m_Data.idmonster(); }

    const TargetFAM* GetTargetFAM() const { return m_pTargetFAM; }
    const SkillFAM*  GetSkillFAM() const { return m_pSkillFAM; }

public:
    const Cfg_AIType& GetDataRef() const { return m_Data; }

private:
    Cfg_AIType m_Data;

    const TargetFAM* m_pTargetFAM = nullptr;
    const SkillFAM*  m_pSkillFAM  = nullptr;
};

DEFINE_GAMEMAPDATA(CAITypeSet, CAIType);

#endif /* AITYPE_H */
