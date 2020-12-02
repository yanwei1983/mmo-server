#include "Package.h"

#include "Item.h"
#include "ItemData.h"
#include "ItemType.h"
#include "Player.h"
#include "PlayerTask.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"

OBJECTHEAP_IMPLEMENTATION(CPackage, s_heap);

CPackage::CPackage() {}

CPackage::~CPackage()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setItem)
    {
        SAFE_DELETE(v);
    }
    m_setItem.clear();
    __LEAVE_FUNCTION
}

bool CPackage::Init(CPlayer* pOwner, uint32_t nPackageType, uint32_t nMaxSize)
{
    __ENTER_FUNCTION
    CHECKF(pOwner);
    m_pOwner    = pOwner;
    m_nPosition = nPackageType;
    m_nMaxSize  = nMaxSize;
    auto pDB    = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECKF(pDB);
    auto pResult = pDB->QueryMultiCond<TBLD_ITEM>(DBCond<TBLD_ITEM, TBLD_ITEM::OWNER_ID, OBJID>{pOwner->GetID()},
                                                  DBCond<TBLD_ITEM, TBLD_ITEM::POSITION, uint32_t>{m_nPosition});

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
                    LOGERROR("PlayerID:{} Pakcage:{} PackageIdx:{} Have SameItem!!!!!", m_pOwner->GetID(), m_nPosition, pItem->GetGrid());
                    SAFE_DELETE(pItem);
                    continue;
                }
                else
                {
                    m_setItem[pItem->GetGrid()] = pItem;
                }
            }
            else
            {
                // logerror
                LOGERROR("CreateItem Fail, PlayerID:{} Pakcage:{} idx:{}", m_pOwner->GetID(), m_nPosition, i);
            }
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPackage::SaveAll()
{
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem)
        {
            pItem->SaveInfo();
        }
    }
    __LEAVE_FUNCTION
}

void CPackage::SendAllItemInfo()
{
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem)
        {
            pItem->SendItemInfo(m_pOwner);
        }
    }
    __LEAVE_FUNCTION
}

CItem* CPackage::ForEach(const std::function<bool(CItem*)>& func)
{
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && func(pItem))
        {
            return pItem;
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}

bool CPackage::IsFull(CItem* pItem)
{
    __ENTER_FUNCTION
    CHECKF(pItem);
    return IsFull(pItem->GetType(), pItem->GetPileNum(), pItem->_GetFlag());
    __LEAVE_FUNCTION
    return false;
}

bool CPackage::IsFull(uint32_t idType, uint32_t nAmount, uint32_t dwFlag /*= 0*/)
{
    __ENTER_FUNCTION
    if(0 == nAmount)
    {
        const CItemType* pType = ItemTypeSet()->QueryObj(idType);
        if(!pType)
            return true; // 反正物品类型不存在，干脆直接返回背包满
        if(pType->IsPileEnable())
            nAmount = pType->GetPileLimit();
        else
            nAmount = 1;
    }
    return GetSpareSpace(idType, nAmount, 0, dwFlag) <= 0;
    __LEAVE_FUNCTION
    return false;
}

uint32_t CPackage::GetSpareSpace(uint32_t idType /*=ID_NONE*/, uint32_t nAmount /*=0*/, uint32_t nFillSpace /*=0*/, uint32_t dwFlag /*= 0*/)
{
    __ENTER_FUNCTION
    if(idType == ID_NONE)
        return m_nMaxSize - m_setItem.size();

    CHECKF_V(nAmount >= 0 && nFillSpace >= 0, nAmount);
    uint32_t nSpareSpace = m_nMaxSize - m_setItem.size() - nFillSpace;
    if(nSpareSpace < 0)
        return nSpareSpace;

    // 不可叠加物品
    const CItemType* pType = ItemTypeSet()->QueryObj(idType);
    CHECKF_V(pType, idType);
    if(pType->GetPileLimit() == 1)
        return nSpareSpace - nAmount;

    // 可叠加物品
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->IsCombineEnable(idType, dwFlag))
        {
            if(pItem->GetPileNum() + nAmount <= pType->GetPileLimit())
                return nSpareSpace;
            nAmount -= pType->GetPileLimit() - pItem->GetPileNum();
        }
    }
    uint32_t nNum = nAmount / pType->GetPileLimit();
    if(nAmount % pType->GetPileLimit() != 0)
        nNum += 1;
    return nSpareSpace - nNum;
    __LEAVE_FUNCTION
    return 0;
}

