#include "Equipment.h"

#include "ActorAttrib.h"
#include "Item.h"
#include "ItemAddition.h"
#include "ItemType.h"
#include "Package.h"
#include "Player.h"
#include "PlayerAchievement.h"
#include "PlayerTask.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "SuitEquip.h"
#include "config/Cfg_Achievement.pb.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"
CEquipment::CEquipment() {}

CEquipment::~CEquipment()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setItem)
    {
        SAFE_DELETE(v);
    }
    m_setItem.clear();
    __LEAVE_FUNCTION
}

bool CEquipment::Init(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    CHECKF(pPlayer)
    m_pOwner = pPlayer;
    auto pDB = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECKF(pDB);
    auto pResult = pDB->QueryMultiCond<TBLD_ITEM>(DBCond<TBLD_ITEM, TBLD_ITEM::OWNER_ID, OBJID>{pPlayer->GetID()},
                                                  DBCond<TBLD_ITEM, TBLD_ITEM::POSITION, uint32_t>{ITEMPOSITION_EQUIP});

    if(pResult)
    {
        for(size_t i = 0; i < pResult->get_num_row(); i++)
        {
            auto   row   = pResult->fetch_row(true);
            CItem* pItem = CItem::CreateNew(std::move(row));
            if(pItem)
            {
                auto it_find = m_setItem.find(pItem->GetGrid());
                if(it_find != m_setItem.end())
                {
                    // logerror
                    LOGERROR("PlayerID:{} Pakcage:{} PackageIdx:{} Have SameItem!!!!!", m_pOwner->GetID(), ITEMPOSITION_EQUIP, pItem->GetGrid());
                    SAFE_DELETE(pItem);
                    continue;
                }
                else
                {
                    m_setItem[pItem->GetGrid()] = pItem;
                    OnItemEquiped(pItem, false);
                    if(pItem->ItemTypePtr()->GetExpireTime() > 0 && pItem->IsExpire() == false)
                    {
                        AddItemExpireCallBack(pItem->GetGrid(), pItem->GetExpireTime());
                    }
                }
            }
            else
            {
                // logerror
                LOGERROR("CreateItem Fail, PlayerID:{} Pakcage:{} idx:{}", m_pOwner->GetID(), ITEMPOSITION_EQUIP, i);
            }
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

CItem* CEquipment::QueryEquipment(uint32_t nGrid) const
{
    __ENTER_FUNCTION
    auto find = m_setItem.find(nGrid);
    if(find != m_setItem.end())
        return find->second;
    __LEAVE_FUNCTION
    return nullptr;
}

CItem* CEquipment::QueryEquipmentById(OBJID idItem) const
{
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetID() == idItem)
            return pItem;
    }
    __LEAVE_FUNCTION
    return nullptr;
}

bool CEquipment::EquipItem(uint32_t nGridInPackage, uint32_t nGrid, bool bSync /*=true*/)
{
    __ENTER_FUNCTION
    // 先从背包中取出，这样就不会导致背包满了//不要更新任务追踪
    CItem* pItem = m_pOwner->GetBag()->QueryItemByGrid(nGridInPackage);
    CHECKF(pItem);
    CHECKF(pItem->IsEquipment());
    if(CanEquip(pItem, nGrid) == false)
        return false;
    m_pOwner->GetBag()->PopItemByGrid(nGridInPackage, SYNC_TRUE);

    // 卸下旧装备
    CItem* pOldItem = UnequipItem(nGrid, false, false, SYNC_TRUE); // 更换装备时卸下装备不要计算战斗力，等穿上后再计算

    if(pItem && pItem->HasFlag(ITEMFLAG_EQUIPEED_DISABLE_EXCHANGE))
    {
        //清除装备后绑定标记
        //设置禁止交易标志
        uint32_t dwFlag = pItem->_GetFlag();
        dwFlag |= ITEMFLAG_EXCHANGE_DISABLE;
        pItem->SetFlag(dwFlag, UPDATE_FALSE);
    }

    // 装上新装备
    SetEquipment(nGrid, pItem);
    OnItemEquiped(pItem, false);

    if(bSync)
    {
        pItem->SendItemInfo(m_pOwner);
    }

    // notify other client
    if(bSync)
    {
        if(nGrid == EQUIPPOSITION_ARMOR || nGrid == EQUIPPOSITION_FASHION_DRESS)
            m_pOwner->AddDelayAttribChange(PROP_ARMOR_CHANGE, GetArmorTypeID());
        else if(nGrid == EQUIPPOSITION_WEAPON || nGrid == EQUIPPOSITION_FASHION_WEAPON)
            m_pOwner->AddDelayAttribChange(PROP_WEAPON_CHANGE, GetWeaponTypeID());
    }

    m_pOwner->RecalcAttrib();
    return true;
    __LEAVE_FUNCTION
    return false;
}

CItem* CEquipment::UnequipItem(uint32_t nGrid, bool bSync /*=true*/, bool bRecalcAbility /*=true*/, bool bRemoveItemExpire /*= true*/)
{
    __ENTER_FUNCTION
    auto it = m_setItem.find(nGrid);
    if(it == m_setItem.end())
        return nullptr;
    CItem* pItem = it->second;
    if(pItem == nullptr)
        return nullptr;

    if(m_pOwner->GetBag()->IsFull(pItem) == true)
        return nullptr;

    m_setItem.erase(it);
    OnItemUnEquiped(pItem, false);
    if(bRemoveItemExpire && pItem->ItemTypePtr()->GetExpireTime() > 0)
    {
        RemoveItemExpireCallBack(nGrid, pItem->GetExpireTime());
    }

    pItem->SetGrid(0, UPDATE_FALSE);
    pItem->SetPosition(ITEMPOSITION_BAG, UPDATE_TRUE);
    if(bSync)
    {
        pItem->SendDeleteMsg(m_pOwner);
    }

    //不要更新任务追踪
    m_pOwner->GetBag()->AddItem(pItem, SYNC_TRUE, true, false);
    if(bSync)
    {
        if(nGrid == EQUIPPOSITION_ARMOR || nGrid == EQUIPPOSITION_FASHION_DRESS)
            m_pOwner->AddDelayAttribChange(PROP_ARMOR_CHANGE, GetArmorTypeID());
        else if(nGrid == EQUIPPOSITION_WEAPON || nGrid == EQUIPPOSITION_FASHION_WEAPON)
            m_pOwner->AddDelayAttribChange(PROP_WEAPON_CHANGE, GetWeaponTypeID());
    }

    if(bRecalcAbility)
        m_pOwner->RecalcAttrib();

    return pItem;
    __LEAVE_FUNCTION
    return nullptr;
}

uint32_t CEquipment::GetWeaponTypeID() const
{
    __ENTER_FUNCTION
    CItem* pWeapon = QueryEquipment(EQUIPPOSITION_FASHION_WEAPON);
    if(pWeapon == nullptr)
        pWeapon = QueryEquipment(EQUIPPOSITION_WEAPON);

    if(pWeapon)
        return pWeapon->GetType();

    __LEAVE_FUNCTION
    return 0;
}

uint32_t CEquipment::GetArmorTypeID() const
{
    __ENTER_FUNCTION
    CItem* pArmor = QueryEquipment(EQUIPPOSITION_FASHION_DRESS);
    if(pArmor == nullptr)
        pArmor = QueryEquipment(EQUIPPOSITION_ARMOR);

    if(pArmor)
        return pArmor->GetType();

    __LEAVE_FUNCTION
    return 0;
}

void CEquipment::SaveAll()
{
    __ENTER_FUNCTION
    for(auto& [k, pItem]: m_setItem)
    {
        if(pItem)
            pItem->SaveInfo();
    }
    __LEAVE_FUNCTION
}

void CEquipment::SendInfo()
{
    __ENTER_FUNCTION
    for(auto& [k, pItem]: m_setItem)
    {
        if(pItem)
            pItem->SendItemInfo(m_pOwner);
    }
    __LEAVE_FUNCTION
}

bool CEquipment::DecEquipmentDurability(bool bBeAttack, bool bMagic, int32_t nDecValue /*=1*/)
{
    return true;
}

void CEquipment::AddEquipmentDurability(uint32_t nPosition, int32_t nAddValue) {}

void CEquipment::DeleteAll(bool bSync /*=true*/, bool bTraceTaskItem)
{
    __ENTER_FUNCTION

    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem)
        {
            // 更新任务追踪物品数量
            if(bTraceTaskItem && pItem->IsTraceItem())
            {
                m_pOwner->GetTaskSet()->OnDelTaskItem(pItem->GetType(), pItem->GetPileNum());
            }

            if(bSync)
            {
                SC_ITEM_DELETE msg;
                msg.set_item_id(pItem->GetID());
                msg.set_position(pItem->GetPosition());
                msg.set_grid(pItem->GetGrid());
                m_pOwner->SendMsg(msg);
            }

            pItem->DelRecord();
            SAFE_DELETE(pItem);
            it->second = nullptr;
        }
    }
    m_setExpireItem.clear();
    m_setItem.clear();
    __LEAVE_FUNCTION
}

