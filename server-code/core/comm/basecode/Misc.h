#ifndef MISC_H
#define MISC_H

#include <cstring>
#include <deque>
#include <functional>

template<class T>
void SAFE_DELETE(T*& p)
{
    if(p)
    {
        delete p;
        p = nullptr;
    }
}
template<class T>
void SAFE_DELETE_ARRAY(T*& p)
{
    if(p)
    {
        delete[] p;
        p = nullptr;
    }
}
template<class T>
void SAFE_RELEASE(T*& p)
{
    if(p)
    {
        p->Release();
        p = nullptr;
    }
}

template<class T, size_t N>
constexpr inline size_t sizeOfArray(T (&array)[N])
{
    return N;
}

template<class T, size_t N>
inline void array_set(T (&array)[N], const T& v)
{
    std::fill(&array, &array + N, v);
}

template<class T, size_t N>
inline void array_set(std::array<T, N>& array, const T& v)
{
    memset(&array, 0, sizeof(T) * N);
}

template<class T, size_t N>
inline void array_setzero(T (&array)[N])
{
    memset(&array, 0, sizeof(T) * N);
}

template<class T, size_t N>
inline void array_setzero(std::array<T, N>& array)
{
    memset(&array, 0, sizeof(T) * N);
}

template<class... Args>
struct type_list
{
    using tuple_type = std::tuple<Args...>;
    template<std::size_t N>
    using type = typename std::tuple_element<N, tuple_type>::type;

    static constexpr size_t size = std::tuple_size<tuple_type>::value;
};

class scope_guards
{
public:
    scope_guards()                    = default;
    scope_guards(const scope_guards&) = delete;
    void operator=(const scope_guards&) = delete;
    scope_guards(scope_guards&&)        = delete;
    void operator=(scope_guards&&) = delete;

    template<class Callable>
    scope_guards& operator+=(Callable&& undo_func)
    {
        m_data.emplace_front(std::forward<Callable>(undo_func));
        return *this;
    }

    ~scope_guards()
    {
        for(auto& f: m_data)
        {
            try
            {
                f(); // must not throw
            }
            catch(...)
            {
            }
        }
    }

    void dismiss() noexcept { m_data.clear(); }

private:
    std::deque<std::function<void()>> m_data;
};

#define unknown(p) ((p) ? (p) : "unknown")

#define DEFINE_CONFIG_SET(T)                         \
public:                                              \
    class T* Get##T() const { return m_p##T.get(); } \
                                                     \
private:                                             \
    std::unique_ptr<class T> m_p##T;

#define DEFINE_CONFIG_LOAD(T)                                           \
    m_p##T.reset(CreateNew<T>(GetCfgFilePath() + T::GetCfgFileName())); \
    CHECKF(m_p##T.get());

#endif /* MISC_H */
