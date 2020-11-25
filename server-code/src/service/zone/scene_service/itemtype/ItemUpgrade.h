#ifndef ITEMUPGRADE_H
#define ITEMUPGRADE_H

#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_ItemUpgrade.pb.h"

//////////////////////////////////////////////////////////////////////
class CItemUpgradeData : public NoncopyableT<CItemUpgradeData>
{
    CItemUpgradeData() {}
    bool Init(const Cfg_ItemUpgrade& row)
    {
        m_Data = row;
        return true;
    }

public:
    CreateNewImpl(CItemUpgradeData);

public:
    virtual ~CItemUpgradeData() {}
    using PB_T = Cfg_ItemUpgrade;

    static uint32_t GetKey(const Cfg_Item& row) { return row.id(); }

public:
    uint32_t       GetID() const { return m_Data.id(); }
    uint32_t       GetSort() const { return m_Data.sort(); }
    uint32_t       GetMainType() const { return m_Data.main_itemtype(); }
    uint32_t       GetLevReq() const { return m_Data.lev_req(); }
    uint32_t       GetSuccRate() const { return m_Data.succ_rate(); }
    uint32_t       GetRumor() const { return m_Data.rumor(); }
    decltype(auto) GetReqData() const { return m_Data.req_data_list(); } // 需要材料itemtypeid

public:
    Cfg_ItemUpgrade m_Data;
};

DEFINE_GAMEMAPDATA(CItemUpgradeDataSet, CItemUpgradeData);

#endif /* ITEMUPGRADE_H */