bool CEquipment::CanEquipByItemType(const CItemType* pItemType) const
{
    CHECKF(pItemType);
    __ENTER_FUNCTION
    if(pItemType->GetProfReq() != 0 && HasFlag(pItemType->GetProfReq(), m_pOwner->GetProf()) == true)
    {
        // 职业要求不符
        return false;
    }

    if(pItemType->GetLevReq() != 0 && pItemType->GetLevReq() > m_pOwner->GetLev())
    {
        // 等级要求不符
        return false;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CEquipment::CanEquip(CItem* pItem, uint32_t& nGrid) const
{
    __ENTER_FUNCTION
    CHECKF(pItem);
    if(CanEquipByItemType(pItem->ItemTypePtr()) == false)
        return false;

    if(nGrid != 0)
    {
        if(!CheckEquipPosition(pItem, nGrid))
            return false;
    }
    else
        nGrid = pItem->GetEquipPosition();

    if(nGrid == EQUIPPOSITION_NONE) // 不可装备
    {
        return false;
    }

    //如果耐久不够不允许装备
    if(pItem->GetDura() == 0 && pItem->ItemTypePtr()->GetDuraLimit() != 0)
    {
        return false;
    }

    if(pItem->IsExpire() == true)
    {
        return false;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CEquipment::AddSuitNum(const CItemType* pItemType)
{
    CHECK(pItemType);
    CHECK(pItemType->GetSuitType() != 0);
    __ENTER_FUNCTION

    uint32_t& nCount   = m_setSuitNum[pItemType->GetSuitType()];
    auto      old_data = SuitEquipSet()->QuerySuitEquip(pItemType->GetSuitType(), nCount);
    if(old_data)
    {
        m_pOwner->GetAttrib().Remove(old_data->GetAttribChangeList());
    }

    nCount++;

    auto new_data = SuitEquipSet()->QuerySuitEquip(pItemType->GetSuitType(), nCount);
    if(new_data)
    {
        m_pOwner->GetAttrib().Store(new_data->GetAttribChangeList());
    }

    m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_EQUIPMENT_SUIT, pItemType->GetSuitType(), nCount);

    __LEAVE_FUNCTION
}

void CEquipment::DecSuitNum(const CItemType* pItemType)
{
    CHECK(pItemType);
    CHECK(pItemType->GetSuitType() != 0);
    __ENTER_FUNCTION
    auto it = m_setSuitNum.find(pItemType->GetSuitType());
    if(it == m_setSuitNum.end())
        return;
    uint32_t& nCount   = it->second;
    auto      old_data = SuitEquipSet()->QuerySuitEquip(pItemType->GetSuitType(), nCount);
    if(old_data)
    {
        m_pOwner->GetAttrib().Remove(old_data->GetAttribChangeList());
    }

    --nCount;
    if(nCount == 0)
    {
        m_setSuitNum.erase(it);
    }

    auto new_data = SuitEquipSet()->QuerySuitEquip(pItemType->GetSuitType(), nCount);
    if(new_data)
    {
        m_pOwner->GetAttrib().Store(new_data->GetAttribChangeList());
    }

    __LEAVE_FUNCTION
}

void CEquipment::CheckItemExpire(uint32_t dwTimeNow)
{
    __ENTER_FUNCTION
    if(m_setExpireItem.empty())
        return;

    bool bRecalcAbility = false;
    auto itEnd          = m_setExpireItem.upper_bound(dwTimeNow);
    if(itEnd == m_setExpireItem.begin())
        return;

    for(auto it = m_setExpireItem.begin(); it != itEnd;)
    {
        uint32_t nGrid = it->second;
        CItem*   pItem = QueryEquipment(nGrid);
        if(pItem)
        {
            if(m_pOwner->GetBag()->IsFull() == false)
            {
                UnequipItem(nGrid, SYNC_TRUE, false, false);
                ScriptManager()->TryExecScript<void>(SCRIPT_ITEM, pItem->ItemTypePtr()->GetScriptID(), "OnTimeOut", pItem, m_pOwner);
            }
            else
            {
            }
            bRecalcAbility = true;
        }

        m_setExpireItem.erase(it++);
    }
    if(bRecalcAbility)
    {
        m_pOwner->RecalcAttrib();
        m_pOwner->BroadcastShow();
    }
    __LEAVE_FUNCTION
}

void CEquipment::RemoveItemExpireCallBack(uint32_t nGrid, uint32_t dwExpireData)
{
    __ENTER_FUNCTION
    auto it_range = m_setExpireItem.equal_range(dwExpireData);
    for(auto it = it_range.first; it != it_range.second;)
    {
        if(it->second == nGrid)
        {
            m_setExpireItem.erase(it++);
        }
        else
        {
            it++;
        }
    }
    __LEAVE_FUNCTION
}

void CEquipment::AddItemExpireCallBack(uint32_t nGrid, uint32_t dwExpireData)
{
    __ENTER_FUNCTION
    m_setExpireItem.insert(std::make_pair(dwExpireData, nGrid));
    __LEAVE_FUNCTION
}

void CEquipment::OnItemEquiped(CItem* pItem, bool bRepair)
{
    __ENTER_FUNCTION
    CHECK(pItem);
    m_pOwner->GetAttrib().Store(pItem->ItemTypePtr()->GetAttrib());
    if(pItem->AdditionType())
    {
        m_pOwner->GetAttrib().Store(pItem->AdditionType()->GetAttrib());
    }
    // 重新计算套装数量
    if((pItem->GetDura() != 0 || pItem->ItemTypePtr()->GetDuraLimit() == 0) && pItem->IsSuit())
    {
        AddSuitNum(pItem->ItemTypePtr());
    }

    m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_EQUIPMENT, pItem->GetType(), pItem->GetGrid());
    if(pItem->ItemTypePtr()->GetQuility() > 0)
    {
        m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_EQUIPMENT_QUILITY, pItem->ItemTypePtr()->GetQuility(), pItem->GetGrid());
    }
    if(pItem->GetAddition() > 0)
    {
        m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_EQUIPMENT_ADDITION, pItem->GetAddition(), pItem->GetGrid());
    }
    __LEAVE_FUNCTION
}

