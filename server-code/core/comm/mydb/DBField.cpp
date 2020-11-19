#include "DBField.h"

#include "DBRecord.h"
#include "MysqlConnection.h"

CDBField::CDBField(CDBRecord* pDBRecord, const CDBFieldInfo* pFieldInfo, char* pVal, uint32_t len)
    : m_pDBRecord(pDBRecord)
    , m_pFieldInfo(pFieldInfo)
{
    __ENTER_FUNCTION
    switch(m_pFieldInfo->GetFieldType())
    {

        case DB_FIELD_TYPE_TINY_UNSIGNED:
        case DB_FIELD_TYPE_SHORT_UNSIGNED:
        case DB_FIELD_TYPE_LONG_UNSIGNED:

        {
            if(pVal == nullptr || len == 0)
                m_Val = uint32_t(0);
            else
                m_Val = uint32_t(std::stoul(pVal));
        }
        break;
        case DB_FIELD_TYPE_TINY:
        case DB_FIELD_TYPE_SHORT:
        case DB_FIELD_TYPE_LONG:
        {
            if(pVal == nullptr || len == 0)
                m_Val = int32_t(0);
            else
                m_Val = int32_t(std::stol(pVal));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG_UNSIGNED:
        {
            if(pVal == nullptr || len == 0)
                m_Val = uint64_t(0);
            else
                m_Val = uint64_t(std::stoull(pVal));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG:
        {
            if(pVal == nullptr || len == 0)
                m_Val = int64_t(0);
            else
                m_Val = int64_t(std::stoll(pVal));
        }
        break;
        case DB_FIELD_TYPE_FLOAT:
        {
            if(pVal == nullptr || len == 0)
                m_Val = float(0.0f);
            else
                m_Val = float(std::stof(pVal));
        }
        break;
        case DB_FIELD_TYPE_DOUBLE:
        {
            if(pVal == nullptr || len == 0)
                m_Val = double(0.0);
            else
                m_Val = double(std::stod(pVal));
        }
        break;
        case DB_FIELD_TYPE_VARCHAR:
        case DB_FIELD_TYPE_BLOB:
        {
            if(pVal == nullptr || len == 0)
                m_Val = std::string("");
            else
                m_Val = std::string(pVal, len);
        }
        break;
        default:
        {
            
        }
        break;
    }

    __LEAVE_FUNCTION
}

CDBField::CDBField() {}

bool CDBField::IsString() const
{
    return (m_pFieldInfo->GetFieldType() == DB_FIELD_TYPE_VARCHAR);
}

std::string CDBField::GetValString() const
{
    __ENTER_FUNCTION
    if(m_pFieldInfo == nullptr)
        return std::string();
    if(m_funcGetValString)
    {

        std::string             tmp    = m_funcGetValString();
        std::unique_ptr<char[]> szBuff = std::make_unique<char[]>(tmp.size() * 2 + 1);
        mysql_real_escape_string(m_pDBRecord->_GetMysqlConnection()->_GetHandle(), szBuff.get(), tmp.c_str(), tmp.size());
        return "'" + std::string(szBuff.get()) + "'";
    }

    switch(m_pFieldInfo->GetFieldType())
    {
        case DB_FIELD_TYPE_TINY_UNSIGNED:
        case DB_FIELD_TYPE_SHORT_UNSIGNED:
        case DB_FIELD_TYPE_LONG_UNSIGNED:
        {
            return std::to_string(std::get<uint32_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_TINY:
        case DB_FIELD_TYPE_SHORT:
        case DB_FIELD_TYPE_LONG:
        {
            return std::to_string(std::get<int32_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG_UNSIGNED:
        {
            return std::to_string(std::get<uint64_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG:
        {
            return std::to_string(std::get<int64_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_FLOAT:
        {
            return std::to_string(std::get<float>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_DOUBLE:
        {
            return std::to_string(std::get<double>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_VARCHAR:
        case DB_FIELD_TYPE_BLOB:
        {
            const std::string& tmp = std::get<std::string>(m_Val);
            std::unique_ptr<char[]> szBuff = std::make_unique<char[]>(tmp.size() * 2 + 1);
            mysql_real_escape_string(m_pDBRecord->_GetMysqlConnection()->_GetHandle(), szBuff.get(), tmp.c_str(), tmp.size());

            return "'" + std::string(szBuff.get()) + "'";
        }
        break;
        default:
        {
            LOGDBERROR("mysql field:{}.{} use unknow type.", m_pFieldInfo->GetTableName(), m_pFieldInfo->GetFieldName());
            LOGDBERROR("CallStack：{}", GetStackTraceString(CallFrameMap(3, 7)));
            return std::string();
        }
        break;
    }
    __LEAVE_FUNCTION
    return std::string();
}

bool CDBField::CanModify() const
{
    if(m_pDBRecord)
        return m_pDBRecord->CanModify();
    return false;
}

void CDBField::MakeDirty()
{
    m_pDBRecord->MakeDirty(m_pFieldInfo->GetFieldIdx());
}

bool CDBField::IsDirty() const
{
    if(m_pDBRecord && m_pFieldInfo)
        return m_pDBRecord->IsDirty(m_pFieldInfo->GetFieldIdx());
    return false;
}

int32_t CDBField::GetFieldIndex() const
{
    return m_pFieldInfo ? m_pFieldInfo->GetFieldIdx() : -1;
}

const char* CDBField::GetFieldName() const
{
    return m_pFieldInfo ? m_pFieldInfo->GetFieldName() : "";
}
