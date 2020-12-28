#ifndef CHECKUTIL_H
#define CHECKUTIL_H

#include <functional>
#include <optional>

#include "LoggingMgr.h"
#include "CallStackDumper.h"

#ifndef CHECK
#define CHECK(expr)                                                             \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr);                                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return;                                                             \
        }                                                                       \
    }
#endif

#ifndef CHECKF
#define CHECKF(expr)                                                            \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr);                                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return 0;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RET
#define CHECK_RET(expr, r)                                                      \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr);                                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RETTYPE
#define CHECK_RETTYPE(expr, R)                                                  \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr);                                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            static R r;                                                         \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_M
#define CHECK_M(expr, msg)                                                      \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return;                                                             \
        }                                                                       \
    }
#endif

#ifndef CHECKF_M
#define CHECKF_M(expr, msg)                                                     \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return 0;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RET_M
#define CHECK_RET_M(expr, r, msg)                                               \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RETTYPE_M
#define CHECK_RETTYPE_M(expr, R, msg)                                           \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            static R r;                                                         \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_V
#define CHECK_V(expr, v)                                                        \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return;                                                             \
        }                                                                       \
    }
#endif

#ifndef CHECKF_V
#define CHECKF_V(expr, v)                                                       \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return 0;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RET_V
#define CHECK_RET_V(expr, r, v)                                                 \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RETTYPE_V
#define CHECK_RETTYPE_V(expr, R, v)                                             \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                         \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            static R r;                                                         \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_FMT
#define CHECK_FMT(expr, fmt_msg, ...)                                           \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return;                                                             \
        }                                                                       \
    }
#endif

#ifndef CHECKF_FMT
#define CHECKF_FMT(expr, fmt_msg, ...)                                          \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return 0;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RET_FMT
#define CHECK_RET_FMT(expr, r, fmt_msg, ...)                                    \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef CHECK_RETTYPE_FMT
#define CHECK_RETTYPE_FMT(expr, R, fmt_msg, ...)                                \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            static R r;                                                         \
            return r;                                                           \
        }                                                                       \
    }
#endif

#ifndef __ENTER_FUNCTION
#define __ENTER_FUNCTION \
    {                    \
        try              \
        {
#endif

#ifndef __LEAVE_FUNCTION
#define __LEAVE_FUNCTION                                                    \
    }                                                                       \
    catch(const std::runtime_error& e)                                      \
    {                                                                       \
        LOGERROR("catch_execpetion:{}", e.what());                          \
        LOGERROR("CallStack: {}", GetStackTraceString(1, 6)); \
    }                                                                       \
    catch(const std::exception& e)                                          \
    {                                                                       \
        LOGERROR("catch_execpetion:{}", e.what());                          \
        LOGERROR("CallStack: {}", GetStackTraceString(1, 6)); \
    }                                                                       \
    catch(...) { LOGSTACK("catch_error"); }                                 \
    }
#endif

#ifndef __LEAVE_FUNCTION_NO_LOG
#define __LEAVE_FUNCTION_NO_LOG \
    }                           \
    catch(...) {}               \
    }
#endif

#ifdef _DEBUG

#ifndef CHECK_DEBUG
#define CHECK_DEBUG(expr)                                                       \
    {                                                                           \
        if(!(expr))                                                             \
        {                                                                       \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);          \
            LOGTRACE("CallStack: {}", GetStackTraceString(2, 7)); \
            return 0;                                                           \
        }                                                                       \
    }
#endif

#else

#ifndef CHECK_DEBUG
#define CHECK_DEBUG(expr)
#endif

#endif

template<class Func, class... Args>
static inline std::optional<std::string> attempt(Func&& func, Args&&... args)
{
    try
    {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    }
    catch(std::exception e)
    {
        return {e.what()};
    }
    return {};
}

#ifndef attempt_call
#define attempt_call(x)                                                         \
    {                                                                           \
        try                                                                     \
        {                                                                       \
            x;                                                                  \
        }                                                                       \
        catch(const std::runtime_error& e)                                      \
        {                                                                       \
            LOGERROR("catch_execpetion:{}", e.what());                          \
            LOGERROR("CallStack: {}", GetStackTraceString(1, 6)); \
        }                                                                       \
        catch(const std::exception& e)                                          \
        {                                                                       \
            LOGERROR("catch_execpetion:{}", e.what());                          \
            LOGERROR("CallStack: {}", GetStackTraceString(1, 6)); \
        }                                                                       \
        catch(...)                                                              \
        {                                                                       \
            LOGSTACK("catch_error");                                            \
        }                                                                       \
    }
#endif

#ifndef attempt_call_printerr
#define attempt_call_printerr(x)                         \
    {                                                    \
        try                                              \
        {                                                \
            x;                                           \
        }                                                \
        catch(const std::runtime_error& e)               \
        {                                                \
            fmt::print("catch_execpetion:{}", e.what()); \
        }                                                \
        catch(const std::exception& e)                   \
        {                                                \
            fmt::print("catch_execpetion:{}", e.what()); \
        }                                                \
        catch(...)                                       \
        {                                                \
            fmt::print("catch_execpetion:unknown");      \
        }                                                \
    }

#define __attempt_call_printerr_begin() \
    {                                   \
        try                             \
        {

#define __attempt_call_printerr_end()                                                   \
    }                                                                                   \
    catch(const std::runtime_error& e) { fmt::print("catch_execpetion:{}", e.what()); } \
    catch(const std::exception& e) { fmt::print("catch_execpetion:{}", e.what()); }     \
    catch(...) { fmt::print("catch_execpetion:unknown"); }                              \
    }
#endif

#endif /* CHECKUTIL_H */