void CEquipment::OnItemUnEquiped(CItem* pItem, bool bBroked)
{
    __ENTER_FUNCTION
    CHECK(pItem);
    m_pOwner->GetAttrib().Remove(pItem->ItemTypePtr()->GetAttrib());
    if(pItem->AdditionType())
    {
        m_pOwner->GetAttrib().Remove(pItem->AdditionType()->GetAttrib());
    }
    // 重新计算套装数量
    if((pItem->GetDura() != 0 || pItem->ItemTypePtr()->GetDuraLimit() == 0) && pItem->IsSuit())
    {
        DecSuitNum(pItem->ItemTypePtr());
    }
    __LEAVE_FUNCTION
}

bool CEquipment::CheckEquipPosition(CItem* pItem, uint32_t nGrid) const
{
    __ENTER_FUNCTION
    CHECKF(pItem);
    uint32_t nPosition = pItem->GetEquipPosition();
    //额外检查左右戒指/手套,GetEquipPosition默认返回左手位
    if(nPosition == nGrid || (nPosition == EQUIPPOSITION_RING_L && nPosition == EQUIPPOSITION_RING_R) ||
       (nPosition == EQUIPPOSITION_HAND_L && nPosition == EQUIPPOSITION_HAND_R))
    {
        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CEquipment::SetEquipment(uint32_t nGrid, CItem* pItem)
{
    __ENTER_FUNCTION
    m_setItem[nGrid] = pItem;
    if(pItem)
    {
        pItem->SetGrid(nGrid, UPDATE_FALSE);
        pItem->SetPosition(ITEMPOSITION_EQUIP, UPDATE_TRUE);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CEquipment::NotifyEquip(uint16_t usAction, CItem* pItem, uint32_t nGrid)
{
    //通知周围的人, 装备变化
}
