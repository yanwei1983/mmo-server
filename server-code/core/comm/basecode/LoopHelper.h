#ifndef LOOPHELPER_H
#define LOOPHELPER_H

#include <iostream>
#include <iterator>
#include <array>
#include <utility>
#include <functional>

template<typename V>
struct ipairs
{
    template<typename VIter>
    struct Iter
    {
        VIter iter;
        std::size_t index;

        Iter() = default;
        
        Iter(VIter iter, std::size_t offset)
            : iter(iter), index(offset)
        {
        }
        
        auto operator * () 
        {
            return std::make_tuple(std::ref(*iter), index);
        }
        
        Iter& operator ++ ()
        {
            ++iter;
            ++index;
            
            return *this;
        }

        
        bool operator != (Iter const& other) const
        {
            return this->iter != other.iter;
        }
        
    };
    
    V& v;
    std::size_t offset;
    Iter<decltype(begin(v))> iter;
    
    auto begin()
    {
        iter.iter = v.begin();
        iter.index = offset;
        
        return this->iter;
    }
    
    auto end()
    {
        return Iter(v.end(), 0);
    }
    

    ipairs(V& v) : v(v), offset(0) {}
    ipairs(V& v, std::size_t offset) : v(v), offset(offset) {}
};

#endif /* LOOPHELPER_H */
