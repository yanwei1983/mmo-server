#ifndef MYSQLSTMT_H
#define MYSQLSTMT_H

#include <memory>

#include "mysql/mysql.h"

using MYSQL_STMT_PTR = std::unique_ptr<MYSQL_STMT, decltype(&mysql_stmt_close)>;

class CMysqlStmt
{
public:
    CMysqlStmt()
        : m_pMysqlStmt(nullptr, mysql_stmt_close)
        , m_ParamsCount(0)
    {
    }

    CMysqlStmt(MYSQL_STMT* stmt)
        : m_pMysqlStmt(stmt, mysql_stmt_close)
        , m_ParamsCount(mysql_stmt_param_count(stmt))
        , m_Params(std::make_unique<MYSQL_BIND[]>(m_ParamsCount))
    {
    }

    CMysqlStmt(CMysqlStmt&& rht)
        : m_pMysqlStmt(rht.m_pMysqlStmt.release(), mysql_stmt_close)
        , m_ParamsCount(rht.m_ParamsCount)
        , m_Params(rht.m_Params.release())
    {
    }

    ~CMysqlStmt() {}

public:
    operator bool() { return !!m_pMysqlStmt; }

    void _BindParam(int32_t i, enum_field_types buffer_type, void* buffer, int32_t buffer_length, bool* is_null, uint64_t* length)
    {
        MYSQL_BIND& b   = m_Params[i];
        b.buffer_type   = buffer_type;
        b.buffer        = buffer;
        b.buffer_length = buffer_length;
        b.is_null       = is_null;
        b.length        = length;
    }

    void BindParam(int32_t i, const int32_t& x) { _BindParam(i, MYSQL_TYPE_LONG, (void*)&x, 0, 0, 0); }

    void BindParam(int32_t i, const int64_t& x) { _BindParam(i, MYSQL_TYPE_LONGLONG, (void*)&x, 0, 0, 0); }

    void BindParam(int32_t i, const std::string& x) { _BindParam(i, MYSQL_TYPE_STRING, (void*)x.c_str(), x.size(), 0, &(m_Params[i].buffer_length)); }

    template<class... Args>
    void Execute(Args&&... args)
    {
        _BindParams(0, std::forward<Args>(args)...);
        mysql_stmt_bind_param(m_pMysqlStmt.get(), m_Params.get());
        //		mysql_stmt_bind_result(m_pMysqlStmt.get(), m_Result.get());
        mysql_stmt_execute(m_pMysqlStmt.get());
    }

    /*void store_result()
    {
        mysql_stmt_store_result(m_pMysqlStmt.get());
        mysql_stmt_fetch(m_pMysqlStmt.get());

    }
    void free_result()
    {
        mysql_stmt_free_result(m_pMysqlStmt.get());
    }*/

    template<class T, class... Args>
    void _BindParams(int32_t n, const T& x, Args&&... args)
    {
        BindParam(n, x);
        _BindParams(n + 1, std::forward<Args>(args)...);
    }

    // base case
    void _BindParams(int32_t n) {}

private:
    MYSQL_STMT_PTR                m_pMysqlStmt;
    size_t                        m_ParamsCount;
    std::unique_ptr<MYSQL_BIND[]> m_Params;
};

#endif /* MYSQLSTMT_H */
