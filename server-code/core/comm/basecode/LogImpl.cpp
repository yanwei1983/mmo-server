#include "LogImpl.h"

#include "FileUtil.h"
#include "Thread.h"
#include "TimeUtil.h"
#include <fmt/format.h>
#include <fmt/chrono.h>

OBJECTHEAP_IMPLEMENTATION(LogData, s_heap);

constexpr const char* LOG_STRING[] = {
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "ALARM",
    "FATAL",
};

CLogManager*      CLogManager::getInstance()
{
    static CLogManager m;
    return &m;
}

CLogManager::CLogManager()
{
    LoggerConfig main_cfg;
    main_cfg.name = LOG_MAIN_LOGGER_KEY;

    createLogger(main_cfg);
    m_logThread = std::make_unique<CWorkerThread>("log_thread", true);
}

CLogManager::~CLogManager() {}

LoggerID CLogManager::createLogger(const LoggerConfig& cfg)
{
    __ENTER_FUNCTION
    auto logID = findLogger(cfg.name);
    if(logID != LOG_INVALID_LOGGER_ID)
        return logID;
    std::lock_guard lock(m_mutex);
    logID = m_logCount;
    m_mapLoggerName.emplace(cfg.name, logID);
    m_logcfg[logID]         = cfg;
    m_loginfo[logID].enable = m_logcfg[logID].enable;
    m_loginfo[logID].level  = m_logcfg[logID].level;

    m_logCount = logID + 1;
    return logID;
    __LEAVE_FUNCTION_NO_LOG
    return LOG_INVALID_LOGGER_ID;
}

LoggerID CLogManager::findLogger(const std::string& name) const
{
    __ENTER_FUNCTION
    auto it = m_mapLoggerName.find(name);
    if(it != m_mapLoggerName.end())
        return it->second;
    __LEAVE_FUNCTION_NO_LOG
    return LOG_INVALID_LOGGER_ID;
}

bool CLogManager::isRunning() const
{
    __ENTER_FUNCTION
    if(m_logThread)
        return m_logThread->IsRunning();
    __LEAVE_FUNCTION_NO_LOG
    return false;
}
bool CLogManager::enableLogger(LoggerID id, bool enable)
{
    __ENTER_FUNCTION
    if(id < 0 || id >= m_logCount)
        return false;
    m_loginfo[id].enable = enable;
    return true;
    __LEAVE_FUNCTION_NO_LOG
    return false;
}
bool CLogManager::setLoggerLevel(LoggerID id, int32_t nLevel)
{
    __ENTER_FUNCTION
    if(id < 0 || id >= m_logCount)
        return false;
    m_loginfo[id].level = nLevel;
    return true;
    __LEAVE_FUNCTION_NO_LOG
    return false;
}

uint64_t CLogManager::getLoggerLogCount(LoggerID id) const
{
    __ENTER_FUNCTION
    if(id < 0 || id >= m_logCount)
        return false;
    return m_loginfo[id].logCount;
    __LEAVE_FUNCTION_NO_LOG
    return false;
}

//! Log4z status statistics, thread safe.
bool CLogManager::isLoggerEnable(LoggerID id) const
{
    __ENTER_FUNCTION
    return m_loginfo[id].enable;
    __LEAVE_FUNCTION_NO_LOG
    return false;
}
bool CLogManager::start()
{
    __ENTER_FUNCTION
    if(m_logThread == nullptr)
        return false;
    if(m_logThread->IsRunning())
        return false;

    m_logThread->Start();
    return true;
    __LEAVE_FUNCTION_NO_LOG
    return false;
}

bool CLogManager::stop()
{
    __ENTER_FUNCTION
    if(m_logThread)
    {
        m_logThread->Stop();
        m_logThread->Join(true);
        m_logThread.reset();
        return true;
    }

    __LEAVE_FUNCTION_NO_LOG
    return false;
}

bool CLogManager::prePushLog(LoggerID id, int32_t level)
{
    __ENTER_FUNCTION
    if(id < 0 || id >= m_logCount || m_loginfo[id].enable == false)
    {
        return false;
    }

    if(level < m_loginfo[id].level)
    {
        return false;
    }

    return true;
    __LEAVE_FUNCTION_NO_LOG
    return false;
}

namespace
{

    void openLoggerFile(const LogDataPtr& pLog, LoggerInfo& info, const LoggerConfig& cfg)
    {
        __ENTER_FUNCTION
        bool sameday       = CheckSameDay(pLog->time, info.curFileCreateTime);
        bool needChageFile = info.curWriteLen > cfg.limitsize * 1024 * 1024;
        if(!sameday || needChageFile)
        {
            if(!sameday)
            {
                info.curFileIndex = 0;
            }
            else
            {
                info.curFileIndex++;
            }
            if(info.File)
            {
                info.File.reset();
            }
        }

        if(info.File)
            return;

        info.curFileCreateTime = pLog->time;
        info.curWriteLen       = 0;

        tm          t = timeToLocalTime(info.curFileCreateTime);
        std::string name;
        std::string path;

        name = cfg.name;
        path = cfg.path;

        if(cfg.monthdir)
        {
            path += fmt::format("{:%Y_/%m/}", t);
        }

        if(!isDirectory(path))
        {
            createRecursionDir(path);
        }

        path += fmt::format("/{}_{:%Y%m%d}_{:03}.log", name, t, info.curFileIndex);
        using file_ptr_type = decltype(info.File);
        info.File           = file_ptr_type{fopen(path.c_str(), "a+"), &std::fclose};

        __LEAVE_FUNCTION_NO_LOG
    }

