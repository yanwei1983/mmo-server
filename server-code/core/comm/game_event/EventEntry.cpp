#include "EventEntry.h"

#include "EventManager.h"
#include "event2/event.h"

OBJECTHEAP_IMPLEMENTATION(CEventEntry, s_heap);

CEventEntry::CEventEntry(CEventManager* pManager, const CEventEntryCreateParam& param, uint32_t nManagerType)
    : m_pManager(pManager)
    , m_tWaitTime(param.tWaitTime)
    , m_evType(param.evType)
    , m_evManagerType(nManagerType)
    , m_pCallBack(param.cb)
    , m_bPersist(param.bPersist)
{
}

CEventEntry::~CEventEntry()
{
    Destory();
}

void CEventEntry::Destory()
{
    __ENTER_FUNCTION
    Cancel();
    if(m_pevTimer)
    {
        m_pManager->SubEventCount();
        event_free(m_pevTimer);
        m_pevTimer = nullptr;
    }
    __LEAVE_FUNCTION
}

uint32_t CEventEntry::GetManagerType() const
{
    return m_evManagerType;
}

void CEventEntry::SetManagerType(uint32_t val)
{
    m_evManagerType = val;
}

uint32_t CEventEntry::GetEventType() const
{
    return m_evType;
}

void CEventEntry::SetEventType(uint32_t val)
{
    m_evType = val;
}

void CEventEntry::Cancel()
{
    __ENTER_FUNCTION
    if(IsWaitTrigger() == false)
        return;
    if(m_pCallBack)
    {
        if(m_pevTimer)
        {
            event_del(m_pevTimer);
            m_pManager->SubRunningEventCount();
        }
        m_pCallBack = nullptr;
    }
    __LEAVE_FUNCTION
}

bool CEventEntry::IsCanceled() const
{
    return !IsWaitTrigger();
}

bool CEventEntry::IsWaitTrigger() const
{
    if(m_pevTimer)
    {
        return evtimer_pending(m_pevTimer, NULL) != 0;
    }
    return false;
}

bool CEventEntry::IsVaild() const
{
    return m_pevTimer && m_pCallBack;
}

void CEventEntry::Clear()
{
    __ENTER_FUNCTION

    if(IsWaitTrigger())
    {
        event_del(m_pevTimer);
        m_pManager->SubRunningEventCount();
    }
    m_pCallBack = nullptr;
    __LEAVE_FUNCTION
}

void CEventEntry::Set(const CEventEntryCreateParam& param)
{
    m_evType    = param.evType;
    m_pCallBack = param.cb;
    m_tWaitTime = param.tWaitTime;
    m_bPersist  = param.bPersist;
}

void CEventEntry::Release()
{
    __ENTER_FUNCTION
    CHECK(m_pManager)
    auto this_ptr = shared_from_this();
    m_pManager->RemoveWait(this_ptr);
    if(m_evManagerType == EMT_EVMANAGER)
    {
        m_pManager->_DeleteManagedEvent(this_ptr);
    }

    m_pManager->ReleaseEvent(this_ptr);

    __LEAVE_FUNCTION
}

bool CEventEntry::CreateEvTimer(event_base* base)
{
    __ENTER_FUNCTION
    if(m_pevTimer == nullptr)
    {
        m_pManager->AddEventCount();
        m_pevTimer = event_new(
            base,
            -1,
            (m_bPersist) ? EV_PERSIST : 0,
            [](int32_t, short, void* ctx) {
                CEventEntry* pEntry = (CEventEntry*)ctx;
                if(pEntry == nullptr || pEntry->m_pCallBack == nullptr)
                    return;

                int32_t nEventType   = pEntry->GetEventType();
                int32_t nManagerType = pEntry->GetManagerType();
                bool    bPersist     = pEntry->m_bPersist;
                pEntry->Trigger();

                if(bPersist == false)
                {
                    if(nManagerType == EMT_EVMANAGER)
                    {
                        pEntry->Release();
                        // pEntry is deleted
                    }
                }
            },
            this);
    }

    return m_pevTimer != nullptr;
    __LEAVE_FUNCTION
    return false;
}

void CEventEntry::Trigger()
{
    __ENTER_FUNCTION

    if(m_pCallBack)
    {
        auto call_back = m_pCallBack;
        call_back();
    }

    if(m_bPersist == false)
    {
        m_pManager->SubRunningEventCount();
    }
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////
CEventEntryMap::CEventEntryMap() {}

CEventEntryMap::~CEventEntryMap()
{
    Clear();
}

void CEventEntryMap::Clear()
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end();)
    {
        CEventEntrySharedPtr pEntry = it->second.lock();
        if(pEntry)
        {
            pEntry->Release();
        }
        it = m_setEntry.erase(it);
    }
    m_setEntry.clear();
    __LEAVE_FUNCTION
}

