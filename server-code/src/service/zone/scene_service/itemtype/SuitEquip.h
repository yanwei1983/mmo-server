#ifndef SUITEQUIP_H
#define SUITEQUIP_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Suit.pb.h"

class CSuitEquipData : public NoncopyableT<CSuitEquipData>
{
    CSuitEquipData() {}
    bool Init(const Cfg_Suit& row)
    {
        m_id        = row.id();
        m_nEquipNum = row.num();
        for(int32_t i = 0; i < row.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{row.attrib_change_list(i)});
        }
        return true;
    }

public:
    CreateNewImpl(CSuitEquipData);

public:
    ~CSuitEquipData() {}
    using PB_T = Cfg_Suit;

    uint32_t GetEquipNum() const { return m_nEquipNum; }
    uint32_t GetID() const { return m_id; }

public:
    const std::vector<CActorAttribChange>& GetAttribChangeList() const { return m_AttribChangeList; }

private:
    uint32_t                        m_id;
    uint32_t                        m_nEquipNum;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

//////////////////////////////////////////////////////////////////////
class CSuitEquipSet : public CGameDataContainer<CSuitEquipData>
{
    CSuitEquipSet();

public:
    CreateNewImpl(CSuitEquipSet);

public:
    virtual ~CSuitEquipSet();

public:
    virtual void Clear() override;
    virtual void BuildIndex() override;
    // 根据itemtype, 追加等级，查询对应的追加数据
    const CSuitEquipData* QuerySuitEquip(uint32_t idSuit, uint32_t nEquipNum);

public:
    typedef std::vector<CSuitEquipData*>            LEVEL_SET;
    typedef std::unordered_map<uint32_t, LEVEL_SET> TYPE_SET;

protected:
    TYPE_SET m_setSuitType;
};
#endif /* SUITEQUIP_H */