bool CPackage::AwardItem(uint32_t idItemType, uint32_t nAmount, uint32_t dwFlag /*= 0*/)
{
    CHECKF(idItemType != 0);
    CHECKF(nAmount != 0);

    if(IsFull(idItemType, nAmount, dwFlag) == true)
        return false;

    nAmount = CombineAddItem(idItemType, nAmount, dwFlag, SYNC_TRUE);
    if(nAmount == 0)
        return true;

    const CItemType* pItemType = ItemTypeSet()->QueryObj(idItemType);
    CHECKF(pItemType);
    uint32_t nNeedAdd = nAmount;
    if(pItemType->GetPileLimit() > 0)
    {
        while(nAmount > 0)
        {
            if(nAmount < pItemType->GetPileLimit())
            {
                CItem* pItem =
                    CItem::CreateNew(SceneService()->GetGameDB(m_pOwner->GetWorldID()), m_pOwner->GetID(), idItemType, nAmount, dwFlag, m_nPosition);
                AddItem(pItem, SYNC_TRUE, false, true);
                break;
            }
            CItem* pItem = CItem::CreateNew(SceneService()->GetGameDB(m_pOwner->GetWorldID()),
                                            m_pOwner->GetID(),
                                            idItemType,
                                            pItemType->GetPileLimit(),
                                            dwFlag,
                                            m_nPosition);
            AddItem(pItem, SYNC_TRUE, false, true);

            nAmount -= pItemType->GetPileLimit();
        }
    }

    return true;
}

CItem* CPackage::AddItem(CItem* pItem, bool bSync /*=false*/, bool bCheckFull, bool bTrackTaskItem)
{
    __ENTER_FUNCTION
    CHECKF(pItem);
    if(bCheckFull && IsFull(pItem))
        return NULL;

    uint32_t nFindGrid = FindFirstEmptyGrid();
    CHECKF(nFindGrid > 0);
    m_setItem[nFindGrid] = pItem;
    pItem->SetGrid(nFindGrid, UPDATE_FALSE);
    pItem->SetPosition(m_nPosition, UPDATE_TRUE);
    if(bSync)
    {
        pItem->SendItemInfo(m_pOwner);
    }
    // 更新任务追踪物品数量
    if(bTrackTaskItem && pItem->IsTraceItem() && m_pOwner)
    {
        m_pOwner->GetTaskSet()->OnAwardTaskItem(pItem->GetType(), pItem->GetPileNum());
    }

    if(pItem->GetExpireTime() != 0)
    {
        if(pItem->IsExpire() == false)
        {
            AddItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
        }
        else if(pItem->HasFlag(ITEMFLAG_DELITEM_EXPIRE))
        {
            // DelItem(pItem->GetID(), SYNCHRO_TRUE, UPDATE_TRUE, false);
            //等待下一次检查时删除，否则返回的pItem指针是野指针
            AddItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
        }
    }

    return pItem;
    __LEAVE_FUNCTION
    return NULL;
}

bool CPackage::DelItem(OBJID idItem, bool bSync /*=false*/, bool bTraceTaskItem)
{
    __ENTER_FUNCTION
    if(idItem == ID_NONE)
        return false;

    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetID() == idItem)
        {
            m_setItem.erase(it);

            // 更新任务追踪物品数量
            if(bTraceTaskItem && pItem->IsTraceItem() && m_pOwner)
            {
                m_pOwner->GetTaskSet()->OnDelTaskItem(pItem->GetType(), pItem->GetPileNum());
            }

            if(pItem->GetExpireTime() != 0)
            {
                RemoveItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
            }

            uint32_t nIdx = pItem->GetGrid();
            if(bSync)
            {
                pItem->SendDeleteMsg(m_pOwner);
            }
            pItem->DelRecord();
            SAFE_DELETE(pItem);
            return true;
        }
    }
    __LEAVE_FUNCTION
    return false;
}

