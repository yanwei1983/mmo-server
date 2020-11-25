#ifndef ITEMFORMULA_H
#define ITEMFORMULA_H

#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_ItemFormula.pb.h"

//////////////////////////////////////////////////////////////////////
class CItemFormulaData : public NoncopyableT<CItemFormulaData>
{
    CItemFormulaData() {}
    bool Init(const Cfg_ItemFormula& row)
    {
        m_Data = row;
        return true;
    }

public:
    CreateNewImpl(CItemFormulaData);

public:
    virtual ~CItemFormulaData() {}
    using PB_T = Cfg_ItemFormula;

    static uint32_t GetKey(const Cfg_ItemFormula& row) { return row.id(); }

public:
    uint32_t       GetID() const { return m_Data.id(); }
    uint32_t       GetSort() const { return m_Data.sort(); }
    uint32_t       GetTargetType() const { return m_Data.target_itemtype(); }
    uint32_t       GetTargetFlag() const { return m_Data.target_flag(); }
    uint32_t       GetTargetNum() const { return m_Data.target_itemnum(); }
    uint32_t       GetLevReq() const { return m_Data.lev_req(); }
    uint32_t       GetSuccRate() const { return m_Data.succ_rate(); }
    uint32_t       GetRumor() const { return m_Data.rumor(); }
    decltype(auto) GetReqData() const { return m_Data.req_data_list(); } // 需要材料itemtypeid

public:
    Cfg_ItemFormula m_Data;
};

DEFINE_GAMEMAPDATA(CItemFormulaDataSet, CItemFormulaData);

#endif /* ITEMFORMULA_H */
