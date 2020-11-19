#include "StoragePackage.h"

#include "Item.h"
#include "Player.h"
CStoragePackage::CStoragePackage() {}

CStoragePackage::~CStoragePackage() {}

void CStoragePackage::CheckIn(uint32_t nGridInPackage)
{
    __ENTER_FUNCTION
    CItem* pItem = m_pOwner->GetBag()->PopItemByGrid(nGridInPackage, true);
    if(pItem == nullptr)
        return;
    //先尝试能不能直接合并入
    CombineAddItem(pItem);
    if(pItem->GetPileNum() != 0)
    {
        AddItem(pItem, SYNC_TRUE, true, true);
        pItem->SaveInfo();
    }
    else
    {
        pItem->DelRecord();
        SAFE_DELETE(pItem);
    }
    __LEAVE_FUNCTION
}

void CStoragePackage::CheckOut(uint32_t nGridInStorage)
{
    __ENTER_FUNCTION
    CItem* pItem = PopItemByGrid(nGridInStorage, true);
    if(pItem == nullptr)
        return;
    m_pOwner->GetBag()->CombineAddItem(pItem);
    if(pItem->GetPileNum() != 0)
    {
        m_pOwner->GetBag()->AddItem(pItem, SYNC_TRUE, true, true);
        pItem->SaveInfo();
    }
    else
    {
        pItem->DelRecord();
        SAFE_DELETE(pItem);
    }
    __LEAVE_FUNCTION
}
