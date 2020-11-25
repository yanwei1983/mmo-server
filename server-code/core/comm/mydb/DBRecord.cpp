#include "DBRecord.h"

#include "MysqlConnection.h"

OBJECTHEAP_IMPLEMENTATION(CDBField, s_Heap);
OBJECTHEAP_IMPLEMENTATION(CDBRecord, s_Heap);

//////////////////////////////////////////////////////////////////////////

CDBRecord::CDBRecord(CMysqlConnection*          pMysqlConnection,
                     const CDBFieldInfoListPtr& pDBFieldInfo,
                     bool                       bCanModify,
                     MYSQL_ROW                  row,
                     unsigned long*             lengths)
    : m_pMysqlConnection(pMysqlConnection)
    , m_pDBFieldInfo(pDBFieldInfo)
    , m_bCanModify(bCanModify)
    , m_bNeedCreateFirst(false)
{
    __ENTER_FUNCTION
    if(CanModify() == false && row == nullptr)
    {
        LOGDBERROR("try to make a unmodify record");
    }
    else
    {
        if(row == nullptr)
        {
            //没有数据的,那就是Make出来的
            m_bNeedCreateFirst = true;
        }

        //通过row 转换为CMysqlField
        m_FieldsByIdx.reserve(m_pDBFieldInfo->size());
        for(size_t i = 0; i < m_pDBFieldInfo->size(); i++)
        {
            auto ref_field_info_ptr = m_pDBFieldInfo->get(i);
            if(ref_field_info_ptr == nullptr)
            {
                LOGDBERROR("query field_info from CDBFieldInfoListPtr Error:{}", i);
                continue;
            }
            if(row == nullptr)
            {
                auto pMysqlField                                   = std::make_unique<CDBField>(this, ref_field_info_ptr, nullptr, 0);
                m_FieldsByName[ref_field_info_ptr->GetFieldName()] = pMysqlField.get();
                m_FieldsByIdx.push_back(std::move(pMysqlField));

                if(ref_field_info_ptr->IsPriKey())
                {
                    m_nPriKeyIdx.insert(i);
                    m_TableName = ref_field_info_ptr->GetTableName();
                }
            }
            else
            {
                //有数据
                auto pMysqlField                                   = std::make_unique<CDBField>(this, ref_field_info_ptr, row[i], lengths[i]);
                m_FieldsByName[ref_field_info_ptr->GetFieldName()] = pMysqlField.get();

                if(CanModify())
                {
                    if(ref_field_info_ptr->IsPriKey())
                    {
                        m_nPriKeyIdx.insert(i);
                        m_TableName = ref_field_info_ptr->GetTableName();
                        if(m_strPriKeyBuf.empty() == false)
                            m_strPriKeyBuf += ",";

                        m_strPriKeyBuf += ref_field_info_ptr->GetFieldName();
                        m_strPriKeyBuf += "=";
                        m_strPriKeyBuf += pMysqlField->GetValString();
                    }
                }

                m_FieldsByIdx.push_back(std::move(pMysqlField));
            }
        }
    }

    __LEAVE_FUNCTION
}

CDBRecord::~CDBRecord()
{
    __ENTER_FUNCTION
    m_FieldsByName.clear();
    m_FieldsByIdx.clear();

    __LEAVE_FUNCTION
}