bool CPackage::DelItemByGrid(uint32_t nGrid, bool bSync /*= false*/, bool bTraceTaskItem /*= true*/)
{
    __ENTER_FUNCTION
    auto it = m_setItem.find(nGrid);
    if(it == m_setItem.end())
        return false;

    CItem* pItem = it->second;
    if(pItem)
    {
        m_setItem.erase(it);

        // 更新任务追踪物品数量
        if(bTraceTaskItem && pItem->IsTraceItem() && m_pOwner)
        {
            m_pOwner->GetTaskSet()->OnDelTaskItem(pItem->GetType(), pItem->GetPileNum());
        }

        if(pItem->GetExpireTime() != 0)
        {
            RemoveItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
        }

        uint32_t nIdx = pItem->GetGrid();
        if(bSync)
        {
            pItem->SendDeleteMsg(m_pOwner);
        }
        pItem->DelRecord();
        SAFE_DELETE(pItem);
        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

CItem* CPackage::PopItem(OBJID idItem, bool bSync /*=false*/)
{
    __ENTER_FUNCTION
    if(idItem == ID_NONE)
        return nullptr;
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetID() == idItem)
        {
            m_setItem.erase(it);

            // 更新任务追踪物品数量
            if(pItem->IsTraceItem() && m_pOwner)
            {
                m_pOwner->GetTaskSet()->OnDelTaskItem(pItem->GetType(), pItem->GetPileNum());
            }

            if(pItem->GetExpireTime() != 0)
            {
                RemoveItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
            }
            uint32_t nIdx = pItem->GetGrid();
            if(bSync)
            {
                pItem->SendDeleteMsg(m_pOwner);
            }
            return pItem;
        }
    }
    __LEAVE_FUNCTION
    return NULL;
}

CItem* CPackage::PopItemByGrid(uint32_t nGrid, bool bSync /*=false*/)
{
    __ENTER_FUNCTION
    if(nGrid == ID_NONE)
        return nullptr;
    auto it_find = m_setItem.find(nGrid);
    if(it_find == m_setItem.end())
        return nullptr;

    CItem* pItem = it_find->second;
    if(pItem && pItem->GetGrid() == nGrid)
    {
        m_setItem.erase(it_find);

        // 更新任务追踪物品数量
        if(pItem->IsTraceItem() && m_pOwner)
        {
            m_pOwner->GetTaskSet()->OnDelTaskItem(pItem->GetType(), pItem->GetPileNum());
        }

        if(pItem->GetExpireTime() != 0)
        {
            RemoveItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
        }
        uint32_t nIdx = pItem->GetGrid();
        if(bSync)
        {
            pItem->SendDeleteMsg(m_pOwner);
        }
        return pItem;
    }

    __LEAVE_FUNCTION
    return NULL;
}

