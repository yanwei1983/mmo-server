#ifndef ATTEMPUTIL_H
#define ATTEMPUTIL_H

#include <fmt/format.h>
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

template<class... Args>
static inline std::string attempt_format(Args&&... args)
{
    try
    {
        return fmt::format(std::forward<Args>(args)...);
    }
    catch(fmt::format_error& e)
    {
        return fmt::format("format_error:{} fmt:{}", e.what(), std::forward<Args>(args)...);
    }
    catch(std::exception e)
    {
        return {e.what()};
    }
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

#endif /* ATTEMPUTIL_H */