    FILE* openLoggerDetailFile(const LogDataPtr& pLog, LoggerInfo& info, const LoggerConfig& cfg)
    {
        __ENTER_FUNCTION
        tm          t    = timeToLocalTime(pLog->time);
        std::string path = cfg.path;
        if(!isDirectory(path))
        {
            createRecursionDir(path);
        }
        path += fmt::format("/{}.log", pLog->detail);
        return std::fopen(path.c_str(), "a+");

        __LEAVE_FUNCTION_NO_LOG
        return nullptr;
    }

}; // namespace

bool CLogManager::pushLog(const LogDataPtr& pLog)
{
    __ENTER_FUNCTION
    if(pLog->id < 0 || pLog->id >= m_logCount || m_loginfo[pLog->id].enable == false || pLog->level < m_loginfo[pLog->id].level ||
       isRunning() == false)
    {
        return false;
    }

    m_logThread->AddJob([this, pLog]() { writeLog(pLog); });
    m_TotalPushLog++;

    return true;

    __LEAVE_FUNCTION_NO_LOG
    return false;
}

void CLogManager::writeLog(const LogDataPtr& pLog)
{
    __ENTER_FUNCTION
    if(pLog->id < 0 || pLog->id >= m_logCount)
    {
        return;
    }
    m_TotalPopLog++;
    auto&       curLogger = m_loginfo[pLog->id];
    const auto& cfg       = m_logcfg[pLog->id];
    if(curLogger.enable == false || pLog->level < curLogger.level)
    {
        return;
    }

    tm          tt          = timeToLocalTime(pLog->time);
    std::string loglev_name = (pLog->level < sizeOfArray(LOG_STRING)) ? LOG_STRING[pLog->level] : "";
    std::string fileline;
    if(cfg.fileLine && pLog->file)
    {
        try
        {
            fileline = fmt::format("[{}:{}]", pLog->file, pLog->line);
        }
        catch(fmt::format_error& e)
        {
            fileline = fmt::format("format_line_error:{}", e.what());
        }
    }
    std::string output_txt;
    try
    {
        output_txt = fmt::format("{:%H:%M:%S}.{:03} {}[{}] {}{}\r\n", tt, pLog->precise, loglev_name, pLog->ndc, pLog->content, fileline);
    }
    catch(fmt::format_error& e)
    {
        output_txt = fmt::format("format_error:{}\r\n", e.what());
    }

    if(cfg.outfile)
    {
        if(cfg.detail_file == false)
        {
            openLoggerFile(pLog, curLogger, cfg);
            if(curLogger.File)
            {
                fwrite(output_txt.c_str(), sizeof(char), output_txt.size(), curLogger.File.get());
                if(cfg.flush)
                {
                    fflush(curLogger.File.get());
                }
                curLogger.curWriteLen += (uint32_t)output_txt.size();
                curLogger.logCount++;
                m_TotalWriteFileCount++;
                m_TotalWriteFileBytes += output_txt.size();
            }
        }
        else
        {
            FILE* pDetailFile = openLoggerDetailFile(pLog, curLogger, cfg);
            if(pDetailFile)
            {
                fwrite(output_txt.c_str(), sizeof(char), output_txt.size(), pDetailFile);
                fclose(pDetailFile);
                m_TotalWriteFileCount++;
                m_TotalWriteFileBytes += output_txt.size();
            }
        }
    }

    if(cfg.display)
    {
        fmt::print(std::cout, output_txt);
    }
    __LEAVE_FUNCTION_NO_LOG
}

LogDataPtr CLogManager::makeLogData(LoggerID id, int32_t level, uint64_t detail, const char* file, int32_t line)
{
    __ENTER_FUNCTION
    auto now = std::chrono::high_resolution_clock::now();

    auto sec = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto ms  = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    //std::chrono::duration<float> ms_float = ms - sec;
    auto ms_diff = ms - sec;
    
    NDC* pNdc = BaseCode::getNdc();
    return LogDataPtr(new LogData {.id      = id,
                                  .level   = level,
                                  .time    = sec.time_since_epoch().count(),
                                  .precise = static_cast<uint32_t>(ms_diff.count()),
                                  .file    = file,
                                  .line    = line,
                                  .detail  = detail,
                                  .ndc     = (pNdc) ? pNdc->ndc : std::string()});
    __LEAVE_FUNCTION_NO_LOG
    return nullptr;
}
