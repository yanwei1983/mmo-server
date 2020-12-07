/*
 * Log4z License
 * -----------
 *
 * Log4z is licensed under the terms of the MIT license reproduced below.
 * This means that Log4z is free software and can be used for both academic
 * and commercial purposes at absolutely no cost.
 *
 *
 * ===============================================================================
 *
 * Copyright (C) 2010-2016 YaweiZhang <yawei.zhang@foxmail.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ===============================================================================
 *
 * (end of COPYRIGHT)
 */

/*
 * AUTHORS:  YaweiZhang <yawei.zhang@foxmail.com>
 * VERSION:  3.3.0
 * PURPOSE:  A lightweight library for error reporting and logging to file and screen .
 * CREATION: 2010.10.4
 * LCHANGE:  2016.02.29
 * LICENSE:  Expat/MIT License, See Copyright Notice at the begin of this file.
 */

/*
 * contact me:
 * tencent qq group: 19811947
 * mail: yawei.zhang@foxmail.com
 */

/*
 * UPDATES LOG
 *
 * VERSION 0.1.0 <DATE: 2010.10.4>
 *     create the first project.
 *     It support put log to screen and files,
 *     support log level, support one day one log file.
 *     support multi-thread, cross-platform.
 *
 * VERSION .... <DATE: ...>
 *     ...
 *
 * VERSION 0.9.0 <DATE: 2012.12.24>
 *     support config files.
 *     support color text in screen.
 *     support multiple output to different files.
 *
 * VERSION 1.0.0 <DATE: 2012.12.29>
 *     support comments in the config file.
 *     add a advanced demo in the ./project
 *     fix some details.
 *
 * VERSION 1.0.1 <DATE: 2013.01.01>
 *     change and add some Comments in the log4z
 *     simplify the 'fast_test' demo projects.
 *
 * VERSION 1.1.0 <DATE: 2013.01.24>
 *     the method Start will wait for the logger thread started.
 *     config and add method change.
 *     namespace change.
 *
 * VERSION 1.1.1 <DATE: 2013.02.23>
 *     add status info method.
 *     optimize.
 *
 * VERSION 1.2.0 <DATE: 2013.04.05>
 *     add stress test demo
 *  rewrite Stream module,better performance.
 *
 * VERSION 1.2.1 <DATE: 2013.04.13>
 *     fixed type name 'long' stream format on 64/32 operation system.
 *  logger will not loss any log on process normal exit.
 *
 * VERSION 2.0.0 <DATE: 2013.04.25>
 *     new interface:
 *      merge some Main interface and Dynamic interface
 *      add Change Logger Attribute method by thread-safe
 *     new config design.
 *     log file name append process id.
 *
 * VERSION 2.1 <DATE: 2013.05.22>
 *     support binary text output
 *  rewrite write file module, support vs2005 open Chinese characters path
 *
 * VERSION 2.2 <DATE: 2013.07.08>
 *    optimized binary stream output view
 *     support wchar * string.
 *
 * VERSION 2.3 <DATE: 2013.08.29>
 *  adjust output file named.
 *  support different month different directory.
 *  adjust some detail.
 *
 * VERSION 2.4 <DATE: 2013.10.07>
 *  support rolling log file.
 *  support hot update configure
 *  used precision time in log.
 *  micro set default logger attribute
 *  fix tls bug in windows xp dll
 *
 * VERSION 2.5 <DATE: 2014.03.25>
 *  screen output can choice synchronous or not
 *  fix sometimes color will disorder on windows.
 *  eliminate some compiler warning
 *  fix sem_timewait in linux
 *  add format-style method at input log, cannot support vs2003 and VC6.
 *  fix WCHAR String cannot output
 *  optimize std::string, binary log input, and support std::wstring.
 *  clean code, better readability
 *
 * VERSION 2.6 <DATE: 2014.08.19>
 *  add PrePushLog
 *  better performance when log is filter out.
 *  interface replace std::string because it's in shared library is unsafe.
 *  add log level 'trace'
 *
 * VERSION 2.6.1 <DATE: 2014.08.22>
 *  fix bug from defined _MSC_VER
 *
 * VERSION 2.7 <DATE: 2014.09.21>
 *  compatible mac machine,  now  log4z can working in linux/windows/mac.
 *
 * VERSION 2.8 <DATE: 2014.09.27>
 *  support synchronous written to file and thread-safe
 *  fix compatibility on MinGW. a constant value suffix.
 *  ignore utf-8 file BOM when load configure file
 *  use macro WIN32_LEAN_AND_MEAN replace head file winsock2.h
 *  new naming notations
 *
 * VERSION 3.0 <DATE: 2014.12.19>
 *  new naming notations
 *  support for reading config from a string.
 *  remove all TLS code, used dispatch_semaphore in apple OS.
 *  support system: windows, linux, mac, iOS
 *
 * VERSION 3.1 <DATE: 2014.12.19>
 *  add method enable/disable logger by specific logger id
 *  add method enable/disable log suffix line number.
 *  add method enable/disable log output to file.
 *
 * VERSION 3.2 <DATE: 2014.12.19>
 *  add interface setLoggerName,setLoggerPath,setAutoUpdate
 *  support auto update from configure file
 *
 * VERSION 3.3 <DATE: 2014.12.19>
 *  support map vector list
 *  support continuum via travis.
 *  new hot change design, all change realize need via push log flow.
 *  support oem string convert.
 *  new method to read whole content of file.
 *  check configure's checksum when auto update it.
 *  some other optimize.
 *
 */

