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

bool CEventManager::Init(event_base* base)
{
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
        std::unique_lock<std::mutex> lock(m_mutex);
        for(auto it = m_mapEntry.begin(); it != m_mapEntry.end(); it++)
        {
            if(it->second == true)
            {
                CEventEntry* pEntry = it->first;
                SAFE_DELETE(pEntry);
            }
        }
        m_mapEntry.clear();
        for(auto it = m_setWaitEntry.begin(); it != m_setWaitEntry.end(); it++)
        {
            CEventEntry* pEntry = *it;
            SAFE_DELETE(pEntry);
        }
        m_setWaitEntry.clear();
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
    CEventEntry* pEntry = _ScheduleEvent(param, refEntry.GetRef(), EMT_ENTRY_PTR);
    if(pEntry)
    {
        return refEntry.Set(pEntry);
    }
    else
    {
        LOGERROR("ScheduleEvent Fail: {}", param.evType);
        LOGERROR("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7)));
        return false;
    }
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryQueue& refEntryQueue)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);
    CEventEntry* pEntry = _ScheduleEvent(param, nullptr, EMT_ENTRY_QUEUE);
    if(pEntry)
    {
        return refEntryQueue.Add(pEntry);
    }
    else
    {
        LOGERROR("ScheduleEvent Fail: {}", param.evType);
        LOGERROR("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7)));
        return false;
    }
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryMap& refEntryMap)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);

    CEventEntry* pEntry = _ScheduleEvent(param, refEntryMap.GetRef(param.evType), EMT_ENTRY_MAP);
    if(pEntry)
    {
        return refEntryMap.Set(pEntry);
    }
    else
    {
        LOGERROR("ScheduleEvent Fail: {}", param.evType);
        LOGERROR("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7)));
        return false;
    }
}

bool CEventManager::ScheduleEvent(const CEventEntryCreateParam& param)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);

    CEventEntry* pEntry = CreateEntry(param, EMT_EVMANAGER);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_mapEntry[pEntry] = false;
    }
    PushWait(pEntry);
    return true;
}

CEventEntry* CEventManager::_ScheduleEvent(const CEventEntryCreateParam& param, CEventEntry* pEntry, uint32_t nManagerType)
{
    CHECKF(param.cb);
    CHECKF(param.tWaitTime >= 0);

    if(pEntry)
    {
        RemoveWait(pEntry);
        pEntry->Clear();
        pEntry->Set(param, nManagerType);
    }
    else
    {
        pEntry = CreateEntry(param, nManagerType);
    }

    PushWait(pEntry);
    return pEntry;
}

CEventEntry* CEventManager::CreateEntry(const CEventEntryCreateParam& param, uint32_t nManagerType)
{
    CEventEntry* pEntry = new CEventEntry(this, param, nManagerType);
    if(pEntry)
    {
        if(pEntry->CreateEvTimer(m_pBase))
            return pEntry;

        SAFE_DELETE(pEntry);
    }
    return nullptr;
}

void CEventManager::PushWait(CEventEntry* pEntry)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_setWaitEntry.insert(pEntry);
    }

    if(m_bOwnBase == false)
    {
        struct timeval tvout;
        if(evtimer_pending(m_pScheduleWaitEvent, &tvout) == 0)
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
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_setWaitEntry.empty() == false)
    {
        CEventEntry* pEntry = *m_setWaitEntry.begin();
        m_setWaitEntry.erase(m_setWaitEntry.begin());
        if(pEntry == nullptr)
            continue;

        if(pEntry->IsVaild() == false)
        {
            continue;
        }

        if(pEntry->CreateEvTimer(m_pBase))
        {
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
            AddRunningEventCount();

            if(nRet == 0)
            {
                auto it = m_mapEntry.find(pEntry);
                if(it != m_mapEntry.end())
                    it->second = true;
                continue;
            }
        }
        // if evtimer_add fail,delete it
        _DeleteMapedEvent(pEntry);
    }
}

void CEventManager::_DeleteMapedEvent(CEventEntry* pEntry)
{
    if(pEntry == nullptr)
        return;

    {
        auto it = m_mapEntry.find(pEntry);
        if(it != m_mapEntry.end())
        {
            m_mapEntry.erase(it);
        }
        else
        {
            LOGERROR("try delet event not mapbymanager:{:p}", (void*)pEntry);
        }
    }

    SAFE_DELETE(pEntry);
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

bool CEventManager::RemoveWait(CEventEntry* pEntry)
{
    if(pEntry == nullptr)
        return false;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_setWaitEntry.erase(pEntry);
    return true;
}
