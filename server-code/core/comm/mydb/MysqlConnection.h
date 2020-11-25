#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include <memory>
#include <unordered_map>

#include "DBRecord.h"
#include "StringAlgo.h"
#include "Thread.h"
#include "mysql/mysql.h"

class CMysqlConnection;
class CMysqlStmt;
class CMysqlResult;

using CMysqlStmtPtr = std::unique_ptr<CMysqlStmt>;
using MYSQL_PTR     = std::unique_ptr<MYSQL, decltype(&mysql_close)>;
using MYSQL_RES_PTR = std::unique_ptr<MYSQL_RES, decltype(&mysql_free_result)>;

inline MYSQL_RES_PTR make_mysql_res_ptr(MYSQL_RES* res)
{
    return MYSQL_RES_PTR{res, mysql_free_result};
}

class CMysqlResult
{
public:
    CMysqlResult(CMysqlConnection* pMysqlConnection, MYSQL_RES_PTR&& res, const CDBFieldInfoListPtr& infolist_ptr);
    ~CMysqlResult();

    CDBFieldInfoListPtr GetFieldInfo() const;
    size_t              get_num_row();

    CDBRecordPtr fetch_row(bool bModify = true);

private:
    CMysqlConnection*   m_pMysqlConnection;
    MYSQL_RES_PTR       m_MySqlResult;
    CDBFieldInfoListPtr m_pFieldInfoList;
};
using CMysqlResultPtr = std::unique_ptr<CMysqlResult>;

template<class TABLE_T, uint32_t KEY_IDX, class KEY_T>
struct DBCond
{
    KEY_T key;
          operator std::string() const
    {
        std::string result = fmt::format(FMT_STRING("{}={}"), DBFieldHelp<TABLE_T, KEY_IDX>::GetFieldName(), key);
        return result;
    }
};

// not thread safe
class CMysqlConnection
{
public:
    CMysqlConnection();

    ~CMysqlConnection();
    bool            Connect(const std::string& strUrl, uint32_t client_flag = 0, bool bCreateAsync = false);
    bool            Connect(const std::string& host,
                            const std::string& user,
                            const std::string& password,
                            const std::string& db,
                            uint32_t           port,
                            uint32_t           client_flag  = 0,
                            bool               bCreateAsync = false);
    CMysqlStmtPtr   Prepare(const std::string& s);
    CMysqlResultPtr UnionQuery(const std::string& query);

    CMysqlResultPtr Query(const std::string& table_name, const std::string& query);
    CMysqlResultPtr QueryAll(const std::string& table_name);
    uint64_t        Update(const std::string& query);
    uint64_t        Insert(const std::string& query);
    bool            SyncExec(const std::string& s);
    void            AsyncExec(const std::string& s);
    void            JoinAsyncThreadFinish();
    CDBRecordPtr    MakeRecord(const std::string& table_name);

    CDBFieldInfoListPtr QueryFieldInfo(const std::string& s);
    CDBFieldInfoListPtr CreateFieldInfo(const std::string& s, const MYSQL_RES_PTR& res);
    void                _AddFieldInfo(const std::string& s, const CDBFieldInfoListPtr& ptr);
    MYSQL*              _GetHandle() const { return m_pHandle.get(); }
    bool                EscapeString(char* pszDst, const char* pszSrc, int32_t nLen);
    bool                EscapeString(std::string& strDst, const std::string& strSrc);

    template<class TABLE_T, class DB_COND_T>
    CMysqlResultPtr QueryCond(DB_COND_T&& cond)
    {
        std::string sql = fmt::format(FMT_STRING("SELECT * FROM {} WHERE {}"), TABLE_T::table_name(), std::forward<DB_COND_T>(cond));
        return Query(TABLE_T::table_name(), sql);
    }

    template<class TABLE_T, class... DB_COND_T>
    CMysqlResultPtr QueryMultiCond(DB_COND_T&&... conds)
    {
        std::vector<std::string> cond_vec;
        (cond_vec.emplace_back(std::forward<DB_COND_T>(conds)), ...);
        std::string cond_str = string_concat(cond_vec, " AND ", "", "");
        std::string sql      = fmt::format(FMT_STRING("SELECT * FROM {} WHERE {}"), TABLE_T::table_name(), cond_str);
        return Query(TABLE_T::table_name(), sql);
    }

    template<class TABLE_T, uint32_t KEY_IDX, class KEY_T>
    CMysqlResultPtr QueryKey(KEY_T key)
    {
        std::string sql =
            fmt::format(FMT_STRING("SELECT * FROM {} WHERE {}={}"), TABLE_T::table_name(), DBFieldHelp<TABLE_T, KEY_IDX>::GetFieldName(), key);
        return Query(TABLE_T::table_name(), sql);
    }

    template<class TABLE_T, uint32_t KEY_IDX, class KEY_T>
    CMysqlResultPtr QueryKeyLimit(KEY_T key, uint32_t limit)
    {
        std::string sql = fmt::format(FMT_STRING("SELECT * FROM {} WHERE {}={} LIMIT {}"),
                                      TABLE_T::table_name(),
                                      DBFieldHelp<TABLE_T, KEY_IDX>::GetFieldName(),
                                      key,
                                      limit);
        return Query(TABLE_T::table_name(), sql);
    }

private:
    uint64_t        GetInsertID();
    uint64_t        GetAffectedRows();
    bool            MoreResults();
    bool            NextResult();
    CMysqlResultPtr UseResult(const std::string& s);

private:
    MYSQL_PTR                                            m_pHandle;
    MYSQL_PTR                                            m_pAsyncHandle;
    std::unordered_map<std::string, CDBFieldInfoListPtr> m_MysqlFieldInfoCache{};
    std::unique_ptr<CWorkerThread>                       m_AsyncThread{};
    MPSCQueue<std::string*>                              m_MessageQueue;

    std::mutex m_Mutex;
};
using CMysqlConnectionPtr = std::unique_ptr<CMysqlConnection>;

#endif /* MYSQLCONNECTION_H */
