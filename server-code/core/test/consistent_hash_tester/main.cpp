#include "BaseCode.h"
#include "consistent_hasher.h"

int main()
{
    consistent_hasher cons_hasher(777);
    cons_hasher.add_node(1, 200);

    std::map<uint32_t,uint32_t> counter;
    for(int i = 0; i < 10000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(i);
        counter[node_id]++;
    }

    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
    counter.clear();

    cons_hasher.add_node(2, 200);
    cons_hasher.add_node(3, 200);
    cons_hasher.add_node(4, 200);
    fmt::print("add node 2,3,4\n");
    for(int i = 0; i < 10000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(i);
        counter[node_id]++;
    }
    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
    counter.clear();

    
    fmt::print("test for string node\n");
    for(int i = 0; i < 10000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(fmt::format("test:{}",i));
        counter[node_id]++;
    }
    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
    counter.clear();
    
    
    fmt::print("remove node 2,4\n");
    cons_hasher.remove_node(2);
    cons_hasher.remove_node(4);
    for(int i = 0; i < 10000; i++)
    {
        auto node_id = cons_hasher.find_nearest_node(i);
        counter[node_id]++;
    }
    for(const auto&[k,v] : counter)
    {
        fmt::print("node[{}]:count={}\n", k,v);
    }
    counter.clear();

}