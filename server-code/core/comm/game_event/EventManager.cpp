#include "EventManager.h"

#include "event2/event.h"

CEventManager::CEventManager()
    : m_pBase(nullptr)
    , m_bOwnBase(false)
{
}

CEventManager::~CEventManager()
{
    Destory();
}

bool CEventManager::Init(event_base* base, bool bPause /* = true*/)
{
    m_bPause = bPause;
    if(base == nullptr)
    {
        m_pBase    = event_base_new();
        m_bOwnBase = true;
    }
    else
    {
        m_pBase              = base;
        m_pScheduleWaitEvent = evtimer_new(
            m_pBase,
            [](int32_t, short int32_t, void* ctx) {
                CEventManager* pThis = (CEventManager*)ctx;
                pThis->ScheduleWait();
            },
            (void*)this);
    }

    timeval set_tv_opt[] = {
        {0, 100 * 1000},   // 100ms
        {0, 200 * 1000},   // 200ms
        {0, 400 * 1000},   // 400ms
        {0, 600 * 1000},   // 600ms
        {0, 320 * 1000},   // 320ms
        {0, 680 * 1000},   // 680ms
        {1, 0},            // 1s
        {2, 0},            // 2s
        {3, 0},            // 3s
        {10, 0},           // 10s
        {30, 0},           // 30s
        {60, 0},           // 60s
        {3 * 60, 0},       // 3m
        {60 * 60, 0},      // 1h
        {24 * 60 * 60, 0}, // 1d
    };
    for(size_t i = 0; i < sizeOfArray(set_tv_opt); i++)
    {
        const timeval* tv_out = event_base_init_common_timeout(m_pBase, &set_tv_opt[i]);
        if(tv_out)
        {
            uint32_t uKey            = set_tv_opt[i].tv_sec * 1000 + set_tv_opt[i].tv_usec / 1000;
            m_mapCommonTimeVal[uKey] = *tv_out;
        }
    }

    return true;
}

void CEventManager::Destory()
{
    {
        m_ManagedEntry.clear();
        m_setWaitEntry.clear();
        m_AllEntry.clear();
    }

    if(m_bOwnBase && m_pBase)
    {
        event_base_free(m_pBase);
        m_pBase = nullptr;
    }
    else if(m_pScheduleWaitEvent && m_pBase)
    {
        event_del(m_pScheduleWaitEvent);
        event_free(m_pScheduleWaitEvent);
        m_pScheduleWaitEvent = nullptr;
        m_pBase              = nullptr;
    }
}

void CEventManager::OnTimer()
{
    if(m_bOwnBase && m_bPause == false)
    {
        event_base_loop(m_pBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
        ScheduleWait();
    }
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryPtr& refEntry)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);
    auto pEntry = CreateOrResetEntry(refEntry.GetRef(), param, EMT_ENTRY_PTR);
    CHECKF(pEntry);
    PushWait(pEntry);
    return refEntry.Set(pEntry);
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryMap& refEntryMap)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);

    auto pEntry = CreateOrResetEntry(refEntryMap.GetRef(param.evType), param, EMT_ENTRY_MAP);
    CHECKF(pEntry);
    PushWait(pEntry);
    return refEntryMap.Set(pEntry);
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryQueue& refEntryQueue)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);
    auto pEntry = CreateEntry(param, EMT_ENTRY_QUEUE);
    CHECKF(pEntry);
    PushWait(pEntry);

    return refEntryQueue.Add(pEntry);
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);

    auto pEntry = CreateEntry(param, EMT_EVMANAGER);
    {
        m_ManagedEntry.emplace(CEventEntryWeakPtr{pEntry});
    }
    PushWait(pEntry);
    return true;
}

CEventEntrySharedPtr CEventManager::CreateOrResetEntry(const CEventEntryWeakPtr& pEntry, const CEventEntryCreateParam& param, uint32_t nManagerType)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);

    CEventEntrySharedPtr shared_entry_ptr;

    if(pEntry.expired() == false)
    {
        shared_entry_ptr = pEntry.lock();
        if(shared_entry_ptr)
        {
            RemoveWait(shared_entry_ptr);

            shared_entry_ptr->Clear();
            shared_entry_ptr->Set(param);
            return shared_entry_ptr;
        }
    }

    shared_entry_ptr = CreateEntry(param, nManagerType);
    return shared_entry_ptr;
}

