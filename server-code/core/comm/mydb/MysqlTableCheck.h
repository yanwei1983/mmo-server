#ifndef MYSQLTABLECHECK_H
#define MYSQLTABLECHECK_H

#include "MysqlConnection.h"

struct MysqlTableCheck
{
    template<class T>
    static bool CheckTable(CMysqlConnection* pDB)
    {
        __ENTER_FUNCTION
        auto result = pDB->UnionQuery(attempt_format("SHOW CREATE TABLE {}", T::table_name()));
        if(result == nullptr)
        {
            LOGDBFATAL("GameDB Check Error, table:{} not find in mysql", T::table_name());
            return false;
        }
        else
        {
            // 0 = table_name, 1= table_sql
            auto record = result->fetch_row();

            std::string sql_create_table = record->Field(1).template get<std::string>();
            std::string sql_ddl          = T::create_sql();
            trim_if(sql_create_table, [](unsigned char c) -> bool { return c == '\n'; });
            trim_if(sql_ddl, [](unsigned char c) -> bool { return c == '\n'; });
            if(sql_create_table == sql_ddl)
                return true;
        }

        auto pFieldInfo_DDL = std::make_unique<CDDLFieldInfoList<T>>();
        if(pFieldInfo_DDL->size_fields == 0)
        {
            LOGDBFATAL("GameDB Check Error, table:{} not find in ddl", T::table_name());
            return false;
        }

        auto ptr              = pDB->MakeRecord(T::table_name());
        auto pFieldInfo_MYSQL = pDB->QueryFieldInfo(T::table_name());
        if(pFieldInfo_MYSQL == nullptr)
        {
            LOGDBFATAL("GameDB Check Error, table:{} not find in MYSQL", T::table_name());
            return false;
        }

        if(pFieldInfo_MYSQL->size() != pFieldInfo_DDL->size_fields)
        {
            LOGDBFATAL("GameDB Check Error, table:{} in MYSQL size:{} not equal then DDL size:{}.",
                       T::table_name(),
                       pFieldInfo_MYSQL->size(),
                       pFieldInfo_DDL->size_fields);
            return false;
        }

        for(size_t i = 0; i < pFieldInfo_DDL->size_fields; i++)
        {
            const CDBFieldInfo* pInfo_DDL   = pFieldInfo_DDL->get(i);
            const CDBFieldInfo* pInfo_MYSQL = pFieldInfo_MYSQL->get(i);
            if(pInfo_DDL == nullptr || pInfo_MYSQL == nullptr)
            {
                LOGDBFATAL("GameDB Check Error, table:{}, field:{}", T::table_name(), i);
                return false;
            }
            else if(pInfo_DDL->GetFieldType() != pInfo_MYSQL->GetFieldType())
            {
                LOGDBFATAL("GameDB Check Error, table:{}, field:{} ddl_field:{}  "
                           "mysql_field:{} ddl_fieldt:{} mysql_fieldt:{}",
                           T::table_name(),
                           i,
                           pInfo_DDL->GetFieldName(),
                           pInfo_MYSQL->GetFieldName(),
                           pInfo_DDL->GetFieldType(),
                           pInfo_MYSQL->GetFieldType());
                return false;
            }
        }
        return true;
        __LEAVE_FUNCTION
        return false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TupleType, std::size_t first, std::size_t... is>
    static inline bool CheckAllTableImpl(CMysqlConnection* pDB, std::index_sequence<first, is...>)
    {
        using TableType = typename std::tuple_element<first, TupleType>::type;
        auto result     = CheckTable<TableType>(pDB);
        if constexpr(sizeof...(is) > 0)
        {
            return result && CheckAllTableImpl<TupleType>(pDB, std::index_sequence<is...>{});
        }
        else
        {
            return result;
        }
    }

    template<class type_list_Table>
    static inline bool CheckAllTable(CMysqlConnection* pDB)
    {
        using TupleType             = typename type_list_Table::tuple_type;
        constexpr size_t tuple_size = type_list_Table::size;
        return CheckAllTableImpl<TupleType>(pDB, std::make_index_sequence<tuple_size>{});
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<class TableType, class KeyType>
    static inline bool CheckKeysAndFix(CMysqlConnection* pDB, KeyType&& key_info)
    {
        auto key_name      = std::get<0>(key_info);
        auto key_field_str = std::get<1>(key_info);
        auto result        = pDB->UnionQuery(attempt_format("SHOW INDEX FROM {} WHERE Key_name='{}'", TableType::table_name(), key_name));
        if(result == nullptr)
        {
            // add
            auto sql = attempt_format("ALTER TABLE {} ADD INDEX {}({})", TableType::table_name(), key_name, key_field_str);
            CHECKF(pDB->SyncExec(sql));
        }
        else
        {
            auto vec_key_field = split_string(key_field_str, ",");
            bool check_fail    = false;
            for(int i = 0; i < result->get_num_row(); i++)
            {
                auto record      = result->fetch_row(false);
                auto column_name = record->Field("Column_name").template get<std::string>();
                if(column_name != vec_key_field[i])
                {
                    check_fail = true;
                    break;
                }
            }

            if(check_fail)
            {
                // modify
                LOGDBERROR("GameDB Check Error, table:{} key:{} is not same with DDL, Need Fix!!!!!!.", TableType::table_name(), key_name);
                return false;
            }
        }

        return true;
    }

    template<typename TableType, typename KeysTupleType, std::size_t first, std::size_t... is>
    static inline bool CheckAllKeysAndFixImpl(CMysqlConnection* pDB, KeysTupleType&& KeysInfo, std::index_sequence<first, is...>)
    {
        auto key_info = std::get<first>(KeysInfo);
        auto result   = CheckKeysAndFix<TableType>(pDB, key_info);
        if constexpr(sizeof...(is) > 0)
        {
            return result && CheckAllKeysAndFixImpl<TableType, KeysTupleType>(pDB, KeysInfo, std::index_sequence<is...>{});
        }
        else
        {
            return result;
        }
    }
    template<typename TableType>
    static inline bool CheckAllKeysAndFix(CMysqlConnection* pDB)
    {
        constexpr auto   keys_info = TableType::keys_info();
        constexpr size_t keys_size = TableType::keys_size();
        return CheckAllKeysAndFixImpl<TableType>(pDB, keys_info, std::make_index_sequence<keys_size>{});
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    static bool CheckTableAndFix(CMysqlConnection* pDB)
    {
        __ENTER_FUNCTION
        auto result = pDB->UnionQuery(attempt_format("SHOW CREATE TABLE {}", T::table_name()));
        if(result == nullptr)
        {
            CHECKF(pDB->SyncExec(T::create_sql()));
            return true;
        }
        else
        {
            // 0 = table_name, 1= table_sql
            auto record = result->fetch_row();

            std::string sql_create_table = record->Field(1).template get<std::string>();
            std::string sql_ddl          = T::create_sql();
            trim_if(sql_create_table, [](unsigned char c) -> bool { return c == '\n'; });
            trim_if(sql_ddl, [](unsigned char c) -> bool { return c == '\n'; });
            if(sql_create_table == sql_ddl)
                return true;

            LOGDBDEBUG("\nDDL:\n{}\nMYSQL:\n{}\n", sql_ddl, sql_create_table);
        }

        auto pFieldInfo_DDL = std::make_unique<CDDLFieldInfoList<T>>();
        if(pFieldInfo_DDL->size_fields == 0)
        {
            LOGDBFATAL("GameDB Check Error, table:{} not find in ddl", T::table_name());
            return false;
        }

        auto ptr              = pDB->MakeRecord(T::table_name());
        auto pFieldInfo_MYSQL = pDB->QueryFieldInfo(T::table_name());
        if(pFieldInfo_MYSQL == nullptr)
        {
            LOGDBERROR("GameDB Check Error, table:{} not find in MYSQL.  Auto Fix.", T::table_name());
            // auto create table, and query again
            CHECKF(pDB->SyncExec(T::create_sql()));
            ptr              = pDB->MakeRecord(T::table_name());
            pFieldInfo_MYSQL = pDB->QueryFieldInfo(T::table_name());
            CHECKF(pFieldInfo_MYSQL);
        }

        if(pFieldInfo_MYSQL->size() > pFieldInfo_DDL->size_fields)
        {
            LOGDBERROR("GameDB Check Error, table:{} in MYSQL size:{} greaterthen DDL size:{}. Auto Fix.",
                       T::table_name(),
                       pFieldInfo_MYSQL->size(),
                       pFieldInfo_DDL->size_fields);
        }
        else if(pFieldInfo_MYSQL->size() < pFieldInfo_DDL->size_fields)
        {
            LOGDBERROR("GameDB Check Error, table:{} in MYSQL less then DDL, Auto Fix.", T::table_name());
        }

        // find need remove
        std::vector<size_t> vecRemoves;
        for(size_t i = 0; i < pFieldInfo_MYSQL->size(); i++)
        {
            const CDBFieldInfo* pInfo_MYSQL = pFieldInfo_MYSQL->get(i);
            if(pInfo_MYSQL == nullptr)
            {
                LOGDBFATAL("GameDB Check Error, table:{}, field:{}", T::table_name(), i);
                return false;
            }
            auto pInfo_DDL = pFieldInfo_DDL->find_field(pInfo_MYSQL->GetFieldName());
            if(pInfo_DDL == nullptr)
            {
                auto sql = attempt_format("ALTER TABLE {} DROP COLUMN {}", T::table_name(), pInfo_MYSQL->GetFieldName());
                CHECKF(pDB->SyncExec(sql));
            }
            else if(pInfo_DDL->GetFieldType() != pInfo_MYSQL->GetFieldType())
            {
                auto sql = attempt_format("ALTER TABLE {} MODIFY COLUMN {}", T::table_name(), pInfo_DDL->GetFieldSql());
                CHECKF(pDB->SyncExec(sql));
            }
        }

        // find need add
        for(size_t i = 0; i < pFieldInfo_DDL->size_fields; i++)
        {
            const CDBFieldInfo* pInfo_DDL = pFieldInfo_DDL->get(i);
            if(pInfo_DDL == nullptr)
            {
                LOGDBFATAL("GameDB Check Error, table:{}, field:{}", T::table_name(), i);
                return false;
            }
            auto pInfo_MYSQL = pFieldInfo_MYSQL->find_field(pInfo_DDL->GetFieldName());
            if(pInfo_MYSQL == nullptr)
            {
                if(i == 0)
                {
                    auto sql = attempt_format("ALTER TABLE {} ADD COLUMN {} FIRST", T::table_name(), pInfo_DDL->GetFieldSql());
                    CHECKF(pDB->SyncExec(sql));
                }
                const CDBFieldInfo* pInfo_DDL_before = pFieldInfo_DDL->get(i - 1);
                if(pInfo_DDL_before == nullptr)
                {
                    LOGDBFATAL("GameDB Check Error, table:{}, field:{} can't find before field", T::table_name(), i);
                    return false;
                }
                auto sql =
                    attempt_format("ALTER TABLE {} ADD COLUMN {} AFTER {}", T::table_name(), pInfo_DDL->GetFieldSql(), pInfo_DDL_before->GetFieldName());
                CHECKF(pDB->SyncExec(sql));
            }
        }

        // fix keys.
        CheckAllKeysAndFix<T>(pDB);

        return true;
        __LEAVE_FUNCTION
        return false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TupleType, std::size_t first, std::size_t... is>
    static inline bool CheckAllTableAndFixImpl(CMysqlConnection* pDB, std::index_sequence<first, is...>)
    {
        using TableType = typename std::tuple_element<first, TupleType>::type;
        auto result     = CheckTableAndFix<TableType>(pDB);
        if constexpr(sizeof...(is) > 0)
        {
            return result && CheckAllTableAndFixImpl<TupleType>(pDB, std::index_sequence<is...>{});
        }
        else
        {
            return result;
        }
    }

    template<class type_list_Table>
    static inline bool CheckAllTableAndFix(CMysqlConnection* pDB)
    {
        using TupleType             = typename type_list_Table::tuple_type;
        constexpr size_t tuple_size = type_list_Table::size;
        return CheckAllTableAndFixImpl<TupleType>(pDB, std::make_index_sequence<tuple_size>{});
    }
};

#endif /* MYSQLTABLECHECK_H */
