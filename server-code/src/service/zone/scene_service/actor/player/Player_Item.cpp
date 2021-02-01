#include "CoolDown.h"
#include "DataCount.h"
#include "Equipment.h"
#include "Item.h"
#include "ItemType.h"
#include "Package.h"
#include "Player.h"
#include "SceneService.h"
#include "ScriptCallBackType.h"
#include "ScriptManager.h"
#include "StoragePackage.h"
#include "config/Cfg_Scene_LeavePoint.pb.h"
#include "server_msg/server_side.pb.h"

CPackage* CPlayer::QueryPackage(uint32_t nPosition)
{
    __ENTER_FUNCTION
    if(nPosition == ITEMPOSITION_BAG)
        return GetBag();
    else if(nPosition == ITEMPOSITION_STORAGE)
        return GetStroagePackage();
    else
        return nullptr;
    __LEAVE_FUNCTION
    return nullptr;
}

CStoragePackage* CPlayer::GetStroagePackage()
{
    __ENTER_FUNCTION
    if(m_pStoragePackage == nullptr)
    {
        m_pStoragePackage.reset(CreateNew<CStoragePackage>(this, ITEMPOSITION_STORAGE, GetStrogeMaxSize()));
        CHECKF(m_pStoragePackage);
        m_pStoragePackage->SendAllItemInfo();
    }
    return m_pStoragePackage.get();
    __LEAVE_FUNCTION
    return nullptr;
}

bool CPlayer::SpendItem(uint32_t nItemLogType, uint32_t idItemType, uint32_t nCount /*= 1*/, bool bUseBindFirst /*= true*/)
{
    __ENTER_FUNCTION
    CHECKF(idItemType > 0);
    CHECKF(nCount > 0);
    switch(idItemType)
    {
        case MT_MONEY:
        case MT_MONEYBIND:
        case MT_GOLD:
        case MT_GOLDBIND:
            return SpendMoney(idItemType, nCount);
            break;
        default:
        {
            if(bUseBindFirst == true)
                nCount -= GetBag()->DelItemByType(idItemType, nCount, ITEMFLAG_EXCHANGE_DISABLE);
            if(nCount > 0)
                nCount -= GetBag()->DelItemByType(idItemType, nCount, 0);
            return nCount == 0;
        }
        break;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::AwardItem(uint32_t nItemLogType, uint32_t idItemType, uint32_t nCount, uint32_t dwFlag)
{
    __ENTER_FUNCTION

    switch(idItemType)
    {
        case MT_MONEY:
        case MT_MONEYBIND:
        case MT_GOLD:
        case MT_GOLDBIND:
            return AwardMeony(idItemType, nCount);
            break;
        default:
            return GetBag()->AwardItem(idItemType, nCount, dwFlag);
            break;
    }

    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::CheckItem(uint32_t idItemType, uint32_t nCount /*= 1*/, uint32_t dwFlag)
{
    __ENTER_FUNCTION
    return GetBag()->HaveSoManyItem(idItemType, nCount, dwFlag);
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::UseItem(uint32_t nGridInBag, uint32_t nCount /* = 1*/)
{
    __ENTER_FUNCTION
    CItem* pItem = GetBag()->QueryItemByGrid(nGridInBag);
    CHECKF(pItem);

    if(nCount > pItem->GetPileNum())
        nCount = pItem->GetPileNum();

    const CItemType* pItemType = ItemTypeSet()->QueryObj(pItem->GetType());
    CHECKF(pItemType);

    if(nCount > pItemType->GetBatchUseCount())
        nCount = pItemType->GetBatchUseCount();

    if(nCount == 0)
        return false;

    if(pItemType->GetCDType() != 0 && GetCDSet()->IsCoolDown(COOLDOWN_TYPE_ITEM, pItemType->GetCDType()) == true)
        return false;

    if(pItemType->GetScriptID() == 0)
        return false;

    if(pItemType->HasFlag(ITEMFLAG_BATCH_USE_CHECK))
    {
        //检查每日限量
        uint64_t nMaxCount = GetDataCountSet()->GetMaxCount(DATA_ACC_USEITEM, pItemType->GetID());
        uint64_t nCurCount = GetDataCountSet()->GetCount(DATA_ACC_USEITEM, pItemType->GetID());
        if(nMaxCount <= nCurCount)
            return false;

        uint64_t nCanUseCount = (nMaxCount - nCurCount);
        if(nCanUseCount < nCount)
            nCount = nCanUseCount;

        bool find = ScriptManager()->QueryScriptFunc(SCRIPT_ITEM, pItemType->GetScriptID(), "OnBatchUseCheck");
        if(find)
        {
            nCount = ScriptManager()->ExecStackScriptFunc<uint32_t>(pItem, nCount, this);
            if(nCount == 0)
                return false;
        }
    }

    bool bSucc = ScriptManager()->TryExecScript<bool>(SCRIPT_ITEM, pItemType->GetScriptID(), "OnUse", pItem, nCount, this);
    if(bSucc == false)
        return false;

    //增加计数
    GetDataCountSet()->_AddCount(DATA_ACC_USEITEM, pItemType->GetID(), nCount);
    if(pItemType->GetCDType() != 0)
        GetCDSet()->StartCoolDown(COOLDOWN_TYPE_ITEM, pItemType->GetCDType(), pItemType->GetCDMsces());

    // dlog

    if(pItem->GetPileNum() == nCount)
    {
        GetBag()->DelItem(pItem->GetGrid(), true, true);
    }
    else
    {
        pItem->DecPileNum(nCount, UPDATE_TRUE);
        pItem->SyncPileNum(this);
    }

    __LEAVE_FUNCTION
    return false;
}
