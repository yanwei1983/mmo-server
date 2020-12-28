#include "Thread.h"

#define RESUME_SIG  SIGUSR2
#define SUSPEND_SIG SIGUSR1

static thread_local int32_t suspended = 0; // per-thread flag
#ifdef WIN32
#else
void resume_handler(int32_t sig, siginfo_t* pInfo, void* pVoid)
{
    suspended = 0;
}

void suspend_handler(int32_t sig, siginfo_t* pInfo, void* pVoid)
{
    if(suspended)
        return;
    suspended = 1;
    do
    {
        sigset_t wait_mask;
        sigfillset(&wait_mask);
        sigdelset(&wait_mask, SUSPEND_SIG);
        sigdelset(&wait_mask, RESUME_SIG);
        sigsuspend(&wait_mask);
    } while(suspended);
}
#endif

CNormalThread::CNormalThread(int32_t                      nWorkIntervalMS,
                             const std::string&           thread_name /*= std::string()*/,
                             on_thread_event_function_t&& on_thread_process_func /*= on_thread_event_function_t()*/,
                             on_thread_event_function_t&& on_thread_create_func /*= on_thread_event_function_t()*/,
                             on_thread_event_function_t&& on_thread_finish_func /*= on_thread_event_function_t() */)
    : m_nWorkIntervalMS(nWorkIntervalMS)
    , m_ThreadName(thread_name)
    , m_funcThreadProcess(std::move(on_thread_process_func))
    , m_funcThreadCreate(std::move(on_thread_create_func))
    , m_funcThreadFinish(std::move(on_thread_finish_func))
    , m_Thread{std::make_unique<std::thread>(std::bind(&CNormalThread::ThreadFunc, this))}
{
    CHECK_V(m_ThreadName.size() < 16, m_ThreadName);
}

CNormalThread::~CNormalThread()
{
    Stop();
    Join();
}

void CNormalThread::Stop()
{
    m_bStop = true;
}

void CNormalThread::Join()
{
    if(m_Thread)
    {
        m_Thread->join();
        m_Thread.reset();
    }
}

void CNormalThread::Suspend()
{
#ifdef WIN32
    SuspendThread(m_tid);
#else
    pthread_kill(m_tid, SUSPEND_SIG);
#endif
}

void CNormalThread::Resume()
{
#ifdef WIN32
    ResumeThread(m_tid);
#else
    pthread_kill(m_tid, RESUME_SIG);
#endif
}

bool CNormalThread::IsReady() const
{
    return m_bIsReady;
}