bool CEventEntryMap::Cancel(uint32_t evType)
{
    __ENTER_FUNCTION
    auto it = m_setEntry.find(evType);
    if(it != m_setEntry.end())
    {
        CEventEntrySharedPtr pEntry = it->second.lock();
        if(pEntry)
            pEntry->Cancel();

        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

void CEventEntryMap::ClearByType(uint32_t evType)
{
    __ENTER_FUNCTION
    auto it = m_setEntry.find(evType);
    if(it != m_setEntry.end())
    {
        CEventEntrySharedPtr pEntry = it->second.lock();
        if(pEntry)
        {
            pEntry->Release();
        }
        m_setEntry.erase(it);
    }
    __LEAVE_FUNCTION
}

CEventEntrySharedPtr CEventEntryMap::Query(uint32_t evType) const
{
    __ENTER_FUNCTION
    auto it = m_setEntry.find(evType);
    if(it != m_setEntry.end())
    {
        return it->second.lock();
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CEventEntryWeakPtr& CEventEntryMap::GetRef(uint32_t evType)
{
    return m_setEntry[evType];
}

CEventEntryWeakPtr& CEventEntryMap::operator[](uint32_t evType)
{
    return m_setEntry[evType];
}

bool CEventEntryMap::Set(const CEventEntrySharedPtr& pEntry)
{
    __ENTER_FUNCTION
    m_setEntry[pEntry->GetEventType()] = CEventEntryWeakPtr{pEntry};
    return true;
    __LEAVE_FUNCTION
    return false;
}

//////////////////////////////////////////////////////////////////////////
CEventEntryQueue::CEventEntryQueue() {}

CEventEntryQueue::~CEventEntryQueue()
{
    Clear();
}

void CEventEntryQueue::Clear()
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end();)
    {
        CEventEntrySharedPtr pEntry = it->lock();
        if(pEntry)
        {
            pEntry->Release();
        }
        it = m_setEntry.erase(it);
    }
    m_setEntry.clear();
    __LEAVE_FUNCTION
}

void CEventEntryQueue::ClearByType(uint32_t evType)
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end();)
    {
        CEventEntrySharedPtr pEntry = it->lock();
        if(pEntry && pEntry->GetEventType() == evType)
        {
            pEntry->Release();
            it = m_setEntry.erase(it);
        }
        else
        {
            it++;
        }
    }
    __LEAVE_FUNCTION
}

void CEventEntryQueue::Clear_IF(const std::function<bool(const CEventEntrySharedPtr&)>& func)
{
    __ENTER_FUNCTION
    for(auto it = m_setEntry.begin(); it != m_setEntry.end();)
    {
        CEventEntrySharedPtr pEntry = it->lock();
        if(pEntry && func(pEntry) == true)
        {
            pEntry->Release();
            it = m_setEntry.erase(it);
        }
        else
        {
            it++;
        }
    }
    __LEAVE_FUNCTION
}

bool CEventEntryQueue::Add(const CEventEntrySharedPtr& pEntry)
{
    __ENTER_FUNCTION
    if(pEntry)
    {
        m_setEntry.emplace(CEventEntryWeakPtr{pEntry});
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

//////////////////////////////////////////////////////////////////////////
CEventEntryPtr::CEventEntryPtr() {}

CEventEntryPtr::~CEventEntryPtr()
{
    Clear();
}

bool CEventEntryPtr::Cancel()
{
    __ENTER_FUNCTION
    if(auto shared_ptr = m_pEntry.lock())
    {
        shared_ptr->Cancel();
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

void CEventEntryPtr::Clear()
{
    __ENTER_FUNCTION
    if(auto shared_ptr = m_pEntry.lock())
    {
        shared_ptr->Release();
        m_pEntry.reset();
    }
    __LEAVE_FUNCTION
}

bool CEventEntryPtr::IsWaitTrigger()
{
    __ENTER_FUNCTION
    if(auto shared_ptr = m_pEntry.lock())
    {
        return shared_ptr->IsWaitTrigger();
    }
    __LEAVE_FUNCTION
    return false;
}

CEventEntrySharedPtr CEventEntryPtr::Query() const
{
    return m_pEntry.lock();
}

CEventEntryWeakPtr& CEventEntryPtr::GetRef()
{
    return m_pEntry;
}

bool CEventEntryPtr::Set(const CEventEntrySharedPtr& pEntry)
{
    m_pEntry = pEntry;
    return pEntry != nullptr;
}
