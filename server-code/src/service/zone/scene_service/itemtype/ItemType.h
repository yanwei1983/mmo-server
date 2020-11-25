#ifndef ITEMTYPE_H
#define ITEMTYPE_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Item.pb.h"

export_lua constexpr uint32_t MAX_ITEM_EXTRDATA_NUM = 10;

export_lua constexpr uint32_t ITEM_MAINTYPE_MASK = 10000000;
export_lua constexpr uint32_t ITEM_SUBTYPE_MASK  = 00100000;

export_lua constexpr uint32_t DEFAULT_BAG_SIZE    = 50;
export_lua constexpr uint32_t DEFAULT_STROGE_SIZE = 50;

export_lua enum ITEM_MAINTYPE {
    ITEM_MAINTYPE_ITEM  = 1,
    ITEM_MAINTYPE_EQUIP = 2,
};

export_lua enum ITEM_SUBTYPE_ITEM {
    ITEM_SUBTYPE_ITEM_MEDICINE         = 1,
    ITEM_SUBTYPE_ITEM_TASK             = 2,
    ITEM_SUBTYPE_ITEM_HORSE            = 4,
    ITEM_SUBTYPE_ITEM_OTHER            = 9,
    ITEM_SUBTYPE_ITEM_ADDITION         = 10,
    ITEM_SUBTYPE_ITEM_ADDITION_PROTECT = 11,
    ITEM_SUBTYPE_ITEM_ADDITION_LUCKY   = 12,
};

export_lua enum ITEM_SUBTYPE_EQUIP {
    ITEM_SUBTYPE_EQUIP_WEAPON         = 1,
    ITEM_SUBTYPE_EQUIP_FASHION_WEAPON = 2,

    ITEM_SUBTYPE_EQUIP_HELMET        = 10,
    ITEM_SUBTYPE_EQUIP_ARMOR         = 11,
    ITEM_SUBTYPE_EQUIP_GIRDLE        = 12,
    ITEM_SUBTYPE_EQUIP_SHOES         = 13,
    ITEM_SUBTYPE_EQUIP_HAND          = 14,
    ITEM_SUBTYPE_EQUIP_FASHION_DRESS = 15,

    ITEM_SUBTYPE_EQUIP_NECKLACE = 20,
    ITEM_SUBTYPE_EQUIP_RING     = 21,
    ITEM_SUBTYPE_EQUIP_WING     = 23,
    ITEM_SUBTYPE_EQUIP_HORSE    = 24,

};

export_lua enum ITEMPOSITION {
    ITEMPOSITION_BAG   = 0, //背包
    ITEMPOSITION_EQUIP = 1, //装备槽

    ITEMPOSITION_EXCHANGE = 99,  //交易
    ITEMPOSITION_STORAGE  = 100, //仓库

    ITEMPOSITION_GUILDSTORAGE = 200, //帮派仓库
    ITEMPOSITION_BUYBACK      = 201, //回购列表
    ITEMPOSITION_MAILL        = 202, //邮箱
    ITEMPOSITION_MARKET       = 203, //拍卖行'
};

export_lua enum EQUIPPOSITION {
    EQUIPPOSITION_NONE           = 0,
    EQUIPPOSITION_WEAPON         = 1, //武器
    EQUIPPOSITION_FASHION_WEAPON = 2, //时装武器

    EQUIPPOSITION_HELMET   = 10, //头
    EQUIPPOSITION_ARMOR    = 11, //身
    EQUIPPOSITION_GIRDLE   = 12, //腰带
    EQUIPPOSITION_SHOES    = 13, //鞋
    EQUIPPOSITION_HAND_L   = 14, //左手腕
    EQUIPPOSITION_HAND_R   = 15, //右手腕
    EQUIPPOSITION_RING_L   = 16, //左戒指
    EQUIPPOSITION_RING_R   = 17, //右戒指
    EQUIPPOSITION_NECKLACE = 18, //项链

    EQUIPPOSITION_FASHION_DRESS = 19, //时装衣服

    EQUIPPOSITION_WING = 23, //翅膀
    EQUIPPOSITION_RUNE = 24, //符石
};

export_lua enum ITEMFLAG_MASK {
    ITEMFLAG_EXCHANGE_DISABLE = 0x0001, // 是否禁止交易
    ITEMFLAG_STORAGE_DISABLE  = 0x0002, // 是否禁止存入仓库
    ITEMFLAG_SELL_DISABLE     = 0x0004, // 是否禁止出售
    ITEMFLAG_DROP_DISABLE     = 0x0008, // 是否禁止丢弃
    ITEMFLAG_REPAIR_DISABLE   = 0x0010, // 是否禁止修理
    ITEMFLAG_DEL_DISABLE      = 0x0020, // 是否禁止销毁
    ITEMFLAG_FORGING_DISABLE = 0x00400, // 不可进行武器装备锻造的所有操作（升级品质，熔炼，打孔，镶嵌宝石，拆除宝石）
    ITEMFLAG_EQUIPEED_DISABLE_EXCHANGE = 0x0080, // 装备后禁止交易

    ITEMFLAG_TRACE_ITEM     = 0x0100, // 是否任务追踪物品（任务物品或者带这个掩码的物品都是任务追踪物品）
    ITEMFLAG_DELITEM_EXPIRE = 0x0200, // 带此掩码的物品，当时间过期后，将自动删除
    ITEMFLAG_SPEND_DURA = 0x0400, // 使用物品消耗耐久度——实现类似叠加物品消耗数量的功能，不能与可叠加物品共存
    ITEMFLAG_BATCH_USE_CHECK = 0x0800, // 批量使用时是否需要检查

    ITEMFLAG_PICK_RUMOR     = 0x1000, // 带此掩码的物品在：配方合成、拾取怪物掉落 的时候会广播传闻
    ITEMFLAG_DROP_RUMOR     = 0x2000, // 广播掉落物品传闻
    ITEMFLAG_BUY_RUMOR      = 0x4000, // 带此掩码的物品在 商城购买、帮派商店购买 的时候会广播传闻
    ITEMFLAG_GAMBLING_RUMOR = 0x8000, // 带此掩码的物品在 赌博产出 的时候会广播传闻

};

