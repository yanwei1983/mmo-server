#ifndef LOOPHELPER_H
#define LOOPHELPER_H

#include <tuple>
#include <cstddef>

template<typename ContainerIter>
struct ipair_range
{
    struct Iter
    {
        ContainerIter iter;
        std::size_t index;
        Iter(ContainerIter iter)
            : iter(iter), index(0)
        {
        }
        
        auto operator*() const
        {
           return std::tie(index, *iter);
        } 
        
        Iter& operator++()
        {
            ++iter;
            ++index;
            
            return *this;
        }

        bool operator!=(const Iter& other) const
        {
            return this->iter != other.iter;
        }
        
    };

    auto begin() const
    {        
        return m_begin;
    }
    
    auto end() const
    {
        return m_end;
    }
    

    Iter m_begin;
    Iter m_end;
    
    ipair_range(ContainerIter beg, ContainerIter end) 
    :m_begin(beg)
    ,m_end(end)
    {}
};

template<class V>
auto ipairs(V&& v)
{
    return ipair_range{std::begin(v),std::end(v)};
}

template<class V>
auto ipairs_c(V&& v)
{
    return ipair_range{std::cbegin(v),std::cend(v)};
}





/*
使用range_based for时， 有时候我们希望能有一个计数器i，来帮我们进行一个简单计数，方便通过计数访问其他对象等

我们就通过一个包裹对象， 来返回一个特殊的Iter给range_based for, 进行一个额外的计数


iparis 传入const T&,内部就用const_iterator
iparis 传入T&,内部就用iterator
iparis_c 内部就是用const_iterator

使用range_based for来对ipairs操作， 返回的实际对象是 std::tuple<const szie_t& , const container_value& > 或 std::tuple<const szie_t& , container_value& >
通过结构化绑定 auto [i,v] 来将这个特殊的临时对象解绑， const auto&/auto&/auto 对这个临时对象来说是没有差别的
如果对map等内部value是一个pair的，还需在for循环内进行第二次结构化解绑,这个时候，就需要考虑是auto& 还是 const auto&了


//举例

//访问set/vector/deque等
//如果m_set是const的, 那么v就是const&， 否则就是&
for(auto[i,v] : ipairs(m_set))
{
    //v是const T& 或者 T&
}

用iparis_c(m_set)，那一定是constT&


//访问map等
for(const auto& [i,ref] : ipairs_c(arr) )
{
    const auto& [k,v] = ref;
    //v 是const T&
    auto& [k,v] = ref;
    //v 还是const T&
}

for(auto& [i,ref] : ipairs(arr) )
{
    const auto& [k,v] = ref;
    //v 是const T&
    auto& [k,v] = ref;
    //v 是T&
}

   







*/

#endif /* LOOPHELPER_H */
