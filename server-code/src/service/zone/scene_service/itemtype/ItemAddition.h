#ifndef ITEMADDITION_H
#define ITEMADDITION_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_ItemAddition.pb.h"

class CItemAdditionData : public NoncopyableT<CItemAdditionData>
{
    CItemAdditionData() {}
    bool Init(const Cfg_ItemAddition& row)
    {
        m_id = row.id();
        for(int32_t i = 0; i < row.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{row.attrib_change_list(i)});
        }
        return true;
    }

public:
    CreateNewImpl(CItemAdditionData);

public:
    ~CItemAdditionData() {}
    using PB_T = Cfg_ItemAddition;

public:
    uint32_t                               GetID() const { return m_id; }
    const std::vector<CActorAttribChange>& GetAttrib() const { return m_AttribChangeList; }

private:
    uint32_t                        m_id;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

//////////////////////////////////////////////////////////////////////
class CItemAdditionSet : public CGameDataContainer<CItemAdditionData>
{
    CItemAdditionSet();

public:
    CreateNewImpl(CItemAdditionSet);

public:
    virtual ~CItemAdditionSet();

public:
    virtual void Clear() override;
    virtual void BuildIndex() override;

    // 根据itemtype, 追加等级，查询对应的追加数据
    const CItemAdditionData* QueryItemAddition(uint32_t idType, int32_t nLevel);

public:
    typedef std::vector<CItemAdditionData*>      LEVEL_SET;
    typedef std::unordered_map<OBJID, LEVEL_SET> TYPE_SET;

protected:
    TYPE_SET m_setAddition;
};

#endif /* ITEMADDITION_H */
