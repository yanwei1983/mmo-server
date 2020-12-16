#ifndef THREAD_H
#define THREAD_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>

#include <unistd.h>

#include "BaseCode.h"
#include "LockfreeQueue.h"
//普通的按照时间间隔运行的Thread
class CNormalThread
{
public:
    typedef std::function<void()> on_thread_event_function_t;

public:
    CNormalThread(int32_t                      nWorkIntervalMS,
                  const std::string&           thread_name            = std::string(),
                  on_thread_event_function_t&& on_thread_process_func = on_thread_event_function_t(),
                  on_thread_event_function_t&& on_thread_create_func  = on_thread_event_function_t(),
                  on_thread_event_function_t&& on_thread_finish_func  = on_thread_event_function_t());
    ~CNormalThread();
    uint32_t GetTid() const { return m_tid; }
    void     SetTid(uint32_t val) { m_tid = val; }

    void Stop();
    void Join();
    void Suspend();
    void Resume();

    bool IsReady() const;

    void ThreadFunc();

private:
    int32_t           m_nWorkIntervalMS;
    std::atomic<bool> m_bStop    = false;
    std::atomic<bool> m_bIsReady = false;
    uint32_t          m_tid;

    std::string m_ThreadName;

    on_thread_event_function_t m_funcThreadProcess;
    on_thread_event_function_t m_funcThreadCreate;
    on_thread_event_function_t m_funcThreadFinish;

    std::unique_ptr<std::thread> m_Thread;
};

//放入Job进行Process的Thread
class CWorkerThread
{
public:
    typedef std::function<void()> on_thread_event_function_t;
    typedef std::function<void()> job_function_t;
    typedef std::function<void()> result_function_t;

public:
    CWorkerThread(const std::string& thread_name = std::string(), bool bWaitStart = false)
        : m_ThreadName(thread_name)
        , m_bWaitStart(bWaitStart)
        , m_Thread{std::make_unique<std::thread>(std::bind(&CWorkerThread::ThreadFunc, this))}
    {
    }

    template<class event_func_t>
    CWorkerThread(const std::string& thread_name, bool bWaitStart, event_func_t&& on_thread_create_func)
        : m_ThreadName(thread_name)
        , m_bWaitStart(bWaitStart)
        , m_Thread{std::make_unique<std::thread>(std::bind(&CWorkerThread::ThreadFunc, this))}
        , m_funcThreadCreate(std::forward<event_func_t>(on_thread_create_func))
    {
    }

    template<class event_func_t>
    CWorkerThread(const std::string& thread_name, bool bWaitStart, event_func_t&& on_thread_create_func, event_func_t&& on_thread_finish_func)
        : m_ThreadName(thread_name)
        , m_bWaitStart(bWaitStart)
        , m_Thread{std::make_unique<std::thread>(std::bind(&CWorkerThread::ThreadFunc, this))}
        , m_funcThreadCreate(std::forward<event_func_t>(on_thread_create_func))
        , m_funcThreadFinish(std::forward<event_func_t>(on_thread_finish_func))
    {
    }
    ~CWorkerThread();

    void Start();
    void Stop();
    void Join(bool bWaitAllJobFinish = true);

    void AddJob(job_function_t&& job_func);

    void _AddResult(result_function_t&& result_func);

    void ProcessResult(int32_t nMaxProcess = -1);

    bool IsRunning() const;

    void ThreadFunc();

private:
    MPSCQueue<job_function_t>    m_JobList;
    MPSCQueue<result_function_t> m_ResultList;

    std::atomic<bool>       m_bStop      = false;
    std::atomic<bool>       m_bIsRunning = false;
    std::mutex              m_csCV;
    std::condition_variable m_cv;
    bool                    m_bWaitStart;

    std::string                m_ThreadName;
    on_thread_event_function_t m_funcThreadCreate;
    on_thread_event_function_t m_funcThreadFinish;

    std::unique_ptr<std::thread> m_Thread;
};

inline int32_t get_cur_thread_id()
{
    constexpr int32_t ___NR_gettid = 186;
    return (int32_t)syscall(___NR_gettid);
}
#endif /* THREAD_H */
