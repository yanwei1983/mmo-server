#include "ItemData.h"

#include "ItemAddition.h"
#include "ItemType.h"
#include "Player.h"
#include "SceneService.h"
#include "gamedb.h"
#include "DB2PB.h"

OBJECTHEAP_IMPLEMENTATION(CItemData, s_heap);

CItemData::CItemData()
    : m_ExtraData(std::make_unique<ItemExtraData>())
{
}

CItemData::~CItemData() 
{

}

bool CItemData::Init(CDBRecordPtr&& pRes)
{
    __ENTER_FUNCTION
    CHECKF(pRes != nullptr);
    m_pRecord.reset(pRes.release());

    m_pType = ItemTypeSet()->QueryObj(GetType());
    if(!m_pType)
    {
        LOGERROR("UNKNOW itemtype:{}", GetType());
        return false;
    }

    DB2PB::LinkProtoWithJsonTxt(m_pRecord->Field(TBLD_ITEM::EXTRA), *m_ExtraData);

    uint32_t nAddition = GetAddition();
    if(nAddition > 0)
    {
        m_pAddition = ItemAdditionSet()->QueryItemAddition(GetType(), nAddition);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItemData::Init(CMysqlConnection* pDB, OBJID idItem)
{
    __ENTER_FUNCTION
    CHECKF(pDB);
    CHECKF(idItem);

    auto pResult = pDB->QueryKeyLimit<TBLD_ITEM, TBLD_ITEM::ID>(idItem, 1);
    if(pResult == nullptr)
        return false;

    m_pRecord = pResult->fetch_row();
    CHECKF(m_pRecord != nullptr);

    DB2PB::LinkProtoWithJsonTxt(m_pRecord->Field(TBLD_ITEM::EXTRA), *m_ExtraData);

    m_pType = ItemTypeSet()->QueryObj(GetType());
    if(!m_pType)
    {
        LOGERROR("UNKNOW itemtype:{}", GetType());
        return false;
    }

    uint32_t nAddition = GetAddition();
    if(nAddition > 0)
    {
        m_pAddition = ItemAdditionSet()->QueryItemAddition(GetType(), nAddition);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItemData::Init(CMysqlConnection* pDB, ST_ITEMINFO& info)
{
    __ENTER_FUNCTION
    m_pType = ItemTypeSet()->QueryObj(info.idType);
    if(!m_pType)
    {
        LOGERROR("UNKNOW itemtype:{}", info.idType);
        return false;
    }

    m_pRecord = pDB->MakeRecord(TBLD_ITEM::table_name());
    if(!m_pRecord)
        return false;
    m_pRecord->Field(TBLD_ITEM::ID)           = info.idItem;
    m_pRecord->Field(TBLD_ITEM::ITEMTYPE)     = info.idType;
    m_pRecord->Field(TBLD_ITEM::OWNER_ID)     = info.idOwner;
    m_pRecord->Field(TBLD_ITEM::POSITION)     = info.nPosition;
    m_pRecord->Field(TBLD_ITEM::GRID)         = info.nGrid;
    m_pRecord->Field(TBLD_ITEM::ADDITION_LEV) = info.nAddition;
    m_pRecord->Field(TBLD_ITEM::PILENUM)      = info.nNum;
    if(m_pType->GetExpireTime() > 0)
        m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME) = uint32_t(TimeGetSecond()) + m_pType->GetExpireTime();
    else
        m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME) = 0;
    m_pRecord->Field(TBLD_ITEM::DURA)       = m_pType->GetDura();
    m_pRecord->Field(TBLD_ITEM::DURA_LIMIT) = m_pType->GetDuraLimit();
    m_pRecord->Field(TBLD_ITEM::FLAG)       = info.dwFlag;

    if(m_pRecord->Update() == false)
        return false;

    DB2PB::LinkProtoWithJsonTxt(m_pRecord->Field(TBLD_ITEM::EXTRA), *m_ExtraData);
    uint32_t nAddition = GetAddition();
    if(nAddition > 0)
    {
        m_pAddition = ItemAdditionSet()->QueryItemAddition(GetType(), nAddition);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItemData::Init(CMysqlConnection* pDB, uint64_t idPlayer, uint32_t idItemType, uint32_t nNum, uint32_t dwFlag, uint32_t nPosition)
{
    __ENTER_FUNCTION
    m_pType = ItemTypeSet()->QueryObj(idItemType);
    if(!m_pType)
    {
        LOGERROR("UNKNOW itemtype:{}", idItemType);
        return false;
    }

    m_pRecord = pDB->MakeRecord(TBLD_ITEM::table_name());
    if(!m_pRecord)
        return false;
    m_pRecord->Field(TBLD_ITEM::ITEMTYPE) = idItemType;
    m_pRecord->Field(TBLD_ITEM::OWNER_ID) = idPlayer;
    m_pRecord->Field(TBLD_ITEM::PILENUM)  = nNum;
    m_pRecord->Field(TBLD_ITEM::POSITION) = nPosition;
    m_pRecord->Field(TBLD_ITEM::FLAG)     = dwFlag;

    DB2PB::LinkProtoWithJsonTxt(m_pRecord->Field(TBLD_ITEM::EXTRA), *m_ExtraData);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItemData::DelRecord()
{
    __ENTER_FUNCTION
    m_pRecord->DeleteRecord();
    m_pRecord.reset();
    m_pType     = nullptr;
    m_pAddition = nullptr;
    __LEAVE_FUNCTION
    return false;
}

bool CItemData::DelRecordStatic(CMysqlConnection* pDB, OBJID idItem)
{
    __ENTER_FUNCTION
    CHECKF(pDB);
    pDB->SyncExec(attempt_format(FMT_STRING("DELETE FROM {} WHERE id={}"), TBLD_ITEM::table_name(), idItem));
    __LEAVE_FUNCTION
    return false;
}

OBJID CItemData::GetID() const
{
    return m_pRecord->Field(TBLD_ITEM::ID);
}
uint32_t CItemData::GetType() const
{
    return m_pRecord->Field(TBLD_ITEM::ITEMTYPE);
}
OBJID CItemData::GetOwnerID() const
{
    return m_pRecord->Field(TBLD_ITEM::OWNER_ID);
}
uint32_t CItemData::GetPosition() const
{
    return m_pRecord->Field(TBLD_ITEM::POSITION);
}
uint32_t CItemData::GetGrid() const
{
    return m_pRecord->Field(TBLD_ITEM::GRID);
}
uint32_t CItemData::CItemData::GetPileNum() const
{
    return m_pRecord->Field(TBLD_ITEM::PILENUM);
}
uint32_t CItemData::GetDura() const
{
    return m_pRecord->Field(TBLD_ITEM::DURA);
}
uint32_t CItemData::GetDuraLimit() const
{
    return m_pRecord->Field(TBLD_ITEM::DURA_LIMIT);
}
uint32_t CItemData::GetExpireTime() const
{
    return m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME);
}
uint32_t CItemData::GetAddition() const
{
    return m_pRecord->Field(TBLD_ITEM::ADDITION_LEV);
}
uint32_t CItemData::_GetFlag() const
{
    return m_pRecord->Field(TBLD_ITEM::FLAG);
}
uint32_t CItemData::GetFlag() const
{
    return _GetFlag() | ItemTypePtr()->GetFlag();
}
bool CItemData::HasFlag(uint32_t dwFlag) const
{
    return ::HasFlag(GetFlag(), dwFlag);
}

const std::string& CItemData::GetName() const
{
    return m_pType->GetName();
}
const CItemType* CItemData::ItemTypePtr() const
{
    return m_pType;
}
const CItemAdditionData* CItemData::AdditionType()
{
    return m_pAddition;
}
uint32_t CItemData::GetMainType() const
{
    return GetMainTypeByID(GetType());
}
uint32_t CItemData::GetMainTypeByID(uint32_t idType)
{
    return idType / ITEM_MAINTYPE_MASK;
}
uint32_t CItemData::GetSubType() const
{
    return GetSubTypeByID(GetType());
}
uint32_t CItemData::GetSubTypeByID(uint32_t idType)
{
    return (idType % ITEM_MAINTYPE_MASK) / ITEM_SUBTYPE_MASK;
}

uint32_t CItemData::GetExtra(uint32_t nIdx) const
{
    CHECKF_V(nIdx >= 0 && nIdx < MAX_ITEM_EXTRDATA_NUM && nIdx < (uint32_t)m_ExtraData->data_size(), nIdx);
    return m_ExtraData->data(nIdx);
}

void CItemData::SetOwnerID(OBJID idOwner, bool bUpdate /*=true*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::OWNER_ID) = idOwner;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetTypeID(OBJID idType, bool bUpdate /*=true*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::ITEMTYPE) = idType;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetPosition(uint8_t ucPosition, bool bUpdate /*=true*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::POSITION) = ucPosition;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetGrid(uint32_t nData, bool bUpdate /*=true*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::GRID) = nData;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetPileNum(uint32_t nNum, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::PILENUM) = nNum;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::AddPileNum(uint32_t nNum, bool bUpdate /*= false*/)
{
    __ENTER_FUNCTION
    uint32_t nFNum = GetPileNum() + nNum;
    if(nFNum > ItemTypePtr()->GetPileLimit())
        SetPileNum(ItemTypePtr()->GetPileLimit(), bUpdate);
    else
        SetPileNum(nFNum, bUpdate);
    __LEAVE_FUNCTION
}

void CItemData::DecPileNum(uint32_t nNum, bool bUpdate /*= false*/)
{
    __ENTER_FUNCTION
    if(GetPileNum() > nNum)
        SetPileNum(GetPileNum() - nNum, bUpdate);
    else
        SetPileNum(0, bUpdate);
    __LEAVE_FUNCTION
}

void CItemData::SetExpireTime(uint32_t nExpireTime, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME) = nExpireTime;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetAddition(uint32_t nAddition, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::ADDITION_LEV) = nAddition;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetDura(uint32_t nDura, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::DURA) = nDura;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetDuraLimit(uint32_t nDuraLimit, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::DURA_LIMIT) = nDuraLimit;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetFlag(uint32_t dwFlag, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    m_pRecord->Field(TBLD_ITEM::FLAG) = dwFlag;
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}

void CItemData::SetExtra(int32_t nIdx, uint32_t nExtra, bool bUpdate /*=false*/)
{
    __ENTER_FUNCTION
    for(int32_t i = m_ExtraData->data_size(); i < nIdx; i++)
    {
        m_ExtraData->add_data(0);
    }
    m_ExtraData->set_data(nIdx, nExtra);
    m_pRecord->Field(TBLD_ITEM::EXTRA).MakeDirty();
    if(bUpdate)
        m_pRecord->Update();
    __LEAVE_FUNCTION
}