#ifndef _ZSUMMER_LOG4Z_H_
#define _ZSUMMER_LOG4Z_H_
#include <string>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "ObjectHeap.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <vector>

#include <fmt/format.h>
#include <fmt/printf.h>
//! logger ID type. DO NOT TOUCH
typedef int32_t LoggerId;

//! the invalid logger id. DO NOT TOUCH
constexpr int32_t LOG4Z_INVALID_LOGGER_ID = -1;

//! the main logger id. DO NOT TOUCH
//! can use this id to set the main logger's attribute.
//! example:
//! ILog4zManager::getPtr()->setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_WARN);
//! ILog4zManager::getPtr()->setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);
constexpr int32_t LOG4Z_MAIN_LOGGER_ID = 0;

//! the main logger name. DO NOT TOUCH
constexpr const char* LOG4Z_MAIN_LOGGER_KEY = "Main";

//! check VC VERSION. DO NOT TOUCH
//! format micro cannot support VC6 or VS2003, please use stream input log, like LOGI, LOGD, LOG_DEBUG, LOG_STREAM ...
#if _MSC_VER >= 1400 // MSVC >= VS2005
#define LOG4Z_FORMAT_INPUT_ENABLE
#endif

#ifndef WIN32
#define LOG4Z_FORMAT_INPUT_ENABLE
#endif

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

//////////////////////////////////////////////////////////////////////////
//! -----------------default logger config, can change on this.-----------
//////////////////////////////////////////////////////////////////////////
//! the max logger count.
constexpr int32_t LOG4Z_LOGGER_MAX = 1024;
//! the max log content length.
constexpr int32_t LOG4Z_LOG_BUF_SIZE = 1024 * 8;
//! the max stl container depth.
constexpr int32_t LOG4Z_LOG_CONTAINER_DEPTH = 5;
//! the log queue length limit size.
constexpr int32_t LOG4Z_LOG_QUEUE_LIMIT_SIZE = 20000;

//! all logger synchronous output or not
constexpr bool LOG4Z_ALL_SYNCHRONOUS_OUTPUT = false;
//! all logger synchronous display to the windows debug output
constexpr bool LOG4Z_ALL_DEBUGOUTPUT_DISPLAY = false;

