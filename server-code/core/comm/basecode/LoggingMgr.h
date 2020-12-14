#ifndef LoggingMgr_H
#define LoggingMgr_H

#include <iostream>

#include <fmt/format.h>

#include "BaseType.h"
#include "log4z.h"

struct NDC
{
    std::string ndc;
};

namespace BaseCode
{
    void MyLogMsgX(const char* pszName, bool logData, const char* pszBuffer);
    void MyLogMsgX(const char* pszName, bool logData, const char* pSrcFile, int32_t line, const char* pszBuffer);

    template<typename... Args>
    void MyLogMsg(const char* pszName, bool logData, const char* pSrcFile, int32_t line, const char* fmt, Args&&... args)
    {
        MyLogMsgX(pszName, logData, pSrcFile, line, fmt::format(fmt, std::forward<Args>(args)...).c_str());
    }

    template<typename... Args>
    void PrintfError(const char* fmt, Args&&... args)
    {
        std::cerr << fmt::format(fmt, std::forward<Args>(args)...) << std::endl;
    }
} // namespace BaseCode

namespace BaseCode
{
    void        InitLog(const std::string& path, int32_t log_lev = LOG_LEVEL_DEBUG);
    void        InitMonitorLog(const std::string& logname);
    std::string SetNdc(const std::string& name);
    void        ClearNdc();
    NDC*        getNdc();
    std::string getNdcStr();
    void        StopLog();
    void        CreateExtLogDir();
    void        SetLogLev(int log_lev);
    bool        IsLogRunning();

    extern int32_t s_default_logger;
    extern int32_t s_debug_logger;
    extern int32_t s_error_logger;
    extern int32_t s_message_logger;
    extern int32_t s_warning_logger;
    extern int32_t s_fatal_logger;
    extern int32_t s_network_logger;
    extern int32_t s_db_logger;
    extern int32_t s_stack_logger;
    extern int32_t s_lua_logger;
    extern int32_t s_ai_logger;
    extern int32_t s_login_logger;
    extern int32_t s_gm_logger;

    extern thread_local int32_t s_monitor_logger;

    extern int32_t g_log_aidebug;
    extern int32_t g_log_actordebug;
    extern int32_t g_log_skilldebug;
} // namespace BaseCode

#define LOGTRACE(...)  ZLOGFMT_TRACE(BaseCode::s_debug_logger, ##__VA_ARGS__)
#define LOGDEBUG(...)  ZLOGFMT_DEBUG(BaseCode::s_debug_logger, ##__VA_ARGS__)
#define LOGASSERT(...) ZLOGFMT_DEBUG(BaseCode::s_debug_logger, ##__VA_ARGS__)

#define LOG_DETAIL_M(expr, g_log_lev, log_lev, log_dir, actor_id, fmtr, ...)                                                   \
    if(expr && log_lev >= g_log_lev)                                                                                           \
    {                                                                                                                          \
        BaseCode::MyLogMsg(fmt::format(log_dir "/role_{}", actor_id).c_str(), false, __FILE__, __LINE__, fmtr, ##__VA_ARGS__); \
    }

#define LOGACTORDEBUG(actor_id, fmtr, ...) \
    LOG_DETAIL_M(true, BaseCode::g_log_actordebug, LOG_LEVEL_DEBUG, "actordebug", actor_id, fmtr, ##__VA_ARGS__)
#define LOGACTORTRACE(actor_id, fmtr, ...) \
    LOG_DETAIL_M(true, BaseCode::g_log_actordebug, LOG_LEVEL_TRACE, "actordebug", actor_id, fmtr, ##__VA_ARGS__)
#define LOGSKILLDEBUG(expr, actor_id, fmtr, ...) \
    LOG_DETAIL_M(expr, BaseCode::g_log_skilldebug, LOG_LEVEL_DEBUG, "skilldebug", actor_id, fmtr, ##__VA_ARGS__)
#define LOGAIDEBUG(expr, actor_id, fmtr, ...) LOG_DETAIL_M(expr, BaseCode::g_log_aidebug, LOG_LEVEL_DEBUG, "aidebug", actor_id, fmtr, ##__VA_ARGS__)

#define LOGNETDEBUG(...) ZLOGFMT_DEBUG(BaseCode::s_network_logger, ##__VA_ARGS__)
#define LOGNETINFO(...)  ZLOGFMT_INFO(BaseCode::s_network_logger, ##__VA_ARGS__)
#define LOGNETERROR(...) ZLOGFMT_ERROR(BaseCode::s_network_logger, ##__VA_ARGS__)
#define LOGNETTRACE(...) ZLOGFMT_TRACE(BaseCode::s_network_logger, ##__VA_ARGS__)

#define LOGDBDEBUG(...) ZLOGFMT_DEBUG(BaseCode::s_db_logger, ##__VA_ARGS__)
#define LOGDBINFO(...)  ZLOGFMT_INFO(BaseCode::s_db_logger, ##__VA_ARGS__)
#define LOGDBERROR(...) ZLOGFMT_ERROR(BaseCode::s_db_logger, ##__VA_ARGS__)
#define LOGDBFATAL(...) ZLOGFMT_FATAL(BaseCode::s_db_logger, ##__VA_ARGS__)

#define LOGSTACK(...) ZLOGFMT_ERROR(BaseCode::s_stack_logger, ##__VA_ARGS__)

#define LOGINFO(...)          ZLOGFMT_INFO(BaseCode::s_message_logger, ##__VA_ARGS__)
#define LOGMESSAGE(...)       ZLOGFMT_INFO(BaseCode::s_message_logger, ##__VA_ARGS__)
#define LOGMESSAGE_NOFMT(msg) ZLOG_INFO(BaseCode::s_message_logger, msg)

#define LOGWARNING(...) ZLOGFMT_WARN(BaseCode::s_warning_logger, ##__VA_ARGS__)
#define LOGERROR(...)   ZLOGFMT_ERROR(BaseCode::s_error_logger, ##__VA_ARGS__)
#define LOGFATAL(msg, ...)                                     \
    {                                                          \
        std::string err_str = fmt::format(msg, ##__VA_ARGS__); \
        ZLOG_FATAL(BaseCode::s_fatal_logger, err_str);         \
        fmt::print(stderr, "{}\n", err_str);                   \
    }

#define LOGLUA(...)      ZLOGFMT_ERROR(BaseCode::s_lua_logger, ##__VA_ARGS__)
#define LOGLUAERROR(...) ZLOGFMT_ERROR(BaseCode::s_lua_logger, ##__VA_ARGS__)
#define LOGLUADEBUG(...) ZLOGFMT_DEBUG(BaseCode::s_lua_logger, ##__VA_ARGS__)

#define LOGMONITOR(...) ZLOGFMT_INFO(BaseCode::s_monitor_logger, ##__VA_ARGS__)
#define LOGLOGIN(...)   ZLOGFMT_INFO(BaseCode::s_login_logger, ##__VA_ARGS__)
#define LOGGM(...)      ZLOGFMT_INFO(BaseCode::s_gm_logger, ##__VA_ARGS__)

#endif /* LoggingMgr_H */
