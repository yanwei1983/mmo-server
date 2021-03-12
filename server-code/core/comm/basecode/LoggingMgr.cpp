#include "LoggingMgr.h"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/printf.h>

#include "FileUtil.h"
#include "GlobalSetting.h"
#include "Thread.h"
#include "TimeUtil.h"

namespace BaseCode
{
    int32_t s_default_logger = LOG_MAIN_LOGGER_ID;
    int32_t s_network_logger = LOG_MAIN_LOGGER_ID;
    int32_t s_db_logger      = LOG_MAIN_LOGGER_ID;
    int32_t s_stack_logger   = LOG_MAIN_LOGGER_ID;
    int32_t s_debug_logger   = LOG_MAIN_LOGGER_ID;
    int32_t s_error_logger   = LOG_MAIN_LOGGER_ID;
    int32_t s_message_logger = LOG_MAIN_LOGGER_ID;
    int32_t s_warning_logger = LOG_MAIN_LOGGER_ID;
    int32_t s_fatal_logger   = LOG_MAIN_LOGGER_ID;
    int32_t s_lua_logger     = LOG_MAIN_LOGGER_ID;

    int32_t s_ai_logger    = LOG_MAIN_LOGGER_ID;
    int32_t s_login_logger = LOG_MAIN_LOGGER_ID;
    int32_t s_gm_logger    = LOG_MAIN_LOGGER_ID;

    thread_local int32_t s_monitor_logger = LOG_MAIN_LOGGER_ID;

    int32_t s_aidetail_logger    = LOG_LEVEL_DEBUG;
    int32_t s_actordetail_logger = LOG_LEVEL_DEBUG;
    int32_t s_skilldetail_logger = LOG_LEVEL_DEBUG;
} // namespace BaseCode

static char g_logPath[1024] = "./log";
static bool g_log_start     = false;
void        BaseCode::InitMonitorLog(const std::string& logname)
{
    BaseCode::s_monitor_logger =
        CLogManager::getPtr()->createLogger({.name = logname, .path = std::string(g_logPath) + "/monitor", .fileLine = false, .flush = true});
    LOGDEBUG("InitMonitor:{}", logname);
}

namespace
{
    std::tuple<FILE*, struct tm> logFileOpen(const char* pszName, bool logData)
    {
        __ENTER_FUNCTION
        if(!pszName)
            return {};
        auto        curTime     = std::chrono::system_clock::now();
        std::time_t now_c       = std::chrono::system_clock::to_time_t(curTime);
        auto        localtime_c = timeToLocalTime(now_c);

        std::string szLogName;
        if(logData)
            szLogName = attempt_format("{}/{}_{:%Y-%m-%d}.log", g_logPath, pszName, localtime_c);
        else
            szLogName = attempt_format("{}/{}.log", g_logPath, pszName);

        FILE* fp = fopen(szLogName.c_str(), "a+");
        return {fp, localtime_c};
        __LEAVE_FUNCTION_NO_LOG
        return {};
    }
} // namespace

void BaseCode::MyLogMsgX(const char* pszName, bool logData, const char* pszBuffer)
{
    if(!pszBuffer)
        return;
    auto [fp, localtime_c] = logFileOpen(pszName, logData);
    if(fp == nullptr)
        return;

    fmt::print(fp, "{:%H:%M:%S}[{}]{:s}\n", localtime_c, getNdcStr(), pszBuffer);

    fclose(fp);
}

void BaseCode::MyLogMsgX(const char* pszName, bool logData, const char* pSrcFile, int32_t line, const char* pszBuffer)
{
    auto [fp, localtime_c] = logFileOpen(pszName, logData);
    if(fp == nullptr)
        return;

    fmt::print(fp, "{:%H:%M:%S}[{}]{:s}[{}:{}]\n", localtime_c, getNdcStr(), pszBuffer, pSrcFile, line);

    fclose(fp);
}

int32_t getLogDefaultLev(const char* logger_name)
{
    __ENTER_FUNCTION
    auto pSetting = GetGlobalSetting();
    CHECK_RET(pSetting, LOG_LEVEL_FATAL);
    const auto& json        = pSetting->GetData();
    const auto& log_setting = json["debug"]["log_default_lev"];

    std::string value  = log_setting[logger_name];
    auto        result = magic_enum::enum_cast<ENUM_LOG_LEVEL>(value);
    if(result)
    {
        auto log_lev = enum_to(result.value());
        return log_lev;
    }
    __LEAVE_FUNCTION_NO_LOG
    return LOG_LEVEL_FATAL;
}

