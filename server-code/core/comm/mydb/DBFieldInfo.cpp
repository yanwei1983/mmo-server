#include "DBFieldInfo.h"
#include "mysql/mysql.h"

MYSQL_FIELD_COPY::MYSQL_FIELD_COPY(MYSQL_FIELD* pField, uint32_t idx)
{
    m_name     = (pField->name ? pField->name : "");
    m_table    = (pField->table ? pField->table : "");
    m_isPriKey = IS_PRI_KEY(pField->flags);
    m_idx      = idx;
    switch(pField->type)
    {
        case MYSQL_TYPE_TINY:
        {
            if(pField->flags & UNSIGNED_FLAG)
            {
                m_field_type = DB_FIELD_TYPE_TINY_UNSIGNED;
            }
            else
            {
                m_field_type = DB_FIELD_TYPE_TINY;
            }
        }
        break;
        case MYSQL_TYPE_SHORT:
        {
            if(pField->flags & UNSIGNED_FLAG)
            {
                m_field_type = DB_FIELD_TYPE_SHORT_UNSIGNED;
            }
            else
            {
                m_field_type = DB_FIELD_TYPE_SHORT;
            }
        }
        break;
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_INT24:
        {
            if(pField->flags & UNSIGNED_FLAG)
            {
                m_field_type = DB_FIELD_TYPE_LONG_UNSIGNED;
            }
            else
            {
                m_field_type = DB_FIELD_TYPE_LONG;
            }
        }
        break;
        case MYSQL_TYPE_LONGLONG:
        {
            if(pField->flags & UNSIGNED_FLAG)
            {
                m_field_type = DB_FIELD_TYPE_LONGLONG_UNSIGNED;
            }
            else
            {
                m_field_type = DB_FIELD_TYPE_LONGLONG;
            }
        }
        break;
        case MYSQL_TYPE_NEWDECIMAL:
        case MYSQL_TYPE_BIT:
        {
            m_field_type = DB_FIELD_TYPE_LONGLONG_UNSIGNED;
        }
        break;
        case MYSQL_TYPE_FLOAT:
        {
            m_field_type = DB_FIELD_TYPE_FLOAT;
        }
        break;
        case MYSQL_TYPE_DOUBLE:
        {
            m_field_type = DB_FIELD_TYPE_DOUBLE;
        }
        break;
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_JSON:
        {
            m_field_type = DB_FIELD_TYPE_VARCHAR;
        }
        break;
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        {
            m_field_type = DB_FIELD_TYPE_BLOB;
        }
        break;
        default:
        {
            m_field_type = DB_FIELD_TYPE_UNSUPPORED;
        }
        break;
    }
}



CMysqlFieldInfoList::CMysqlFieldInfoList(MYSQL_RES* res)
{
    int32_t nFields = mysql_num_fields(res);
    for(uint32_t i = 0; i < nFields; i++)
    {
        auto pField = mysql_fetch_field(res);
        if(pField == nullptr)
        {
            LOGDBERROR("mysql_fetch_field Error:{}", i);
            continue;
        }
        m_FieldInfos.push_back(std::make_unique<MYSQL_FIELD_COPY>(pField, i));
    }
}

CMysqlFieldInfoList::~CMysqlFieldInfoList() {}

const CDBFieldInfo* CMysqlFieldInfoList::operator[](size_t idx) const
{
    CHECKF_V(idx < size(), idx);
    return m_FieldInfos[idx].get();
}
const CDBFieldInfo* CMysqlFieldInfoList::get(size_t idx) const
{
    CHECKF_V(idx < size(), idx);
    return m_FieldInfos[idx].get();
}
size_t              CMysqlFieldInfoList::size() const { return m_FieldInfos.size(); }
const CDBFieldInfo* CMysqlFieldInfoList::find_field(const std::string& name)
{
    auto it_find = std::find_if(m_FieldInfos.begin(), m_FieldInfos.end(), [name](const std::unique_ptr<CDBFieldInfo>& v) {
        return name == v->GetFieldName();
    });

    if(it_find != m_FieldInfos.end())
    {
        return (*it_find).get();
    }
    return nullptr;
}