#include "LoggingMgr.h"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/printf.h>

#include "FileUtil.h"
#include "Thread.h"
#include "TimeUtil.h"

namespace BaseCode
{
    int32_t              s_default_logger = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_network_logger = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_db_logger      = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_stack_logger   = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_debug_logger   = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_error_logger   = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_message_logger = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_warning_logger = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_fatal_logger   = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_lua_logger     = LOG4Z_MAIN_LOGGER_ID;
    thread_local int32_t s_monitor_logger = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_ai_logger      = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_login_logger   = LOG4Z_MAIN_LOGGER_ID;
    int32_t              s_gm_logger      = LOG4Z_MAIN_LOGGER_ID;
} // namespace BaseCode

static std::string g_logPath;

void BaseCode::InitMonitorLog(const std::string& logname)
{
    using namespace zsummer::log4z;
    BaseCode::s_monitor_logger = ILog4zManager::getRef().createLogger(logname.c_str());
    if(g_logPath.empty() == false)
    {
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_monitor_logger, (g_logPath + "/monitor/").c_str());
    }
    ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_monitor_logger, false);
    ILog4zManager::getRef().setLoggerFileLine(BaseCode::s_monitor_logger, false);
    ILog4zManager::getRef().setLoggerLimitsize(BaseCode::s_monitor_logger, 50);
    LOGDEBUG("InitMonitor:{}", logname);
}

void BaseCode::MyLogMsgX(const char* pszName, const char* pszBuffer)
{
    if(!pszName || !pszBuffer)
        return;
    auto        curTime     = std::chrono::system_clock::now();
    std::time_t now_c       = std::chrono::system_clock::to_time_t(curTime);
    auto        localtime_c = timeToLocalTime(now_c);

    std::string szLogName = fmt::format("{}/{}_{:%Y-%m-%d}.log", g_logPath, pszName, localtime_c);

    FILE* fp = fopen(szLogName.c_str(), "a+");
    if(nullptr == fp)
        return;

    fmt::print(fp, "{:%H:%M:%S} [{:d}] {:s}\n", localtime_c, get_cur_thread_id(), pszBuffer);

    fclose(fp);
}

void BaseCode::InitLog(const std::string& path, int32_t log_lev)
{
    using namespace zsummer::log4z;
    BaseCode::s_debug_logger   = ILog4zManager::getRef().createLogger("debug");
    BaseCode::s_error_logger   = ILog4zManager::getRef().createLogger("error");
    BaseCode::s_message_logger = ILog4zManager::getRef().createLogger("message");
    BaseCode::s_warning_logger = ILog4zManager::getRef().createLogger("warning");
    BaseCode::s_fatal_logger   = ILog4zManager::getRef().createLogger("fatal");

    BaseCode::s_network_logger = ILog4zManager::getRef().createLogger("network");
    BaseCode::s_db_logger      = ILog4zManager::getRef().createLogger("db");
    BaseCode::s_stack_logger   = ILog4zManager::getRef().createLogger("stack");
    BaseCode::s_lua_logger     = ILog4zManager::getRef().createLogger("lua");
    BaseCode::s_ai_logger      = ILog4zManager::getRef().createLogger("ai");
    BaseCode::s_login_logger   = ILog4zManager::getRef().createLogger("login");
    BaseCode::s_gm_logger      = ILog4zManager::getRef().createLogger("gm");

    ILog4zManager::getRef().enableLogger(LOG4Z_MAIN_LOGGER_ID, false);
    ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);
    ILog4zManager::getRef().setLoggerOutFile(LOG4Z_MAIN_LOGGER_ID, false);
    if(path.empty() == false)
    {
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_default_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_debug_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_error_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_message_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_warning_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_fatal_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_network_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_db_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_stack_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_lua_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_ai_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_login_logger, path.c_str());
        ILog4zManager::getRef().setLoggerPath(BaseCode::s_gm_logger, path.c_str());
        g_logPath = path;
    }

    {
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_debug_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_error_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_message_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_warning_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_fatal_logger, false);

        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_network_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_db_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_stack_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_lua_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_ai_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_login_logger, false);
        ILog4zManager::getRef().setLoggerDisplay(BaseCode::s_gm_logger, false);
    }
    {
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_debug_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_error_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_message_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_warning_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_fatal_logger, log_lev);

        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_network_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_db_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_stack_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_lua_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_ai_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_login_logger, log_lev);
        ILog4zManager::getRef().setLoggerLevel(BaseCode::s_gm_logger, log_lev);
    }

    ILog4zManager::getRef().setLoggerFileLine(BaseCode::s_lua_logger, false);
    ILog4zManager::getRef().setLoggerFileLine(BaseCode::s_gm_logger, false);

    createRecursionDir(g_logPath + "/aidebug");

    ILog4zManager::getRef().start();
}

void BaseCode::StopLog()
{
    using namespace zsummer::log4z;

    const char* end_line = "======================================================================";
    uint32_t max_log = ILog4zManager::getRef().getStatusActiveLoggers();
    for(uint32_t log_id = 0; log_id < max_log; log_id++)
    {
        if(ILog4zManager::getRef().getLoggerLogCount(log_id) > 0)
        {
            ZLOGFMT_DEBUG(log_id, end_line);
        }
    }

   

    ILog4zManager::getRef().stop();
}

static thread_local NDC* this_thread_NDC = nullptr;

std::string BaseCode::SetNdc(const std::string& name)
{
    std::string oldNdc;
    if(this_thread_NDC == nullptr)
    {
        this_thread_NDC = new NDC{name};
    }
    else
    {
        oldNdc = this_thread_NDC->ndc;
        if(name.empty())
        {
            SAFE_DELETE(this_thread_NDC);
        }
        else
        {
            this_thread_NDC->ndc = name;
        }
    }
    return oldNdc;
}

void BaseCode::ClearNdc()
{
    SAFE_DELETE(this_thread_NDC);
}

NDC* BaseCode::getNdc()
{
    return this_thread_NDC;
}

std::string BaseCode::getNdcStr()
{
    return (this_thread_NDC) ? this_thread_NDC->ndc : std::string();
}