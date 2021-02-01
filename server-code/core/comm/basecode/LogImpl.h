#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <errno.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <stdio.h>

#include "LockfreeQueue.h"
#include "ObjectHeap.h"
#include "Singleton.h"

using LoggerID                              = int32_t;
constexpr LoggerID    LOG_INVALID_LOGGER_ID = -1;
constexpr LoggerID    LOG_MAIN_LOGGER_ID    = 0;
constexpr const char* LOG_MAIN_LOGGER_KEY   = "Main";
//! LOG Level
enum ENUM_LOG_LEVEL
{
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_ALARM,
    LOG_LEVEL_FATAL,
};

constexpr int32_t LOG_LOGGER_MAX = 1024;

struct LogData
{
    LoggerID    id;      // dest logger id
    int32_t     level;   // log level
    time_t      time;    // create time
    uint32_t    precise; // create time
    const char* file;
    int32_t     line;
    uint64_t    detail;
    std::string ndc;     // ndc
    std::string content; // content

    OBJECTHEAP_DECLARATION(s_heap);
};
using LogDataPtr = std::shared_ptr<LogData>;

struct LoggerConfig
{
    //! attribute
    std::string name;                     // one logger one name.
    std::string path   = "./log";         // path for log file.
    bool        enable = true;            // logger is enable
    int32_t     level  = LOG_LEVEL_DEBUG; // filter level

    bool     fileLine    = true;  // enable/disable the log's suffix.(file name:line number)
    bool     display     = false; // display to screen
    bool     outfile     = true;  // output to file
    bool     monthdir    = false; // create directory per month
    bool     detail_file = false;
    bool     flush       = false;
    uint32_t limitsize   = 800; // limit file's size, unit Million byte.
};

struct LoggerInfo
{
    std::atomic<bool>     enable;       // logger is enable
    std::atomic<int32_t>  level;        // filter level
    std::atomic<uint64_t> logCount = 0; // output_line_count;

    //! file info
    time_t   curFileCreateTime = 0; // file create time
    uint32_t curFileIndex;
    uint64_t curWriteLen = 0; // current file length

    std::unique_ptr<FILE, decltype(&std::fclose)> File{nullptr, nullptr}; // file handle.
};

class CWorkerThread;
class CLogManager
{
    CLogManager();

public:
    ~CLogManager();
    static CLogManager*        getInstance();
    inline static CLogManager& getRef() { return *getInstance(); }
    inline static CLogManager* getPtr() { return getInstance(); }
    LoggerID                   createLogger(const LoggerConfig& cfg);

    bool start();
    bool stop();
    bool isRunning() const;

    bool enableLogger(LoggerID id, bool enable);
    bool setLoggerLevel(LoggerID id, int32_t nLevel);

    uint64_t getLoggerLogCount(LoggerID id) const;

    //! Log4z status statistics, thread safe.
    bool     isLoggerEnable(LoggerID id) const;
    uint64_t getStatusTotalWriteCount() const { return m_TotalWriteFileCount; }
    uint64_t getStatusTotalWriteBytes() const { return m_TotalWriteFileBytes; }
    uint64_t getStatusTotalPushQueue() const { return m_TotalPushLog; }
    uint64_t getStatusTotalPopQueue() const { return m_TotalPopLog; }
    uint32_t getStatusActiveLoggers() const { return m_logCount; }

    bool       prePushLog(LoggerID id, int32_t level);
    bool       pushLog(const LogDataPtr& pLog);
    LogDataPtr makeLogData(LoggerID id, int32_t level, uint64_t detail = 0, const char* file = NULL, int32_t line = 0);

private:
    LoggerID findLogger(const std::string& name) const;
    void     writeLog(const LogDataPtr& pLog);

private:
    std::atomic<LoggerID> m_logCount;

    std::mutex                                m_mutex;
    std::unordered_map<std::string, LoggerID> m_mapLoggerName;

    std::array<LoggerConfig, LOG_LOGGER_MAX> m_logcfg;
    std::array<LoggerInfo, LOG_LOGGER_MAX>   m_loginfo;

    MPSCQueue<LogData*>            m_logqueue;
    std::unique_ptr<CWorkerThread> m_logThread;
    std::atomic<uint64_t>          m_TotalWriteFileCount;
    std::atomic<uint64_t>          m_TotalWriteFileBytes;

    // Log queue statistics
    std::atomic<uint64_t> m_TotalPushLog;
    std::atomic<uint64_t> m_TotalPopLog;
};

//! base macro.
template<class ... Args>
inline void ZLOG_STREAM(uint32_t id, uint32_t level, const char* file, uint32_t line, const std::string& log)                                                                                                               
{
    if(CLogManager::getPtr()->prePushLog(id, level) == false)
        return;

    auto __pLog = CLogManager::getPtr()->makeLogData(id, level, 0, file, line);
    if(__pLog == nullptr)
        return;
    __pLog->content = log;
    CLogManager::getPtr()->pushLog(__pLog);
} 