bool CPackage::DelAll(bool bSync /*=false*/, bool bTraceTaskItem /*= true*/)
{
    __ENTER_FUNCTION

    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem)
        {
            // 更新任务追踪物品数量
            if(bTraceTaskItem && pItem->IsTraceItem() && m_pOwner)
            {
                m_pOwner->GetTaskSet()->OnDelTaskItem(pItem->GetType(), pItem->GetPileNum());
            }

            if(bSync)
            {
                pItem->SendDeleteMsg(m_pOwner);
            }

            pItem->DelRecord();
            SAFE_DELETE(pItem);
            it->second = nullptr;
        }
    }
    m_setExpireItem.clear();
    m_setItem.clear();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPackage::SwapItem(uint32_t nGrid1, uint32_t nGrid2)
{
    __ENTER_FUNCTION
    CHECKF_V(nGrid1 > 0 && nGrid1 < m_nMaxSize, nGrid1);
    CHECKF_V(nGrid2 > 0 && nGrid2 < m_nMaxSize, nGrid2);
    CHECKF_V(nGrid1 != nGrid2, nGrid1);

    CItem* pItem1 = nullptr;
    CItem* pItem2 = nullptr;
    auto   it1    = m_setItem.find(nGrid1);
    if(it1 != m_setItem.end())
    {
        pItem1 = it1->second;
        m_setItem.erase(it1);
    }
    auto it2 = m_setItem.find(nGrid1);
    if(it2 != m_setItem.end())
    {
        pItem2 = it2->second;
        m_setItem.erase(it2);
    }

    if(pItem1)
    {
        m_setItem[nGrid2] = pItem1;
        pItem1->SetGrid(nGrid2, UPDATE_TRUE);
        pItem1->SyncGridData(m_pOwner);
    }

    if(pItem2)
    {
        m_setItem[nGrid1] = pItem2;
        pItem2->SetGrid(nGrid1, UPDATE_TRUE);
        pItem2->SyncGridData(m_pOwner);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPackage::SplitItem(uint32_t nGrid1, uint32_t nGrid2, uint32_t nSplitNum)
{
    __ENTER_FUNCTION
    CHECKF_V(nGrid1 > 0 && nGrid1 < m_nMaxSize, nGrid1);
    CHECKF_V(nGrid2 > 0 && nGrid2 < m_nMaxSize, nGrid2);
    CHECKF_V(nGrid1 != nGrid2, nGrid1);

    // 先判断还有没有空余的格子
    if(IsFull(1))
        return false;

    CItem* pItem1 = QueryItemByGrid(nGrid1);
    CHECKF(pItem1);
    // 如果不是可叠加物品，或叠加数量小等于拆分数量，失败
    if(pItem1->IsPileEnable() == false || pItem1->GetPileNum() <= nSplitNum)
        return false;

    //判断目标格是否有东西， 如果有东西，则寻找一个新的格子
    CItem* pItem2 = QueryItemByGrid(nGrid2);
    if(pItem2 != nullptr)
    {
        nGrid2 = FindFirstEmptyGrid();
        CHECKF(nGrid2 > 0);
    }

    ST_ITEMINFO info;
    info.idItem    = SceneService()->CreateUID();
    info.idType    = pItem1->GetType();
    info.idOwner   = pItem1->GetOwnerID();
    info.dwFlag    = pItem1->_GetFlag();
    info.nAddition = pItem1->GetAddition();
    info.nPosition = pItem1->GetPosition();
    info.nGrid     = nGrid2;
    info.nNum      = nSplitNum;
    auto pDB       = SceneService()->GetGameDB(m_pOwner->GetWorldID());
    CHECKF(pDB);
    CItem* pNewItem = CItem::CreateNew(pDB, info);
    CHECKF(pNewItem);
    if(!AddItem(pNewItem, SYNC_TRUE, false, false))
    {
        LOGERROR("Package AddItem failed.");
        // 如果加入失败，要删除新物品并且还原原物品数量
        pNewItem->DelRecord();
        SAFE_DELETE(pNewItem);
        return false;
    }
    // 加入成功，则原物品数量写入数据库，并同步数量
    pItem1->DecPileNum(nSplitNum, UPDATE_TRUE);
    pItem1->SyncPileNum(m_pOwner);

    // dlog

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPackage::CombineItem(uint32_t nGrid1, uint32_t nGrid2)
{
    __ENTER_FUNCTION
    CHECKF(nGrid1 < m_nMaxSize);
    CHECKF(nGrid2 < m_nMaxSize);

    // 寻找需要合并的两个物品
    CItem* pItem1 = QueryItemByGrid(nGrid1);
    CItem* pItem2 = QueryItemByGrid(nGrid2);
    // 两个物品都必须存在且都不是满数量的
    if(pItem1 == nullptr || pItem1->IsPileEnable() == false)
        return false;
    if(pItem2 == nullptr || pItem2->IsPileEnable() == false)
        return false;

    //只要类型一样就合并，掩码也同步合并
    if(pItem1->GetType() != pItem2->GetType())
        return false;

    uint32_t dwNewFlag = pItem1->_GetFlag() | pItem2->_GetFlag();

    // item1是目标物品，取得可以容纳的合并数量
    uint32_t nCombineNum = pItem1->GetCanPileNum();
    if(nCombineNum >= pItem2->GetPileNum())
    {
        //如果超过第二个物品现有数量，取第二个物品的数量，并直接删除第二个物品
        nCombineNum = pItem2->GetPileNum();
        // dlog
        DelItem(pItem2->GetID(), SYNC_TRUE, false); // 合并背包叠加物品不需要更新任务物品数量
    }
    else
    {
        // 否则仅仅更新第二个物品的expire data
        pItem2->DecPileNum(nCombineNum, UPDATE_TRUE);
        pItem2->SyncPileNum(m_pOwner);
    }
    // 更新第一个物品的expire data
    pItem1->SetFlag(dwNewFlag, UPDATE_FALSE);
    pItem1->AddPileNum(nCombineNum, UPDATE_TRUE);
    pItem1->SendItemInfo(m_pOwner);

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPackage::CombineAddItem(CItem* pItem, bool bSync /*=true*/)
{
    __ENTER_FUNCTION
    CHECKF(pItem);
    if(pItem->ItemTypePtr()->IsPileEnable() == false)
        return false;
    uint32_t nNum = CombineAddItem(pItem->GetType(), pItem->GetPileNum(), pItem->_GetFlag(), bSync);
    pItem->SetPileNum(nNum, UPDATE_TRUE);

    return true;
    __LEAVE_FUNCTION
    return false;
}

uint32_t CPackage::CombineAddItem(uint32_t idItemType, uint32_t nNum, uint32_t dwFlag, bool bSync /*=true*/)
{
    __ENTER_FUNCTION
    const CItemType* pItemType = ItemTypeSet()->QueryObj(idItemType);
    CHECKF(pItemType);
    if(pItemType->IsPileEnable() == false)
        return nNum;

    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pPackageItem = it->second;
        if(pPackageItem && pPackageItem->IsCombineEnable(idItemType, dwFlag))
        {
            uint32_t nPileNum    = nNum;
            uint32_t nCanPileNum = pPackageItem->ItemTypePtr()->GetPileLimit() - pPackageItem->GetPileNum();
            uint32_t nAdd        = __min(nPileNum, nCanPileNum);

            // 添加叠加数
            nNum -= nAdd;
            pPackageItem->AddPileNum(nAdd, UPDATE_TRUE);

            // 更新任务追踪物品数量
            if(pPackageItem->IsTraceItem())
            {
                m_pOwner->GetTaskSet()->OnAwardTaskItem(pPackageItem->GetType(), nAdd);
            }

            if(bSync)
                pPackageItem->SyncPileNum(m_pOwner);

            if(nNum == 0)
                break;
        }
    }

    return nNum;
    __LEAVE_FUNCTION
    return nNum;
}

bool CPackage::TidyItem()
{
    __ENTER_FUNCTION
    // step1: 进行自动合并
    {
        // 先搜索找出所有需要合并的物品
        std::deque<CItem*> setPileItem;
        for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
        {
            CItem* pItem = it->second;
            if(pItem && pItem->IsPileEnable())
            { // 可叠加物品，并且没有叠满，才可以参加自动合并
                setPileItem.push_back(pItem);
            }
        }
        // 遍历每个物品进行合并
        int32_t nCount = 0;
        while(setPileItem.empty() == false)
        {
            // 先取出第一个物品
            CItem* pTargetItem = setPileItem.front();
            setPileItem.pop_front();
            bool bUpdate = false;
            // 在剩余的物品中寻找可以合并的物品
            for(auto it = setPileItem.begin(); it != setPileItem.end();)
            {
                CItem* pItem = *it;
                if(pItem == nullptr)
                    continue;
                if(pTargetItem->IsCombineEnable(pItem))
                {
                    if(pTargetItem->ItemTypePtr()->GetPileLimit() >= pItem->GetPileNum() + pTargetItem->GetPileNum())
                    {
                        // 可以整个合并
                        pTargetItem->AddPileNum(pItem->GetPileNum(), UPDATE_FALSE);
                        bUpdate = true;
                        // dlog

                        DelItem(pItem->GetID(), SYNC_TRUE, false); // 整理背包不需要更新任务追踪物品数量
                        it = setPileItem.erase(it);
                        continue;
                    }
                    else
                    {
                        // 只能合并部分
                        int32_t nCombineAmount = pTargetItem->ItemTypePtr()->GetPileLimit() - pTargetItem->GetPileNum();
                        pTargetItem->SetPileNum(pTargetItem->ItemTypePtr()->GetPileLimit(), UPDATE_TRUE);
                        bUpdate = true;
                        pItem->DecPileNum(nCombineAmount, UPDATE_TRUE);
                        pItem->SyncPileNum(m_pOwner);
                        break;
                    }
                }
                it++;
            }

            if(bUpdate)
            {
                pTargetItem->SaveInfo();
                pTargetItem->SyncPileNum(m_pOwner);
            }
        }
    }

    // step2: 对背包中所有ITEM进行排序
    //将背包清空，根据sort进行排序
    std::vector<CItem*> setItem;
    setItem.reserve(m_setItem.size());
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        setItem.push_back(it->second);
    }
    m_setItem.clear();

    std::sort(setItem.begin(), setItem.end(), [](CItem* pItem1, CItem* pItem2) -> bool {
        CHECKF(pItem1);
        CHECKF(pItem2);

        uint32_t nOrder1 = pItem1->ItemTypePtr()->GetSort();
        uint32_t nOrder2 = pItem2->ItemTypePtr()->GetSort();

        if(nOrder1 != nOrder2)
            return nOrder1 < nOrder2;
        if(pItem1->GetType() != pItem2->GetType())
            return pItem1->GetType() < pItem2->GetType();
        return pItem1->GetID() < pItem2->GetID();
    });

    for(uint32_t nGrid = 0; nGrid < setItem.size(); nGrid++)
    {
        m_setItem[nGrid] = setItem[nGrid];
        setItem[nGrid]->SetGrid(nGrid, UPDATE_TRUE);
        setItem[nGrid]->SyncGridData(m_pOwner);
    }
    __LEAVE_FUNCTION
    return false;
}

CItem* CPackage::QueryItem(OBJID idItem)
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

CItem* CPackage::QueryItemByType(uint32_t idType, uint32_t dwFlag /*=0*/)
{
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetType() == idType && pItem->HasFlag(dwFlag))
            return pItem;
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CItem* CPackage::QueryItemByGrid(uint32_t nGrid)
{
    __ENTER_FUNCTION
    auto find = m_setItem.find(nGrid);
    if(find != m_setItem.end())
        return find->second;
    __LEAVE_FUNCTION
    return nullptr;
}

CItem* CPackage::FindCombineItem(uint32_t idType, uint32_t dwFlag, uint32_t nAmount)
{
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->IsCombineEnable(idType, dwFlag) && (pItem->GetPileNum() + nAmount) <= pItem->ItemTypePtr()->GetPileLimit())
        {
            return pItem;
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}

bool CPackage::HaveSoManyItem(uint32_t idType, uint32_t nNum, uint32_t dwFlag /*=0*/)
{
    __ENTER_FUNCTION
    CHECKF(nNum > 0); // 必须确保nNum>0才有意义

    uint32_t nAmount = 0;

    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetType() == idType)
        {
            // 不需要检查掩码或者掩码相同
            if(dwFlag == 0 || dwFlag == pItem->_GetFlag())
            {
                nAmount += pItem->GetPileNum();
                if(nAmount >= nNum)
                    return true;
            }
        }
    }
    __LEAVE_FUNCTION
    return false;
}