//! default logger output file.
constexpr const char* LOG4Z_DEFAULT_PATH = "./log/";
//! default log filter level
constexpr int32_t LOG4Z_DEFAULT_LEVEL = LOG_LEVEL_DEBUG;
//! default logger display
constexpr bool LOG4Z_DEFAULT_DISPLAY = true;
//! default logger output to file
constexpr bool LOG4Z_DEFAULT_OUTFILE = true;
//! default logger month dir used status
constexpr bool LOG4Z_DEFAULT_MONTHDIR = false;
//! default logger output file limit size, unit M byte.
constexpr int32_t LOG4Z_DEFAULT_LIMITSIZE = 2000;
//! default logger show suffix (file name and line number)
constexpr bool LOG4Z_DEFAULT_SHOWSUFFIX = true;
//! support ANSI->OEM console conversion on Windows
#undef LOG4Z_OEM_CONSOLE
//! default logger force reserve log file count.
constexpr size_t LOG4Z_FORCE_RESERVE_FILE_COUNT = 7;

///////////////////////////////////////////////////////////////////////////
//! -----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#ifndef _ZSUMMER_BEGIN
#define _ZSUMMER_BEGIN \
    namespace zsummer  \
    {
#endif
#ifndef _ZSUMMER_LOG4Z_BEGIN
#define _ZSUMMER_LOG4Z_BEGIN \
    namespace log4z          \
    {
#endif
_ZSUMMER_BEGIN
_ZSUMMER_LOG4Z_BEGIN

struct LogData
{
    LoggerId _id;   // dest logger id
    int32_t  _type; // type.
    int32_t  _typeval;
    int32_t  _level;   // log level
    time_t   _time;    // create time
    uint32_t _precise; // create time

    std::string _content; // content

    OBJECTHEAP_DECLARATION(s_heap);
};

//! log4z class
class ILog4zManager
{
public:
    ILog4zManager(){};
    virtual ~ILog4zManager(){};

    //! Log4z Singleton

    static ILog4zManager*        getInstance();
    inline static ILog4zManager& getRef() { return *getInstance(); }
    inline static ILog4zManager* getPtr() { return getInstance(); }

    //! Config or overwrite configure
    //! Needs to be called before ILog4zManager::Start,, OR Do not call.
    virtual bool config(const char* configPath)              = 0;
    virtual bool configFromString(const char* configContent) = 0;

    //! Create or overwrite logger.
    //! Needs to be called before ILog4zManager::Start, OR Do not call.
    virtual LoggerId createLogger(const char* key) = 0;

    //! Start Log Thread. This method can only be called once by one process.
    virtual bool start() = 0;

    //! Default the method will be calling at process exit auto.
    //! Default no need to call and no recommended.
    virtual bool stop() = 0;

    virtual bool isRunning() = 0;

    //! Find logger. thread safe.
    virtual LoggerId findLogger(const char* key) = 0;

    // pre-check the log filter. if filter out return false.
    virtual bool prePushLog(LoggerId id, int32_t level) = 0;
    //! Push log, thread safe.
    virtual bool pushLog(LogData* pLog, const char* file = NULL, int32_t line = 0) = 0;

    //! set logger's attribute, thread safe.
    virtual bool     enableLogger(LoggerId id, bool enable)              = 0; // immediately when enable, and queue up when disable.
    virtual bool     setLoggerName(LoggerId id, const char* name)        = 0;
    virtual bool     setLoggerPath(LoggerId id, const char* path)        = 0;
    virtual bool     setLoggerLevel(LoggerId id, int32_t nLevel)         = 0; // immediately when enable, and queue up when disable.
    virtual bool     setLoggerFileLine(LoggerId id, bool enable)         = 0;
    virtual bool     setLoggerDisplay(LoggerId id, bool enable)          = 0;
    virtual bool     setLoggerOutFile(LoggerId id, bool enable)          = 0;
    virtual bool     setLoggerLimitsize(LoggerId id, uint32_t limitsize) = 0;
    virtual bool     setLoggerMonthdir(LoggerId id, bool enable)         = 0;
    virtual bool     setLoggerReserveTime(LoggerId id, time_t sec)       = 0;
    virtual uint64_t getLoggerLogCount(LoggerId id)                      = 0;

    //! Update logger's attribute from config file, thread safe.
    virtual bool setAutoUpdate(int32_t interval /*per second, 0 is disable auto update*/) = 0;
    virtual bool updateConfig()                                                           = 0;

    //! Log4z status statistics, thread safe.
    virtual bool     isLoggerEnable(LoggerId id) = 0;
    virtual uint64_t getStatusTotalWriteCount()  = 0;
    virtual uint64_t getStatusTotalWriteBytes()  = 0;
    virtual uint64_t getStatusTotalPushQueue()   = 0;
    virtual uint64_t getStatusTotalPopQueue()    = 0;
    virtual uint32_t getStatusActiveLoggers()    = 0;

    virtual LogData* makeLogData(LoggerId id, int32_t level) = 0;
    virtual void     freeLogData(LogData* log)               = 0;
};

class Log4zStream;
class Log4zBinary;

#ifndef _ZSUMMER_END
#define _ZSUMMER_END }
#endif
#ifndef _ZSUMMER_LOG4Z_END
#define _ZSUMMER_LOG4Z_END }
#endif

_ZSUMMER_LOG4Z_END
_ZSUMMER_END

//! base macro.
#define ZLOG_STREAM(id, level, file, line, log)                                                                \
    do                                                                                                         \
    {                                                                                                          \
        if(zsummer::log4z::ILog4zManager::getPtr()->prePushLog(id, level))                                     \
        {                                                                                                      \
            zsummer::log4z::LogData* __pLog = zsummer::log4z::ILog4zManager::getPtr()->makeLogData(id, level); \
            __pLog->_content += log;                                                                           \
            zsummer::log4z::ILog4zManager::getPtr()->pushLog(__pLog, file, line);                              \
        }                                                                                                      \
    } while(0)

//! fast macro
#define ZLOG_TRACE(id, log) ZLOG_STREAM(id, LOG_LEVEL_TRACE, __FILE__, __LINE__, log)
#define ZLOG_DEBUG(id, log) ZLOG_STREAM(id, LOG_LEVEL_DEBUG, __FILE__, __LINE__, log)
#define ZLOG_INFO(id, log)  ZLOG_STREAM(id, LOG_LEVEL_INFO, __FILE__, __LINE__, log)
#define ZLOG_WARN(id, log)  ZLOG_STREAM(id, LOG_LEVEL_WARN, __FILE__, __LINE__, log)
#define ZLOG_ERROR(id, log) ZLOG_STREAM(id, LOG_LEVEL_ERROR, __FILE__, __LINE__, log)
#define ZLOG_ALARM(id, log) ZLOG_STREAM(id, LOG_LEVEL_ALARM, __FILE__, __LINE__, log)
#define ZLOG_FATAL(id, log) ZLOG_STREAM(id, LOG_LEVEL_FATAL, __FILE__, __LINE__, log)

//! super macro.
#define ZLOGT(log) ZLOG_TRACE(LOG4Z_MAIN_LOGGER_ID, log)
#define ZLOGD(log) ZLOG_DEBUG(LOG4Z_MAIN_LOGGER_ID, log)
#define ZLOGI(log) ZLOG_INFO(LOG4Z_MAIN_LOGGER_ID, log)
#define ZLOGW(log) ZLOG_WARN(LOG4Z_MAIN_LOGGER_ID, log)
#define ZLOGE(log) ZLOG_ERROR(LOG4Z_MAIN_LOGGER_ID, log)
#define ZLOGA(log) ZLOG_ALARM(LOG4Z_MAIN_LOGGER_ID, log)
#define ZLOGF(log) ZLOG_FATAL(LOG4Z_MAIN_LOGGER_ID, log)

//! format input log.
#ifdef LOG4Z_FORMAT_INPUT_ENABLE

#define ZLOG_FORMAT(id, level, file, line, logformat, ...)                                                     \
    do                                                                                                         \
    {                                                                                                          \
        if(zsummer::log4z::ILog4zManager::getPtr()->prePushLog(id, level))                                     \
        {                                                                                                      \
            zsummer::log4z::LogData* __pLog = zsummer::log4z::ILog4zManager::getPtr()->makeLogData(id, level); \
            try                                                                                                \
            {                                                                                                  \
                __pLog->_content += fmt::format(logformat, ##__VA_ARGS__);                                     \
                zsummer::log4z::ILog4zManager::getPtr()->pushLog(__pLog, file, line);                          \
            }                                                                                                  \
            catch(fmt::format_error & e)                                                                       \
            {                                                                                                  \
                __pLog->_content += fmt::format("format_error:%s", e.what());                                  \
                zsummer::log4z::ILog4zManager::getPtr()->pushLog(__pLog, file, line);                          \
            }                                                                                                  \
        }                                                                                                      \
    } while(0)

//! format string
#define ZLOGFMT_TRACE(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_DEBUG(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_INFO(id, fmt, ...)  ZLOG_FORMAT(id, LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_WARN(id, fmt, ...)  ZLOG_FORMAT(id, LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_ERROR(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_ALARM(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_ALARM, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMT_FATAL(id, fmt, ...) ZLOG_FORMAT(id, LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZLOGFMTT(fmt, ...)          ZLOGFMT_TRACE(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTD(fmt, ...)          ZLOGFMT_DEBUG(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTI(fmt, ...)          ZLOGFMT_INFO(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTW(fmt, ...)          ZLOGFMT_WARN(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTE(fmt, ...)          ZLOGFMT_ERROR(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTA(fmt, ...)          ZLOGFMT_ALARM(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#define ZLOGFMTF(fmt, ...)          ZLOGFMT_FATAL(LOG4Z_MAIN_LOGGER_ID, fmt, ##__VA_ARGS__)
#else
inline void empty_log_format_function1(LoggerId id, const char*, ...) {}
inline void empty_log_format_function2(const char*, ...) {}
#define ZLOGFMT_TRACE empty_log_format_function1
#define ZLOGFMT_DEBUG ZLOGFMT_TRACE
#define ZLOGFMT_INFO  ZLOGFMT_TRACE
#define ZLOGFMT_WARN  ZLOGFMT_TRACE
#define ZLOGFMT_ERROR ZLOGFMT_TRACE
#define ZLOGFMT_ALARM ZLOGFMT_TRACE
#define ZLOGFMT_FATAL ZLOGFMT_TRACE
#define ZLOGFMTT      empty_log_format_function2
#define ZLOGFMTD      ZLOGFMTT
#define ZLOGFMTI      ZLOGFMTT
#define ZLOGFMTW      ZLOGFMTT
#define ZLOGFMTE      ZLOGFMTT
#define ZLOGFMTA      ZLOGFMTT
#define ZLOGFMTF      ZLOGFMTT
#endif

_ZSUMMER_BEGIN
_ZSUMMER_LOG4Z_BEGIN

//! optimze from std::stringstream to Log4zStream
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
class Log4zBinary
{
public:
    Log4zBinary(const void* buf, int32_t len)
    {
        _buf = (const char*)buf;
        _len = len;
    }
    const char* _buf;
    int32_t     _len;
};
class Log4zStream
{
public:
    inline Log4zStream(char* buf, int32_t len);
    inline int32_t getCurrentLen() { return (int32_t)(_cur - _begin); }

private:
    template<class T>
    inline Log4zStream& writeData(const char* ft, T t);
    inline Log4zStream& writeLongLong(int64_t t);
    inline Log4zStream& writeULongLong(uint64_t t);
    inline Log4zStream& writePointer(const void* t);
    inline Log4zStream& writeString(const char* t, size_t len);
    inline Log4zStream& writeWString(const wchar_t* t);
    inline Log4zStream& writeBinary(const Log4zBinary& t);

public:
    inline Log4zStream& operator<<(const void* t) { return writePointer(t); }

    inline Log4zStream& operator<<(const char* t) { return writeString(t, strlen(t)); }
#ifdef WIN32
    inline Log4zStream& operator<<(const wchar_t* t) { return writeWString(t); }
#endif
    inline Log4zStream& operator<<(bool t) { return (t ? writeData("%s", "true") : writeData("%s", "false")); }

    inline Log4zStream& operator<<(char t) { return writeData("%c", t); }

    inline Log4zStream& operator<<(unsigned char t) { return writeData("%u", (uint32_t)t); }

    inline Log4zStream& operator<<(int16_t t) { return writeData("%d", (int32_t)t); }

    inline Log4zStream& operator<<(uint16_t t) { return writeData("%u", (uint32_t)t); }

    inline Log4zStream& operator<<(int32_t t) { return writeData("%d", t); }

    inline Log4zStream& operator<<(uint32_t t) { return writeData("%u", t); }

    inline Log4zStream& operator<<(int64_t t) { return writeLongLong(t); }

    inline Log4zStream& operator<<(uint64_t t) { return writeULongLong(t); }

    inline Log4zStream& operator<<(float t) { return writeData("%.4f", t); }

    inline Log4zStream& operator<<(double t) { return writeData("%.4lf", t); }

    template<class _Elem, class _Traits, class _Alloc> // support std::string, std::wstring
    inline Log4zStream& operator<<(const std::basic_string<_Elem, _Traits, _Alloc>& t)
    {
        return writeString(t.c_str(), t.length());
    }

    inline Log4zStream& operator<<(const zsummer::log4z::Log4zBinary& binary) { return writeBinary(binary); }

    template<class _Ty1, class _Ty2>
    inline Log4zStream& operator<<(const std::pair<_Ty1, _Ty2>& t)
    {
        return *this << "pair(" << t.first << ":" << t.second << ")";
    }

    template<class _Elem, class _Alloc>
    inline Log4zStream& operator<<(const std::vector<_Elem, _Alloc>& t)
    {
        *this << "vector(" << t.size() << ")[";
        int32_t inputCount = 0;
        for(typename std::vector<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); iter++)
        {
            inputCount++;
            if(inputCount > LOG4Z_LOG_CONTAINER_DEPTH)
            {
                *this << "..., ";
                break;
            }
            *this << *iter << ", ";
        }
        if(!t.empty())
        {
            _cur -= 2;
        }
        return *this << "]";
    }
    template<class _Elem, class _Alloc>
    inline Log4zStream& operator<<(const std::list<_Elem, _Alloc>& t)
    {
        *this << "list(" << t.size() << ")[";
        int32_t inputCount = 0;
        for(typename std::list<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); iter++)
        {
            inputCount++;
            if(inputCount > LOG4Z_LOG_CONTAINER_DEPTH)
            {
                *this << "..., ";
                break;
            }
            *this << *iter << ", ";
        }
        if(!t.empty())
        {
            _cur -= 2;
        }
        return *this << "]";
    }
    template<class _Elem, class _Alloc>
    inline Log4zStream& operator<<(const std::deque<_Elem, _Alloc>& t)
    {
        *this << "deque(" << t.size() << ")[";
        int32_t inputCount = 0;
        for(typename std::deque<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); iter++)
        {
            inputCount++;
            if(inputCount > LOG4Z_LOG_CONTAINER_DEPTH)
            {
                *this << "..., ";
                break;
            }
            *this << *iter << ", ";
        }
        if(!t.empty())
        {
            _cur -= 2;
        }
        return *this << "]";
    }
    template<class _Elem, class _Alloc>
    inline Log4zStream& operator<<(const std::queue<_Elem, _Alloc>& t)
    {
        *this << "queue(" << t.size() << ")[";
        int32_t inputCount = 0;
        for(typename std::queue<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); iter++)
        {
            inputCount++;
            if(inputCount > LOG4Z_LOG_CONTAINER_DEPTH)
            {
                *this << "..., ";
                break;
            }
            *this << *iter << ", ";
        }
        if(!t.empty())
        {
            _cur -= 2;
        }
        return *this << "]";
    }
    template<class _K, class _V, class _Pr, class _Alloc>
    inline Log4zStream& operator<<(const std::map<_K, _V, _Pr, _Alloc>& t)
    {
        *this << "map(" << t.size() << ")[";
        int32_t inputCount = 0;
        for(typename std::map<_K, _V, _Pr, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); iter++)
        {
            inputCount++;
            if(inputCount > LOG4Z_LOG_CONTAINER_DEPTH)
            {
                *this << "..., ";
                break;
            }
            *this << *iter << ", ";
        }
        if(!t.empty())
        {
            _cur -= 2;
        }
        return *this << "]";
    }