void BaseCode::InitLog(const std::string& path)
{
    __ENTER_FUNCTION
    createRecursionDir(path);
    CLogManager::getPtr()->enableLogger(LOG_MAIN_LOGGER_ID, false);

    BaseCode::s_debug_logger   = CLogManager::getPtr()->createLogger({.name = "debug", .path = path, .level = getLogDefaultLev("debug")});
    BaseCode::s_error_logger   = CLogManager::getPtr()->createLogger({.name = "error", .path = path, .level = getLogDefaultLev("error")});
    BaseCode::s_message_logger = CLogManager::getPtr()->createLogger({.name = "message", .path = path, .level = getLogDefaultLev("message")});
    BaseCode::s_warning_logger = CLogManager::getPtr()->createLogger({.name = "warning", .path = path, .level = getLogDefaultLev("warning")});
    BaseCode::s_fatal_logger   = CLogManager::getPtr()->createLogger({.name = "fatal", .path = path, .level = getLogDefaultLev("fatal")});

    BaseCode::s_network_logger = CLogManager::getPtr()->createLogger({.name = "network", .path = path, .level = getLogDefaultLev("network")});
    BaseCode::s_db_logger      = CLogManager::getPtr()->createLogger({.name = "db", .path = path, .level = getLogDefaultLev("db")});
    BaseCode::s_stack_logger   = CLogManager::getPtr()->createLogger({
        .name     = "stack",
        .path     = path,
        .level    = getLogDefaultLev("stack"),
        .fileLine = false,
        .flush    = true,
    });
    BaseCode::s_lua_logger     = CLogManager::getPtr()->createLogger({
        .name     = "lua",
        .path     = path,
        .level    = getLogDefaultLev("lua"),
        .fileLine = false,
        .flush    = true,
    });
    BaseCode::s_ai_logger      = CLogManager::getPtr()->createLogger({.name = "ai", .path = path, .level = getLogDefaultLev("ai")});
    BaseCode::s_login_logger   = CLogManager::getPtr()->createLogger({.name = "login", .path = path, .level = getLogDefaultLev("login")});
    BaseCode::s_gm_logger = CLogManager::getPtr()->createLogger({.name = "gm", .path = path, .level = getLogDefaultLev("gm"), .fileLine = false});

    BaseCode::s_aidetail_logger = CLogManager::getPtr()->createLogger(
        {.name = "aidetail", .path = path + "/aidetail", .level = getLogDefaultLev("aidetail"), .fileLine = false, .detail_file = true});
    BaseCode::s_actordetail_logger = CLogManager::getPtr()->createLogger(
        {.name = "actordetail", .path = path + "/actordetail", .level = getLogDefaultLev("actordetail"), .fileLine = false, .detail_file = true});
    BaseCode::s_skilldetail_logger = CLogManager::getPtr()->createLogger(
        {.name = "skilldetail", .path = path + "/skilldetail", .level = getLogDefaultLev("skilldetail"), .fileLine = false, .detail_file = true});

    if(path.empty() == false)
    {
        strcpy(g_logPath, path.c_str());
    }

    CLogManager::getPtr()->start();
    g_log_start = true;
    __LEAVE_FUNCTION_NO_LOG
}

void BaseCode::SetLogLev(int log_lev)
{
    __ENTER_FUNCTION
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_debug_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_error_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_message_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_warning_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_fatal_logger, log_lev);

    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_network_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_db_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_stack_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_lua_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_ai_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_login_logger, log_lev);
    CLogManager::getPtr()->setLoggerLevel(BaseCode::s_gm_logger, log_lev);
    __LEAVE_FUNCTION_NO_LOG
}

void BaseCode::StopLog()
{
    __ENTER_FUNCTION
    const char* end_line = "======================================================================";
    uint32_t    max_log  = CLogManager::getPtr()->getStatusActiveLoggers();
    for(uint32_t log_id = 0; log_id < max_log; log_id++)
    {
        if(CLogManager::getPtr()->getLoggerLogCount(log_id) > 0)
        {
            ZLOGFMT_DEBUG(log_id, end_line);
        }
    }

    CLogManager::getPtr()->stop();
    g_log_start = false;
    __LEAVE_FUNCTION_NO_LOG
}

bool BaseCode::IsLogRunning()
{
    return g_log_start;
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
    return (this_thread_NDC) ? this_thread_NDC->ndc : std::to_string(get_cur_thread_id());
}