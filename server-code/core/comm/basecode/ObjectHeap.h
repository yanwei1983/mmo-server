#ifndef __OBJECTHEAP_H__
#define __OBJECTHEAP_H__

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "BaseType.h"
#include "CallStackDumper.h"

uint64_t get_alloc_from_object_heap();

//#ifdef DEBUG
//#define HEAP_DEBUG
//#endif

class CObjectHeap
{
public:
    CObjectHeap(const std::string& szClassName, size_t OneSize);
    virtual ~CObjectHeap();

    bool   IsValidPt(void* p);
    void*  Alloc(std::size_t size);
    void*  AlignAlloc(std::size_t size, std::align_val_t align);
    void   Free(void* ptr);
    size_t GetAllocedSize();

protected:
    std::string           m_strName;
    size_t                m_OneSize;
    std::atomic<uint32_t> m_lNumAllocsInHeap;
    std::atomic<uint32_t> m_lMaxAllocsInHeap;

#if defined(HEAP_DEBUG)
    std::mutex                                 m_mutexDebugInfo;
    CALLFRAME_NODE*                            m_setCallFrame;
    std::unordered_map<void*, CALLFRAME_NODE*> m_setDebugInfo;
#endif
};

#define OBJECTHEAP_DECLARATION(VAR)                                                                                                        \
public:                                                                                                                                    \
    void* operator new(std::size_t size) { return VAR.Alloc(size); }                                                                       \
    void* operator new[](std::size_t size) { return VAR.Alloc(size); }                                                                     \
    void* operator new(std::size_t size, const std::nothrow_t&) noexcept { return VAR.Alloc(size); }                                       \
    void* operator new[](std::size_t size, const std::nothrow_t&) noexcept { return VAR.Alloc(size); }                                     \
    void  operator delete(void* ptr) noexcept { VAR.Free(ptr); }                                                                           \
    void  operator delete[](void* ptr) noexcept { VAR.Free(ptr); }                                                                         \
    void  operator delete(void* ptr, const std::nothrow_t&) noexcept { VAR.Free(ptr); }                                                    \
    void  operator delete[](void* ptr, const std::nothrow_t&) noexcept { VAR.Free(ptr); }                                                  \
                                                                                                                                           \
public:                                                                                                                                    \
    void operator delete(void* ptr, std::size_t size) noexcept { VAR.Free(ptr); }                                                          \
    void operator delete[](void* ptr, std::size_t size) noexcept { VAR.Free(ptr); }                                                        \
                                                                                                                                           \
public:                                                                                                                                    \
    void* operator new(std::size_t size, std::align_val_t align) { return VAR.AlignAlloc(size, align); }                                   \
    void* operator new(std::size_t size, std::align_val_t align, const std::nothrow_t&) noexcept { return VAR.AlignAlloc(size, align); }   \
    void* operator new[](std::size_t size, std::align_val_t align) { return VAR.AlignAlloc(size, align); }                                 \
    void* operator new[](std::size_t size, std::align_val_t align, const std::nothrow_t&) noexcept { return VAR.AlignAlloc(size, align); } \
    void  operator delete(void* ptr, std::align_val_t) noexcept { VAR.Free(ptr); }                                                         \
    void  operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept { VAR.Free(ptr); }                                  \
    void  operator delete(void* ptr, std::size_t size, std::align_val_t al) noexcept { VAR.Free(ptr); }                                    \
    void  operator delete[](void* ptr, std::align_val_t) noexcept { VAR.Free(ptr); }                                                       \
    void  operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept { VAR.Free(ptr); }                                \
    void  operator delete[](void* ptr, std::size_t size, std::align_val_t al) noexcept { VAR.Free(ptr); }                                  \
                                                                                                                                           \
public:                                                                                                                                    \
    static bool IsValidPt(void* p) { return VAR.IsValidPt(p); }                                                                            \
                                                                                                                                           \
public:                                                                                                                                    \
    static CObjectHeap VAR;

#define OBJECTHEAP_IMPLEMENTATION(T, VAR) CObjectHeap T::VAR(#T, sizeof(T));

#endif // __OBJECTHEAP_H__