uint32_t CPackage::DelItemByType(uint32_t idType, uint32_t nNum, uint32_t dwFlag /*=0*/, bool bTraceTaskItem /*=true*/)
{
    __ENTER_FUNCTION
    CHECKF(nNum > 0); // 必须确保nNum>0才有意义
    const CItemType* pItemType = ItemTypeSet()->QueryObj(idType);
    CHECKF(pItemType);

    uint32_t nToDelNum = nNum; // 需要删除的数量
    uint32_t nAmount   = 0;    // 已删除数量
    for(auto it = m_setItem.begin(); it != m_setItem.end();)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetType() == idType)
        {
            if(dwFlag == 0 || dwFlag == pItem->_GetFlag())
            {
                // 不需要检查掩码或者掩码相同
                if(pItem->ItemTypePtr()->IsPileEnable())
                {
                    if(pItem->GetPileNum() > (nNum - nAmount))
                    {
                        pItem->DecPileNum(nNum - nAmount, UPDATE_FALSE);
                        pItem->SyncPileNum(m_pOwner);
                        nAmount = nNum;
                        nNum    = 0;
                        break;
                    }
                    else
                    {
                        nAmount += pItem->GetPileNum();
                    }
                }
                else
                    ++nAmount;

                it = m_setItem.erase(it);

                if(pItem->ItemTypePtr()->GetExpireTime() != 0)
                {
                    RemoveItemExpireCallBack(pItem->GetID(), pItem->GetExpireTime());
                }

                pItem->SendDeleteMsg(m_pOwner);

                pItem->DelRecord();
                SAFE_DELETE(pItem);
                if(nNum > 0 && nAmount >= nNum)
                {
                    nNum = 0;
                    break;
                }
                continue;
            }
        }
        it++;
    }

    uint32_t nDelNum = nAmount;
    if(bTraceTaskItem && m_pOwner && pItemType->IsTraceItem())
    {
        m_pOwner->GetTaskSet()->OnDelTaskItem(idType, nDelNum);
    }

    return nAmount;
    __LEAVE_FUNCTION
    return 0;
}

