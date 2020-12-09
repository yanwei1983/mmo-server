#ifndef MEMORYHELP_H
#define MEMORYHELP_H

#include "BaseType.h"

struct memory_status
{
    uint64_t allocted = 0;
    uint64_t active   = 0;
    uint64_t resident = 0;
    uint64_t mapped   = 0;
    uint64_t retained = 0;

    uint64_t metadata     = 0;
    uint64_t metadata_thp = 0;

    uint64_t num_threads  = 0;
    uint64_t back_runtime = 0;
};

uint64_t      get_thread_memory_allocted();
memory_status get_memory_status();

void open_jemalloc_thp();
void start_jemalloc_backgroud_thread();
void stop_jemalloc_backgroud_thread();

#endif /* MEMORYHELP_H */
