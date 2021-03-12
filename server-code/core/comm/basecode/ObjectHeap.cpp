#include "ObjectHeap.h"

#include <algorithm>
#include <deque>

#include "BaseCode.h"
#include "LoggingMgr.h"

#ifdef USE_JEMALLOC
extern "C"
{
#define JEMALLOC_MANGLE
#include <jemalloc/jemalloc.h>
}

namespace
{

    inline uint32_t arena_alloc()
    {
        uint32_t    arena;
        std::size_t arena_byte_size = sizeof(arena);

        // Extend number of arenas
        if(je_mallctl("arenas.create", (void*)&arena, &arena_byte_size, nullptr, 0))
        {
            throw std::runtime_error("Failed to get new arena");
        }
        return arena;
    }

    inline void arena_free(unsigned arena) { je_mallctl(attempt_format("arena.{}.destroy", arena).c_str(), 0, 0, 0, 0); }

} // namespace
#endif

std::atomic<uint64_t> g_alloc_from_object_heap_size = 0;
uint64_t              get_alloc_from_object_heap()
{
    return g_alloc_from_object_heap_size;
}

CObjectHeap::CObjectHeap(const std::string& szClassName, size_t OneSize)
    : m_strName(szClassName)
    , m_OneSize(OneSize)
    , m_lNumAllocsInHeap(0)
    , m_lMaxAllocsInHeap(0)
{
}

CObjectHeap::~CObjectHeap()
{
    if(m_lNumAllocsInHeap != 0)
    {
        BaseCode::MyLogMsg("objheap",
                           true,
                           __FILE__,
                           __LINE__,
                           "ObjectHeap {} was not clear:{} {:p}",
                           m_strName.c_str(),
                           m_lNumAllocsInHeap.load(),
                           (void*)this);
    }
}

bool CObjectHeap::IsValidPt(void* p)
{
#ifdef USE_JEMALLOC
    return je_sallocx(p, 0) != 0;
#else
    return true;
#endif
}

#ifdef __linux__
void* CObjectHeap::AlignAlloc(std::size_t size, std::align_val_t align)
{
#ifdef USE_JEMALLOC
    void* result = je_aligned_alloc(enum_to(align), size);
#else
    void* result = aligned_alloc(align, size);
#endif
    if(result == nullptr)
    {
        throw std::bad_alloc();
    }
    g_alloc_from_object_heap_size += m_OneSize;

    m_lNumAllocsInHeap++;
    if(m_lNumAllocsInHeap > m_lMaxAllocsInHeap)
        m_lMaxAllocsInHeap = m_lNumAllocsInHeap.load();

    return result;
}
#else
void* CObjectHeap::AlignAlloc(std::size_t size, std::align_val_t align) 
{
    return Alloc(size);
}
#endif

void* CObjectHeap::Alloc(size_t size)
{
#ifdef USE_JEMALLOC
    void* result = je_malloc(size);
#else
    void* result = malloc(size);
#endif
    if(result == nullptr)
    {
        throw std::bad_alloc();
    }
    g_alloc_from_object_heap_size += m_OneSize;

    m_lNumAllocsInHeap++;
    if(m_lNumAllocsInHeap > m_lMaxAllocsInHeap)
        m_lMaxAllocsInHeap = m_lNumAllocsInHeap.load();


    return result;
}

void CObjectHeap::Free(void* ptr)
{
#ifdef USE_JEMALLOC
    je_free(ptr);
#else
    free(ptr);
#endif
    m_lNumAllocsInHeap--;
    g_alloc_from_object_heap_size -= m_OneSize;

}

size_t CObjectHeap::GetAllocedSize()
{
    return m_lNumAllocsInHeap;
}
