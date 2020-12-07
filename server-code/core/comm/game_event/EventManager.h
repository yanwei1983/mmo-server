#ifndef EventManager_h__
#define EventManager_h__

#include <map>
#include <set>
#include <unordered_set>
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
    bool RemoveWait(const CEventEntrySharedPtr& pEntry);
    void _DeleteManagedEvent(const CEventEntrySharedPtr& pEntry);
    void ReleaseEvent(const CEventEntrySharedPtr& pEntry);
    
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
    
    const std::unordered_map<uint32_t, uint32_t>& GetCountEntryByManagerType();
    const std::unordered_map<uint32_t, uint32_t>& GetCountEntryByType();
protected:
    CEventEntrySharedPtr CreateOrResetEntry(const CEventEntryWeakPtr& pEntry, const CEventEntryCreateParam& param, uint32_t nManagerType);
    CEventEntrySharedPtr CreateEntry(const CEventEntryCreateParam& param, uint32_t nManagerType = 0);
    
    void         PushWait(const CEventEntrySharedPtr& pEntry);
    void         ScheduleWait();

protected:
    

protected:
    event_base*                        m_pBase              = nullptr;
    bool                               m_bOwnBase           = false;
    std::atomic<size_t>                m_nEventCount        = 0;
    std::atomic<size_t>                m_nRunningEventCount = 0;
    std::map<uint32_t, struct timeval> m_mapCommonTimeVal;
    std::set<CEventEntryWeakPtr, std::owner_less<CEventEntryWeakPtr>>       m_ManagedEntry;
    std::set<CEventEntryWeakPtr, std::owner_less<CEventEntryWeakPtr>>       m_setWaitEntry;
    std::unordered_set<CEventEntrySharedPtr> m_AllEntry;
    std::unordered_map<uint32_t, uint32_t> m_CountEntryByManagerType;
    std::unordered_map<uint32_t, uint32_t> m_CountEntryByType;
    
    struct event*                      m_pScheduleWaitEvent = nullptr;
    bool                               m_bPause             = false;
};
#endif // EventManager_h__