private:
    Log4zStream() {}
    Log4zStream(Log4zStream&) {}
    char* _begin;
    char* _end;
    char* _cur;
};

inline Log4zStream::Log4zStream(char* buf, int32_t len)
{
    _begin = buf;
    _end   = buf + len;
    _cur   = _begin;
}

template<class T>
inline Log4zStream& Log4zStream::writeData(const char* ft, T t)
{
    if(_cur < _end)
    {
        int32_t len   = 0;
        int32_t count = (int32_t)(_end - _cur);
#ifdef WIN32
        len = _snprintf(_cur, count, ft, t);
        if(len == count || len < 0)
        {
            len         = count;
            *(_end - 1) = '\0';
        }
#else
        len = snprintf(_cur, count, ft, t);
        if(len < 0)
        {
            *_cur = '\0';
            len   = 0;
        }
        else if(len >= count)
        {
            len         = count;
            *(_end - 1) = '\0';
        }
#endif
        _cur += len;
    }
    return *this;
}

inline Log4zStream& Log4zStream::writeLongLong(int64_t t)
{
#ifdef WIN32
    writeData("%I64d", t);
#else
    writeData("%lld", t);
#endif
    return *this;
}

inline Log4zStream& Log4zStream::writeULongLong(uint64_t t)
{
#ifdef WIN32
    writeData("%I64u", t);
#else
    writeData("%llu", t);
#endif
    return *this;
}