CEventEntrySharedPtr CEventManager::CreateEntry(const CEventEntryCreateParam& param, uint32_t nManagerType)
{
    CEventEntrySharedPtr pEntry{new CEventEntry(this, param, nManagerType)};
    if(pEntry)
    {
        if(pEntry->CreateEvTimer(m_pBase))
        {
            {
                m_AllEntry.emplace(pEntry);
                m_CountEntryByManagerType[nManagerType]++;
                m_CountEntryByType[param.evType]++;
            }
            return pEntry;
        }
    }
    return nullptr;
}

void CEventManager::ReleaseEvent(const CEventEntrySharedPtr& pEntry)
{
    m_AllEntry.erase(pEntry);
    m_CountEntryByManagerType[pEntry->GetManagerType()]--;
    m_CountEntryByType[pEntry->GetEventType()]--;
}

const std::unordered_map<uint32_t, uint32_t>& CEventManager::GetCountEntryByManagerType()
{
    return m_CountEntryByManagerType;
}

const std::unordered_map<uint32_t, uint32_t>& CEventManager::GetCountEntryByType()
{
    return m_CountEntryByType;
}

void CEventManager::PushWait(const CEventEntrySharedPtr& pEntry)
{
    {
        m_setWaitEntry.insert(CEventEntryWeakPtr{pEntry});
    }

    if(m_bOwnBase == false)
    {
        if(evtimer_pending(m_pScheduleWaitEvent, NULL) == 0)
        {
            struct timeval tv
            {
                0, 0
            };
            evtimer_add(m_pScheduleWaitEvent, &tv);
        }
    }
}

void CEventManager::ScheduleWait()
{

    while(true)
    {
        CEventEntrySharedPtr pEntry;
        {
            if(m_setWaitEntry.empty())
                return;
            pEntry = m_setWaitEntry.begin()->lock();
            m_setWaitEntry.erase(m_setWaitEntry.begin());
        }

        if(pEntry == nullptr)
            continue;
        if(pEntry->IsVaild() == false)
            continue;

        int32_t nRet = -1;
        auto    it   = m_mapCommonTimeVal.find((uint32_t)pEntry->m_tWaitTime);
        if(it != m_mapCommonTimeVal.end())
        {
            nRet = evtimer_add(pEntry->m_pevTimer, &it->second);
        }
        else
        {
            struct timeval tv = {(long)pEntry->m_tWaitTime / 1000, (long)pEntry->m_tWaitTime % 1000 * 1000};
            nRet              = evtimer_add(pEntry->m_pevTimer, &tv);
        }

        if(nRet == 0)
        {
            AddRunningEventCount();
            continue;
        }

        // if evtimer_add fail,delete it
        pEntry->Release();
    }
}

void CEventManager::_DeleteManagedEvent(const CEventEntrySharedPtr& pEntry)
{
    if(pEntry == nullptr)
        return;

    {
        auto it = m_ManagedEntry.find(CEventEntryWeakPtr{pEntry});
        if(it != m_ManagedEntry.end())
        {
            m_ManagedEntry.erase(it);
        }
        else
        {
            LOGERROR("try delet event not mapbymanager:{:p}", (void*)pEntry.get());
        }
    }
}

size_t CEventManager::GetEventCount()
{
    return m_nEventCount;
}

void CEventManager::AddEventCount()
{
    m_nEventCount++;
}

void CEventManager::SubEventCount()
{
    m_nEventCount--;
}

size_t CEventManager::GetRunningEventCount()
{
    return m_nRunningEventCount;
}

void CEventManager::AddRunningEventCount()
{
    m_nRunningEventCount++;
}

void CEventManager::SubRunningEventCount()
{
    m_nRunningEventCount--;
}

bool CEventManager::RemoveWait(const CEventEntrySharedPtr& pEntry)
{
    if(pEntry == nullptr)
        return false;
    m_setWaitEntry.erase(CEventEntryWeakPtr{pEntry});
    return true;
}
