#include "BaseCode.h"
#include "consistent_hasher.h"

int main()
{
    consistent_hasher cons_hasher(888);
    cons_hasher.add_node(1, 100);

    std::map<uint32_t,uint32_t> counter;
    for(int i = 0; i < 1000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(i);
        counter[node_id]++;
    }

    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
    counter.clear();

    cons_hasher.add_node(2, 100);
    cons_hasher.add_node(3, 100);
    fmt::print("add node 2,3\n");
    for(int i = 0; i < 1000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(i);
        counter[node_id]++;
    }
    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
    counter.clear();

    
    fmt::print("remove node 2\n");
    cons_hasher.remove_node(2);
    for(int i = 0; i < 1000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(i);
        counter[node_id]++;
        
    }
    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
}