class CItemType : public NoncopyableT<CItemType>
{
    CItemType() {}
    bool Init(const Cfg_Item& row)
    {
        m_Data = row;
        for(int32_t i = 0; i < m_Data.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{m_Data.attrib_change_list(i)});
        }
        return true;
    }

public:
    CreateNewImpl(CItemType);

public:
    ~CItemType() {}
    using PB_T = Cfg_Item;

    static uint32_t GetKey(const Cfg_Item& row) { return row.id(); }

    uint32_t                               GetID() const { return m_Data.id(); }
    uint32_t                               GetSort() const { return m_Data.sort(); }
    const std::string&                     GetName() const { return m_Data.name(); }
    uint32_t                               GetQuility() const { return m_Data.quility(); }
    uint32_t                               GetExpireTime() const { return m_Data.expire_time(); }
    uint32_t                               GetDura() const { return m_Data.dura(); }
    uint32_t                               GetDuraLimit() const { return m_Data.dura_limit(); }
    uint32_t                               GetPileLimit() const { return m_Data.pile_limit(); }
    uint32_t                               GetAdditionLimit() const { return m_Data.addition_limit(); }
    uint32_t                               GetFlag() const { return m_Data.flag(); }
    bool                                   HasFlag(uint32_t flag) const { return ::HasFlag(GetFlag(), flag); }
    uint32_t                               GetLevReq() const { return m_Data.lev_req(); }
    uint32_t                               GetProfReq() const { return m_Data.prof_req(); }
    uint32_t                               GetValue() const { return m_Data.value(); }
    uint32_t                               GetCDType() const { return m_Data.cd_type(); }
    uint32_t                               GetCDMsces() const { return m_Data.cd_msces(); }
    uint32_t                               GetSuitType() const { return m_Data.suit_id() / 100; }
    uint32_t                               GetSuitIdx() const { return m_Data.suit_id() % 100; }
    const std::vector<CActorAttribChange>& GetAttrib() const { return m_AttribChangeList; }
    uint64_t                               GetScriptID() const { return m_Data.scriptid(); }
    uint32_t                               GetBatchUseCount() const { return m_Data.batchuse_count(); }

public:
    bool IsPileEnable() const { return GetPileLimit() > 1; }
    // 是否可交易
    bool IsExchangeEnable() const { return HasFlag(ITEMFLAG_EXCHANGE_DISABLE) == false; }
    // 是否可存仓库
    bool IsStorageEnable() const { return HasFlag(ITEMFLAG_STORAGE_DISABLE) == false; }
    // 是否可出售
    bool IsSellEnable() const { return HasFlag(ITEMFLAG_SELL_DISABLE) == false; }
    // 是否可丢弃
    bool IsDropEnable() const { return HasFlag(ITEMFLAG_DROP_DISABLE) == false; }
    // 是否客户端不可销毁
    bool IsDelEnable() const { return HasFlag(ITEMFLAG_DEL_DISABLE) == false; }
    // 是否可升级品质
    bool IsForgingEnable() const { return HasFlag(ITEMFLAG_FORGING_DISABLE) == false; }
    // 是否可修理
    bool IsRepairEnable() const { return (IsEquipment() && (HasFlag(ITEMFLAG_REPAIR_DISABLE)) == false); }
    // 配方合成、拾取怪物掉落的时候是否广播传闻
    bool IsPickRumor() const { return HasFlag(ITEMFLAG_PICK_RUMOR); }
    // 商城购买、帮派商店购买 的时候是否广播传闻
    bool IsBuyRumor() const { return HasFlag(ITEMFLAG_BUY_RUMOR); }
    // 赌博产出 的时候是否广播传闻
    bool IsGamblingRumor() const { return HasFlag(ITEMFLAG_GAMBLING_RUMOR); }
    // 是否任务追踪物品
    bool        IsTraceItem() const { return HasFlag(ITEMFLAG_TRACE_ITEM); }
    bool        IsEquipment() const { return IsEquipment(GetID()); }
    static bool IsEquipment(uint32_t idType) { return (idType / ITEM_MAINTYPE_MASK) == ITEM_MAINTYPE_EQUIP; }
    bool        IsSuit() const { return m_Data.suit_id() != 0; }

public:
    const Cfg_Item& GetDataRef() const { return m_Data; }

private:
    Cfg_Item                        m_Data;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

DEFINE_GAMEMAPDATA(CItemTypeSet, CItemType);

#endif /* ITEMTYPE_H */