void CNormalThread::ThreadFunc()
{
    __ENTER_FUNCTION
#ifdef WIN32
    SetTid(GetCurrentThread());
#else
    SetTid(pthread_self());

    if(m_ThreadName.empty() == false)
    {
        pthread_setname_np(pthread_self(), m_ThreadName.c_str());
    }
#endif
    BaseCode::SetNdc(m_ThreadName);


#ifdef WIN32
#else
    //允许线程处理SUSPEND_SIG和RESUME_SIG
    sigset_t unblock_mask;
    sigemptyset(&unblock_mask);
    sigaddset(&unblock_mask, SUSPEND_SIG);
    sigaddset(&unblock_mask, RESUME_SIG);
    pthread_sigmask(SIG_UNBLOCK, &unblock_mask, NULL);

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = &resume_handler;
    sigaction(RESUME_SIG, &sa, NULL);

    sa.sa_sigaction = &suspend_handler;
    sigaction(SUSPEND_SIG, &sa, NULL);
#endif

    LOGDEBUG("ThreadCreate:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());
    if(m_funcThreadCreate)
    {
        m_funcThreadCreate();
    }

    TimeGetCacheCreate();
    m_bIsReady = true;
    while(!m_bStop)
    {
        __ENTER_FUNCTION
        auto beginTime = std::chrono::high_resolution_clock::now();
        TimeGetCacheUpdate();
        m_funcThreadProcess();
        auto endTime = std::chrono::high_resolution_clock::now();

        std::chrono::milliseconds costTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
        if(m_nWorkIntervalMS > 0)
        {
            std::chrono::milliseconds waitTime = std::chrono::milliseconds(m_nWorkIntervalMS) - costTime;
            if(waitTime.count() > 0)
            {
                msleep(waitTime.count() * 1000);
            }
            else
            {
                std::this_thread::yield();
            }
        }
        else
        {
            std::this_thread::yield();
        }
        __LEAVE_FUNCTION
    }

    if(m_funcThreadFinish)
    {
        m_funcThreadFinish();
    }

    LOGDEBUG("ThreadExit:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());
    BaseCode::ClearNdc();
    __LEAVE_FUNCTION
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWorkerThread::~CWorkerThread()
{
    Stop();
    Join();
}

void CWorkerThread::Start()
{
    {
        std::unique_lock<std::mutex> lk(m_csCV);
        if(m_bWaitStart == false)
            return;
        m_bWaitStart = false;
    }
    m_cv.notify_one();
}

void CWorkerThread::ThreadFunc()
{
    __ENTER_FUNCTION

#ifdef __linux__
    if(m_ThreadName.empty() == false)
    {
        pthread_setname_np(pthread_self(), m_ThreadName.c_str());
    }
#endif

    BaseCode::SetNdc(m_ThreadName);
    LOGDEBUG("ThreadCreate:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());

    if(m_bWaitStart)
    {
        std::unique_lock<std::mutex> lk(m_csCV);
        m_cv.wait(lk, [this] { return m_bWaitStart == false; });
    }

    if(m_funcThreadCreate)
    {
        m_funcThreadCreate();
    }
    m_bIsRunning = true;
    while(m_bStop.load() == false)
    {
        __ENTER_FUNCTION
        std::function<void()> job_func;
        while(m_JobList.get(job_func))
        {
            job_func();
        }
        if(m_bStop)
        {
            break;
        }
        std::unique_lock<std::mutex> lk(m_csCV);
        m_cv.wait(lk);
        __LEAVE_FUNCTION
    }

    if(m_funcThreadFinish)
    {
        m_funcThreadFinish();
    }
    LOGDEBUG("ThreadExit:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());
    BaseCode::ClearNdc();
    m_bIsRunning = false;
    __LEAVE_FUNCTION
}

void CWorkerThread::Stop()
{
    m_bStop = true;
    m_cv.notify_one();
}

void CWorkerThread::Join(bool bWaitAllJobFinish)
{
    if(m_Thread)
    {
        m_Thread->join();
        m_Thread.reset();
        if(bWaitAllJobFinish)
        {
            std::function<void()> job_func;
            while(m_JobList.get(job_func))
            {
                job_func();
            }
        }
    }
}

void CWorkerThread::AddJob(job_function_t&& job_func)
{
    // call by main thread
    m_JobList.push(std::move(job_func));
    m_cv.notify_one();
}

void CWorkerThread::_AddResult(result_function_t&& result_func)
{
    // call by worker thread
    m_ResultList.push(std::move(result_func));
}

void CWorkerThread::ProcessResult(int32_t nMaxProcess)
{
    // call by main thread
    int32_t               nProcessed = 0;
    std::function<void()> result_func;
    while(m_ResultList.get(result_func))
    {
        result_func();
        if(nMaxProcess > 0 && nProcessed >= nMaxProcess)
        {
            return;
        }
    }
}

bool CWorkerThread::IsRunning() const
{
    return m_bIsRunning;
}



std::string getProcessID()
{
    std::string pid      = "0";
    char        buf[260] = {0};
#ifdef WIN32
    uint32_t winPID = GetCurrentProcessId();
    pid             = fmt::format("{:06u}", winPID);
#else
    pid = fmt::format("{:06u}", getpid());
#endif
    return pid;
}

std::string getProcessName()
{
    std::string name      = "process";
    char        buf[1024] = {0};
#ifdef WIN32
    if(GetModuleFileNameA(NULL, buf, 259) > 0)
    {
        name = buf;
    }
    std::string::size_type pos = name.rfind("\\");
    if(pos != std::string::npos)
    {
        name = name.substr(pos + 1, std::string::npos);
    }
    pos = name.rfind(".");
    if(pos != std::string::npos)
    {
        name = name.substr(0, pos - 0);
    }

// #elif defined(LOG4Z_HAVE_LIBPROC)
// 	proc_name(getpid(), buf, 260);
// 	name = buf;
// 	return name;
// 	;
#else
    sprintf(buf, "/proc/%d/cmdline", (int32_t)getpid());
    FILE* fp = fopen(buf, "rb");
    if(!fp)
    {
        return name;
    }

    fgets(buf, 1024, fp);
    name = buf;
    fclose(fp);

    std::string::size_type pos = name.rfind('/');
    if(pos != std::string::npos)
    {
        name = name.substr(pos + 1, std::string::npos);
    }
#endif

    return name;
}