bool CDBRecord::Update(bool bSync)
{
    __ENTER_FUNCTION
    if(CanModify() == false)
        return false;
    if(IsDirty() == false)
        return false;

    if(m_bNeedCreateFirst)
    {
        std::string sql = BuildInsertSQL();
        if(sql.empty())
        {
            // log error
            LOGDBERROR("BuildInsertSQL fail:{}", m_TableName.c_str());
            return false;
        }
        uint64_t insert_id = m_pMysqlConnection->Insert(sql);
        if(insert_id == static_cast<uint64_t>(-1))
        {
            return false;
        }
        if(m_nPriKeyIdx.size() == 1)
        {
            uint32_t idx   = *m_nPriKeyIdx.begin();
            auto&    field = *(m_FieldsByIdx[idx]);
            if(insert_id != 0)
            {
                if(field.GetType() == DB_FIELD_TYPE_LONG)
                    field.set<uint32_t>(insert_id);
                else
                    field.set<uint64_t>(insert_id);
            }

            m_strPriKeyBuf += field.GetFieldInfo()->GetFieldName();
            m_strPriKeyBuf += "=";
            m_strPriKeyBuf += field.GetValString();
        }
        else
        {
            // rebuildPriKeyBuf
            m_strPriKeyBuf.clear();
            for(uint32_t idx: m_nPriKeyIdx)
            {
                auto&       pField     = m_FieldsByIdx[idx];
                const auto& pFieldInfo = pField->GetFieldInfo();
                if(pFieldInfo->IsPriKey())
                {
                    if(m_strPriKeyBuf.empty() == false)
                        m_strPriKeyBuf += ",";

                    m_strPriKeyBuf += pFieldInfo->GetFieldName();
                    m_strPriKeyBuf += "=";
                    m_strPriKeyBuf += pField->GetValString();
                }
            }
        }

        ClearDirty();

        m_bNeedCreateFirst = false;
    }
    else
    {
        std::string sql = BuildUpdateSQL();
        if(sql.empty())
        {
            // log error
            LOGDBERROR("BuildUpdateSQL fail:{}", m_TableName.c_str());
            return false;
        }

        if(bSync)
        {
            m_pMysqlConnection->Update(sql);
            return true;
        }
        else
        {
            m_pMysqlConnection->AsyncExec(sql);
        }
        ClearDirty();
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CDBRecord::ClearDirty()
{
    __ENTER_FUNCTION
    m_setDirty.clear();
    __LEAVE_FUNCTION
}

void CDBRecord::DeleteRecord(bool bSync)
{
    __ENTER_FUNCTION
    if(CanModify() == false)
        return;

    std::string sql = BuildDeleteSQL();
    if(sql.empty())
    {
        // log error
        LOGDBERROR("BuildDeleteSQL fail:{}", m_TableName.c_str());
        return;
    }

    if(bSync)
    {
        if(m_pMysqlConnection->Update(sql) != 1)
        {
            // log error
            LOGDBERROR("DBRecord Update fail:{}", sql.c_str());
        }
    }
    else
    {
        m_pMysqlConnection->AsyncExec(sql);
    }
    __LEAVE_FUNCTION
}

std::string CDBRecord::BuildDeleteSQL()
{
    __ENTER_FUNCTION
    if(m_strPriKeyBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return fmt::format(FMT_STRING("DELETE FROM {} WHERE {} LIMIT 1"), m_TableName, m_strPriKeyBuf);
    __LEAVE_FUNCTION
    return std::string();
}

std::string CDBRecord::BuildUpdateSQL()
{
    __ENTER_FUNCTION
    std::string szKeyBuf;
    for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
    {
        CDBField* pField = m_FieldsByIdx[i].get();
        if(IsDirty(i) == false)
        {
            continue;
        }
        else
        {
            auto ref_field_info_ptr = (*m_pDBFieldInfo)[i];
            if(szKeyBuf.empty() == false)
                szKeyBuf += ",";
            std::string val_string = pField->GetValString();
            CHECK_RETTYPE_V(val_string.empty() == false, std::string, i);

            szKeyBuf += ref_field_info_ptr->GetFieldName();
            szKeyBuf += "=";
            szKeyBuf += val_string;
        }
    }
    if(szKeyBuf.empty() || m_strPriKeyBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return fmt::format(FMT_STRING("UPDATE {} SET {} WHERE {} LIMIT 1"), m_TableName, szKeyBuf, m_strPriKeyBuf);
    __LEAVE_FUNCTION
    return std::string();
}

std::string CDBRecord::DumpInsertSQL() const
{
    __ENTER_FUNCTION
    std::string szKeyNameBuf;
    std::string szKeyValBuf;
    for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
    {
        CDBField* pField             = m_FieldsByIdx[i].get();
        auto      ref_field_info_ptr = (*m_pDBFieldInfo)[i];

        if(szKeyNameBuf.empty() == false)
        {
            szKeyNameBuf += ",";
            szKeyValBuf += ",";
        }
        std::string val_string = pField->GetValString();
        if(val_string.empty() == false)
        {
            szKeyNameBuf += ref_field_info_ptr->GetFieldName();
            szKeyValBuf += val_string;
        }
    }
    if(szKeyNameBuf.empty() || szKeyValBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return fmt::format(FMT_STRING("INSERT INTO {} ({}) VALUES ({}) "), m_TableName, szKeyNameBuf, szKeyValBuf);
    __LEAVE_FUNCTION
    return std::string();
}

std::string CDBRecord::BuildInsertSQL()
{
    __ENTER_FUNCTION
    std::string szKeyNameBuf;
    std::string szKeyValBuf;
    for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
    {
        CDBField* pField             = m_FieldsByIdx[i].get();
        auto      ref_field_info_ptr = (*m_pDBFieldInfo)[i];
        if(IsDirty(i) == false)
        {
            continue;
        }
        else
        {
            if(szKeyNameBuf.empty() == false)
            {
                szKeyNameBuf += ",";
                szKeyValBuf += ",";
            }
            std::string val_string = pField->GetValString();
            if(val_string.empty() == false)
            {
                szKeyNameBuf += ref_field_info_ptr->GetFieldName();
                szKeyValBuf += val_string;
            }
        }
    }
    if(szKeyNameBuf.empty() || szKeyValBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return fmt::format(FMT_STRING("INSERT INTO {} ({}) VALUES ({}) "), m_TableName, szKeyNameBuf, szKeyValBuf);
    __LEAVE_FUNCTION
    return std::string();
}

bool CDBRecord::IsDirty() const
{
    return m_setDirty.any();
}

bool CDBRecord::IsDirty(uint32_t idx) const
{
    return m_setDirty.test(idx);
}

void CDBRecord::MakeDirty(uint32_t idx)
{
    m_setDirty.set(idx, true);
}