inline Log4zStream& Log4zStream::writePointer(const void* t)
{
#ifdef WIN32
    sizeof(t) == 8 ? writeData("%016I64x", (uint64_t)t) : writeData("%08I64x", (uint64_t)t);
#else
    sizeof(t) == 8 ? writeData("%016llx", (uint64_t)t) : writeData("%08llx", (uint64_t)t);
#endif
    return *this;
}

inline Log4zStream& Log4zStream::writeBinary(const Log4zBinary& t)
{
    writeData("%s", "\r\n\t[");
    for(int32_t i = 0; i < (t._len / 16) + 1; i++)
    {
        writeData("%s", "\r\n\t");
        *this << (void*)(t._buf + i * 16);
        writeData("%s", ": ");
        for(int32_t j = i * 16; j < (i + 1) * 16 && j < t._len; j++)
        {
            writeData("%02x ", (unsigned char)t._buf[j]);
        }
        writeData("%s", "\r\n\t");
        *this << (void*)(t._buf + i * 16);
        writeData("%s", ": ");
        for(int32_t j = i * 16; j < (i + 1) * 16 && j < t._len; j++)
        {
            if(isprint((unsigned char)t._buf[j]))
            {
                writeData(" %c ", t._buf[j]);
            }
            else
            {
                *this << " . ";
            }
        }
    }

    writeData("%s", "\r\n\t]\r\n\t");
    return *this;
}
inline Log4zStream& zsummer::log4z::Log4zStream::writeString(const char* t, size_t len)
{
    if(_cur < _end)
    {
        size_t count = (size_t)(_end - _cur);
        if(len > count)
        {
            len = count;
        }
        memcpy(_cur, t, len);
        _cur += len;
        if(_cur >= _end - 1)
        {
            *(_end - 1) = '\0';
        }
        else
        {
            *(_cur + 1) = '\0';
        }
    }
    return *this;
}
inline zsummer::log4z::Log4zStream& zsummer::log4z::Log4zStream::writeWString(const wchar_t* t)
{
#ifdef WIN32
    uint32_t dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
    if(dwLen < LOG4Z_LOG_BUF_SIZE)
    {
        std::string str;
        str.resize(dwLen, '\0');
        dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, &str[0], dwLen, NULL, NULL);
        if(dwLen > 0)
        {
            writeData("%s", str.c_str());
        }
    }
#else
    // not support
#endif
    return *this;
}

#ifdef WIN32
#pragma warning(pop)
#endif

_ZSUMMER_LOG4Z_END
_ZSUMMER_END

#endif
