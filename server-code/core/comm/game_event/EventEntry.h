#ifndef EventEntry_h__
#define EventEntry_h__

#include <functional>
#include <map>
#include <set>
#include <memory>

#include "BaseCode.h"
#include "ObjectHeap.h"

struct event;
struct event_base;
typedef std::function<void()> EventCallBackFunc;

enum EventManagerType
{
    EMT_EVMANAGER = 0,
    EMT_ENTRY_PTR,
    EMT_ENTRY_QUEUE,
    EMT_ENTRY_MAP,
};
class CEventManager;

struct CEventEntryCreateParam
{
    uint32_t          evType = 0;
    EventCallBackFunc cb;
    time_t            tWaitTime = 0;
    bool              bPersist  = false;
};

class CEventEntry : public std::enable_shared_from_this<CEventEntry>
{
    CEventEntry(CEventManager* pManager, const CEventEntryCreateParam& param, uint32_t nManagerType);

public:
    ~CEventEntry();

public:
    void Destory();
    void Cancel();
    void Clear();
    void Set(const CEventEntryCreateParam& param);
    void Release();

    void Trigger();

    uint32_t GetManagerType() const;
    void     SetManagerType(uint32_t val);
    uint32_t GetEventType() const;
    void     SetEventType(uint32_t val);
    bool     IsCanceled() const;
    bool     IsWaitTrigger() const;
    bool     IsVaild() const;
private:
    bool CreateEvTimer(event_base* base);
public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    CEventManager*    m_pManager;
    EventCallBackFunc m_pCallBack;
    time_t            m_tWaitTime;
    bool              m_bPersist;
    event*            m_pevTimer = nullptr;
    uint32_t          m_evType;
    uint32_t          m_evManagerType;
    friend class CEventManager;
};
using CEventEntrySharedPtr = std::shared_ptr<CEventEntry>;
using CEventEntryWeakPtr = std::weak_ptr<CEventEntry>;

class CEventEntryMap
{
public:
    CEventEntryMap();
    ~CEventEntryMap();

public:
    void               Clear();
    bool               Cancel(uint32_t evType);
    void               ClearByType(uint32_t evType);
    CEventEntrySharedPtr Query(uint32_t evType) const;

protected:
    CEventEntryWeakPtr& GetRef(uint32_t evType);
    CEventEntryWeakPtr& operator[](uint32_t evType);
    bool          Set(const CEventEntrySharedPtr& pEntry);

protected:
    std::map<uint32_t, CEventEntryWeakPtr> m_setEntry;
    friend class CEventManager;
};

class CEventEntryQueue
{
public:
    CEventEntryQueue();
    ~CEventEntryQueue();

public:
    void Clear();
    void ClearByType(uint32_t evType);
    void Clear_IF(const std::function<bool(const CEventEntrySharedPtr&)>& func);

protected:
    bool Add(const CEventEntrySharedPtr& pEntry);

protected:
    std::set<CEventEntryWeakPtr, std::owner_less<CEventEntryWeakPtr> > m_setEntry;
    friend class CEventManager;
};

class CEventEntryPtr
{
public:
    CEventEntryPtr();
    ~CEventEntryPtr();

public:
    bool Cancel();
    void Clear();
    bool IsWaitTrigger();

protected:
    CEventEntrySharedPtr  Query() const;
    CEventEntryWeakPtr& GetRef();
    bool          Set(const CEventEntrySharedPtr& pEntry);

protected:
    CEventEntryWeakPtr m_pEntry;
    friend class CEventManager;
};
#endif // EventEntry_h__
