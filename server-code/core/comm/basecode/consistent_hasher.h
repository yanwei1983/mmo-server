#ifndef CONSISTENT_HASHER_H
#define CONSISTENT_HASHER_H

#include "BaseCode.h"
#include "StaticHash.h"
#include <map>

class consistent_hasher
{
public:
    consistent_hasher(uint32_t seed = 0)
    :m_seed(seed)
    {}

    void add_node(uint32_t node_id, uint32_t virtual_node_count)
    {
        m_nodeinfo.emplace(node_id, virtual_node_count);

        
        for(int i = 0; i < virtual_node_count; i++)
        {
            uint64_t value = MakeUINT64(node_id,i);
            auto key = hash64(value);
            m_nodes.try_emplace(key, node_id);
        }
    }

    void remove_node(uint32_t node_id)
    {
        auto it_info = m_nodeinfo.find(node_id);
        if(it_info == m_nodeinfo.end())
            return;
        
        auto virtual_node_count = it_info->second;
        std::hash<uint64_t> hasher;
        for(int i = 0; i < virtual_node_count; i++)
        {
            uint64_t value = MakeUINT64(node_id,i);
            auto key = hash64(value);
            auto it = m_nodes.find(key);
            if(it == m_nodes.end())
                continue;
            if(it->second == node_id)
            {
                m_nodes.erase(it);
            }
        }
    }


    template<class T>
    uint32_t find_nearest_node(T&& key) const
    {
        if(m_nodes.empty())
            return -1;
        std::hash<std::decay_t<T>> hasher;
        auto hash_value = hasher(key);
        auto hash_key = hash64(hash_value);
        auto it = m_nodes.lower_bound(hash_key);
        if(it != m_nodes.end())
            return it->second;
        return m_nodes.begin()->second;
        
    }

private:
    uint32_t hash64(uint64_t hash_value) const
    {
        auto hash_key = hash::MurmurHash3::shash((char*)&hash_value, sizeof(uint64_t)/sizeof(char), m_seed);
        return hash_key;
    }
private:
    using node_id_t = uint32_t;
    using hash_t = uint32_t;
    uint32_t m_seed = 0;
    std::map<hash_t, node_id_t> m_nodes;
    std::map<node_id_t, uint32_t> m_nodeinfo;
};

#endif /* CONSISTENT_HASHER_H */