//! fast macro
#define ZLOG_TRACE(id, log) ZLOG_STREAM(id, LOG_LEVEL_TRACE, __FILE_NAME__, __LINE__, log)
#define ZLOG_DEBUG(id, log) ZLOG_STREAM(id, LOG_LEVEL_DEBUG, __FILE_NAME__, __LINE__, log)
#define ZLOG_INFO(id, log)  ZLOG_STREAM(id, LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, log)
#define ZLOG_WARN(id, log)  ZLOG_STREAM(id, LOG_LEVEL_WARN, __FILE_NAME__, __LINE__, log)
#define ZLOG_ERROR(id, log) ZLOG_STREAM(id, LOG_LEVEL_ERROR, __FILE_NAME__, __LINE__, log)
#define ZLOG_ALARM(id, log) ZLOG_STREAM(id, LOG_LEVEL_ALARM, __FILE_NAME__, __LINE__, log)
#define ZLOG_FATAL(id, log) ZLOG_STREAM(id, LOG_LEVEL_FATAL, __FILE_NAME__, __LINE__, log)

//! super macro.
#define ZLOGT(log) ZLOG_TRACE(LOG_MAIN_LOGGER_ID, log)
#define ZLOGD(log) ZLOG_DEBUG(LOG_MAIN_LOGGER_ID, log)
#define ZLOGI(log) ZLOG_INFO(LOG_MAIN_LOGGER_ID, log)
#define ZLOGW(log) ZLOG_WARN(LOG_MAIN_LOGGER_ID, log)
#define ZLOGE(log) ZLOG_ERROR(LOG_MAIN_LOGGER_ID, log)
#define ZLOGA(log) ZLOG_ALARM(LOG_MAIN_LOGGER_ID, log)
#define ZLOGF(log) ZLOG_FATAL(LOG_MAIN_LOGGER_ID, log)

//! format input log.
template<class ... Args>
inline void ZLOG_FORMAT(uint32_t id,uint32_t level,const char* file,uint32_t line, const std::string& logformat, Args&& ... args)
{
    if(CLogManager::getPtr()->prePushLog(id, level) == false)
        return;

    auto __pLog = CLogManager::getPtr()->makeLogData(id, level, 0, file, line);
    if(__pLog == nullptr)
        return;

    if constexpr(sizeof...(args) == 0)
    {
        __pLog->content = logformat;
        CLogManager::getPtr()->pushLog(__pLog);
    }
    else
    {
        try
        {
            __pLog->content = fmt::format(logformat, std::forward<Args>(args)...);
            CLogManager::getPtr()->pushLog(__pLog);
        }
        catch(fmt::format_error& e)
        {
            __pLog->content = fmt::format("format_error:{} fmt:{}", e.what(), logformat);
            CLogManager::getPtr()->pushLog(__pLog);
        }
    }
}

template<class ... Args>
inline void ZLOG_FORMAT_DETAIL(bool expr, uint32_t id, uint32_t level, uint64_t detail_id, const char* file,uint32_t line, const std::string& logformat, Args&& ... args)
{
    if(expr == false)
        return;
    if(CLogManager::getPtr()->prePushLog(id, level) == false)
        return;

    auto __pLog = CLogManager::getPtr()->makeLogData(id, level, detail_id, file, line);
    if(__pLog == nullptr)
        return;

    if constexpr(sizeof...(args) == 0)
    {
        __pLog->content = logformat;
        CLogManager::getPtr()->pushLog(__pLog);
    }
    else
    {
        try
        {
            __pLog->content = fmt::format(logformat, std::forward<Args>(args)...);
            CLogManager::getPtr()->pushLog(__pLog);
        }
        catch(fmt::format_error& e)
        {
            __pLog->content = fmt::format("format_error:{} fmt:{}", e.what(), logformat);
            CLogManager::getPtr()->pushLog(__pLog);
        }
    }
}


//! format string
#define ZLOGFMT_TRACE(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_TRACE, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_DEBUG(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_DEBUG, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_INFO(id, fmt, ...)  ZLOG_FORMAT(id, LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_WARN(id, fmt, ...)  ZLOG_FORMAT(id, LOG_LEVEL_WARN, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_ERROR(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_ERROR, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_ALARM(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_ALARM, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_FATAL(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_FATAL, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMTT(fmt, ...)          ZLOGFMT_TRACE(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTD(fmt, ...)          ZLOGFMT_DEBUG(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTI(fmt, ...)          ZLOGFMT_INFO(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTW(fmt, ...)          ZLOGFMT_WARN(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTE(fmt, ...)          ZLOGFMT_ERROR(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTA(fmt, ...)          ZLOGFMT_ALARM(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTF(fmt, ...)          ZLOGFMT_FATAL(LOG_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)

#endif /* LOGMANAGER_H */