uint32_t CPackage::GetItemTypeAmount(uint32_t idType, uint32_t dwFlag /*=0*/)
{
    uint32_t nAmount = 0;
    __ENTER_FUNCTION
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        CItem* pItem = it->second;
        if(pItem && pItem->GetType() == idType)
        {
            // 不需要检查掩码或者掩码相同
            if(dwFlag == 0 || dwFlag == pItem->_GetFlag())
            {
                nAmount += pItem->GetPileNum();
            }
        }
    }
    __LEAVE_FUNCTION
    return nAmount;
}

uint32_t CPackage::FindFirstEmptyGrid()
{
    __ENTER_FUNCTION
    uint32_t nNextGrid = 1;
    for(auto it = m_setItem.begin(); it != m_setItem.end(); it++)
    {
        if(it->first != nNextGrid)
        {
            return nNextGrid;
        }
        nNextGrid++;
    }
    if(nNextGrid > m_nMaxSize)
        return 0;
    else
        return nNextGrid;
    __LEAVE_FUNCTION
    return 0;
}

void CPackage::RemoveItemExpireCallBack(OBJID idItem, uint32_t dwExpireData)
{
    __ENTER_FUNCTION
    auto it_range = m_setExpireItem.equal_range(dwExpireData);
    for(auto it = it_range.first; it != it_range.second;)
    {
        if(it->second == idItem)
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
void CPackage::AddItemExpireCallBack(OBJID idItem, uint32_t dwExpireData)
{
    m_setExpireItem.insert(std::make_pair(dwExpireData, idItem));
}

void CPackage::CheckItemExpire(uint32_t dwTimeNow)
{
    __ENTER_FUNCTION
    if(m_setExpireItem.empty())
        return;

    auto itEnd = m_setExpireItem.upper_bound(dwTimeNow);
    if(itEnd == m_setExpireItem.begin())
        return;

    for(auto it = m_setExpireItem.begin(); it != itEnd;)
    {
        OBJID  idItem = it->second;
        CItem* pItem  = QueryItem(idItem);
        if(pItem)
        {
            uint64_t idScript = pItem->ItemTypePtr()->GetScriptID();

            ScriptManager()->TryExecScript<void>(SCRIPT_ITEM, idScript, "OnTimeOut", pItem, m_pOwner);

            //带掩码的物品，自动删除
            if(pItem->HasFlag(ITEMFLAG_DELITEM_EXPIRE))
            {
                // dlog
                DelItem(idItem, SYNC_TRUE, true);
            }
        }

        m_setExpireItem.erase(it++);
    }
    __LEAVE_FUNCTION
}
