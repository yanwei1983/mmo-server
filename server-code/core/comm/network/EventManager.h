#ifndef EventManager_h__
#define EventManager_h__

#include <map>
#include <set>

#include <time.h>

#include "BaseCode.h"
#include "EventEntry.h"

struct event_base;
struct event;

class CEventManager : public NoncopyableT<CEventManager>
{
    CEventManager();
    bool Init(event_base* base);

public:
    CreateNewImpl(CEventManager);
    ~CEventManager();

    void Destory();

    void OnTimer();
    void Pause(bool bPause) { m_bPause = bPause; }
    bool RemoveWait(CEventEntry* pEntry);

    bool ScheduleEvent(const CEventEntryCreateParam& param);
    bool ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryPtr& refEntry);
    bool ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryQueue& refEntryQueue);
    bool ScheduleEvent(const CEventEntryCreateParam& param, CEventEntryMap& refEntryMap);

    size_t GetEventCount();
    void   AddEventCount();
    void   SubEventCount();
    size_t GetRunningEventCount();
    void   AddRunningEventCount();
    void   SubRunningEventCount();

protected:
    CEventEntry* _ScheduleEvent(const CEventEntryCreateParam& param, CEventEntry* pEntry, uint32_t nManagerType);
    CEventEntry* CreateEntry(const CEventEntryCreateParam& param, uint32_t nManagerType = 0);
    void         PushWait(CEventEntry* pEntry);
    void         ScheduleWait();

protected:
    void _DeleteMapedEvent(CEventEntry* pEntry);

protected:
    event_base*                        m_pBase              = nullptr;
    bool                               m_bOwnBase           = false;
    std::atomic<size_t>                m_nEventCount        = 0;
    std::atomic<size_t>                m_nRunningEventCount = 0;
    std::map<uint32_t, struct timeval> m_mapCommonTimeVal;
    std::map<CEventEntry*, bool>       m_mapEntry;
    std::set<CEventEntry*>             m_setWaitEntry;
    std::mutex                         m_mutex;
    struct event*                      m_pScheduleWaitEvent = nullptr;
    bool                               m_bPause             = false;
};
#endif // EventManager_h__
