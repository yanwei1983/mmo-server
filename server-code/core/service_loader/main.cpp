#include <iostream>
#include <set>
#include <thread>

#include <fmt/format.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/logging.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "BaseCode.h"
#include "FileLock.h"
#include "GlobalSetting.h"
#include "LoggingMgr.h"
#include "MemoryHelp.h"
#include "ObjectHeap.h"
#include "SegvCatch.h"
#include "ServiceDefine.h"
#include "ServiceLoader.h"
#include "StringAlgo.h"
#include "Thread.h"
#include "get_opt.h"

#ifdef USE_JEMALLOC
extern "C"
{
#define JEMALLOC_MANGLE
#include <jemalloc/jemalloc.h>
}
#endif
// std::atomic<uint64_t> g_GlobalAllocSize(0);
// inline void   operator delete	(void* p)
//{
//	if(p)
//	{
//		size_t size_mem = je_sallocx(p, 0);
//		g_GlobalAllocSize -= size_mem;
//		je_free(p);
//	}
//}
//
// inline void*  operator new	(size_t size)
//{
//	g_GlobalAllocSize += size;
//	return je_malloc(size);
//}
//
// inline void   operator delete[](void* p)
//{
//	size_t size_mem = je_sallocx(p, 0);
//	g_GlobalAllocSize -= size_mem;
//	je_free(p);
//}
//
// inline void*  operator new[](size_t size)
//{
//	g_GlobalAllocSize += size;
//	return je_malloc(size);
//}

void ProtobufLogHandler(google::protobuf::LogLevel level, const char* file, int32_t line, const std::string& msg)
{
    LOGERROR("PBError: {} in {}:{}", msg.c_str(), file, line);
}

struct PB_Initer
{
public:
    PB_Initer()
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        google::protobuf::SetLogHandler(ProtobufLogHandler);
    }
    ~PB_Initer() {}
} const s_PB_Initer;

void print_help()
{
    std::cout << " [-h|--help] [--config=service.xml] [--worldid=0] [--start] [--start=1,2,3,4,5,6] [--stop] [--daemon]" << std::endl;
}

#ifndef STDIN_FILENO
/* Standard file descriptors. */
#define STDIN_FILENO  0 /* Standard input. */
#define STDOUT_FILENO 1 /* Standard output. */
#define STDERR_FILENO 2 /* Standard error output. */
#endif

ServiceLoader*             g_pLoader;
std::unique_ptr<file_lock> plock;
std::mutex                 g_tem_mutex;
int32_t                    savefd_out = -1;
int32_t                    savefd_err = -1;
std::string                start_service_set;

void destory_all()
{
    if(g_pLoader)
    {
        g_pLoader->Destory();
        plock->unlock();
        plock.reset();
        BaseCode::StopLog();
        SAFE_DELETE(g_pLoader);

        ReleaseGlobalSetting();
    }
}
//////////////////////////////////////////////////////////////////////////
void sig_term(int32_t signo, siginfo_t* pInfo, void* pVoid)
{
    __ENTER_FUNCTION
    std::unique_lock<std::mutex> lock(g_tem_mutex, std::try_to_lock);
    if(lock.owns_lock() == false)
    {
        return;
    }
    destory_all();
    if(savefd_out != -1)
        dup2(savefd_out, STDOUT_FILENO);
    if(savefd_err != -1)
        dup2(savefd_err, STDERR_FILENO);
    fmt::print("service {} destory.\n", start_service_set);
    __LEAVE_FUNCTION
    exit(1);
    // std::quick_exit(1);
}

// int32_t daemon_init()
//{
//	int32_t		i;
//	pid_t	pid;
//
//	if ((pid = fork()) < 0)
//		return (-1);
//	else if (pid)
//		_exit(0);			/* parent terminates */
//
//							/* child 1 continues... */
//
//	if (setsid() < 0)			/* become session leader */
//		return (-1);
//
//	signal(SIGHUP, SIG_IGN);
//	if ((pid = fork()) < 0)
//		return (-1);
//	else if (pid)
//		_exit(0);			/* child 1 terminates */
//
//	/* child 2 continues... */
//
//	/* close off file descriptors */
//	for (i = 0; i < 64; i++)
//		close(i);
//
//	return (0);				/* success */
//}

int32_t main(int32_t argc, char* argv[])
{
    
    get_opt opt(argc, (const char**)argv);
    //请小心使用daemon/fork,这样会导致在main函数之前创建的线程被干掉
    if(opt.has("--daemon") || opt.has("-d"))
    {
        daemon(1, 1);
        // daemon_init();
    }
    // block all sig ,除了SIGFPE和SIGSEGV
    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGFPE);
    sigdelset(&block_mask, SIGSEGV);
    sigprocmask(SIG_BLOCK, &block_mask, NULL);

    G_INITSEGV();

    setlocale(LC_ALL, "en_US.UTF-8");
    tzset();

    uint16_t nWorldID = 0;
    if(opt.has("--worldid"))
    {
        nWorldID = std::atoi(opt["--worldid"].c_str());
    }

    if(opt.has("--stop"))
    {
        file_lock lock("service_loader_" + std::to_string(nWorldID) + "_" + opt["--stop"]);
        lock.kill(SIGQUIT);
        exit(0);
    }
    else if(opt.has("--start") == false)
    {
        print_help();
        exit(0);
    }

    std::string setting_filename = "res/service_cfg.json";
    if(opt.has("--config"))
    {
        setting_filename = opt["--config"];
    }
    CreateGlobalSetting();
    if(GetGlobalSetting()->LoadSetting(setting_filename) == false)
    {
        fmt::print(std::cerr, "load LoadSetting from {} fail.\n", setting_filename);
        exit(-1);
    }

    std::string logpath = "./log/";
    if(opt.has("--logpath"))
    {
        logpath = opt["--logpath"];
    }
    int32_t log_lev = LOG_LEVEL_DEBUG;
    if(opt.has("--loglev"))
    {
        log_lev = std::atoi(opt["--loglev"].c_str());
    }

    BaseCode::InitLog(logpath);
    BaseCode::SetNdc("service_loader");
    g_pLoader = new ServiceLoader();
    if(opt.has("--start"))
        start_service_set = opt["--start"];

    plock.reset(new file_lock("service_loader_" + std::to_string(nWorldID) + "_" + start_service_set));
    if(plock->lock() == false)
    {
        std::cerr << "only can start one instance." << std::endl;
        exit(-1);
    }

    auto                vec_start_service = split_string(start_service_set, ",");
    std::set<ServiceID> create_service_set;
    for(const auto& serviceid_str: vec_start_service)
    {
        if(serviceid_str.empty() == false && serviceid_str != "" && serviceid_str != "\001")
        {
            auto vec_servicetype = split_string(serviceid_str, "-");
            if(vec_servicetype.size() == 2)
            {
                uint8_t service_type = GetServiceTypeFromName(vec_servicetype[0]);
                uint8_t service_idx  = std::stol(vec_servicetype[1]);
                create_service_set.emplace(service_type, service_idx);
            }
        }
    }

    if(g_pLoader->Load(nWorldID, create_service_set) == false)
    {
        destory_all();
        fmt::print(stderr, "service {} load fail.\n", start_service_set);
        exit(-1);
    }
    fmt::print("service {} load succ.\n", start_service_set);
    LOGDEBUG("main ThreadID:{}", get_cur_thread_id());
    FILE* pStdOutFile = fopen((logpath + "/stdout.log").c_str(), "w+");
    if(pStdOutFile == NULL)
    {
        destory_all();
        fmt::print(stderr, "open stdout.log fail.\n");
        exit(-1);
    }

    savefd_out = dup(STDOUT_FILENO);
    savefd_err = dup(STDERR_FILENO);
    dup2(fileno(pStdOutFile), STDOUT_FILENO);
    dup2(fileno(pStdOutFile), STDERR_FILENO);
    fclose(pStdOutFile);
    pStdOutFile = NULL;
    

    {
        //当前主线程处理该信号
        sigset_t unblock_mask;
        sigemptyset(&unblock_mask);
        sigaddset(&unblock_mask, SIGTERM);
        sigaddset(&unblock_mask, SIGINT);
        sigaddset(&unblock_mask, SIGQUIT);
        pthread_sigmask(SIG_UNBLOCK, &unblock_mask, NULL);

        struct sigaction sa;
        sigfillset(&sa.sa_mask); // block all sa when process
        sa.sa_flags     = SA_SIGINFO;
        sa.sa_sigaction = &sig_term;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGQUIT, &sa, NULL);
    }

    BaseCode::InitMonitorLog("comm");
    while(true)
    {
        __ENTER_FUNCTION
        // std::this_thread::yield();
        // PurgeJemalloc();
        auto alloc_from_obj_heap = get_alloc_from_object_heap();
        auto result              = get_memory_status();

        LOGMONITOR("obj_heap: {:.2f}m, "
                   "alloc: {:.2f}m, "
                   "active: {:.2f}m, "
                   "rss: {:.2f}m, "
                   "ext: {:.2f}m, "
                   "vm: {:.2f}m, "
                   "meta: {:.2f}m, "
                   "meta_thp: {:.2f}m, "
                   "n_thread: {} "
                   "t_runtime: {}ms",
                   alloc_from_obj_heap / 1024.0f / 1024.0f,
                   result.allocted / 1024.0f / 1024.0f,
                   result.active / 1024.0f / 1024.0f,
                   result.resident / 1024.0f / 1024.0f,
                   result.mapped / 1024.0f / 1024.0f,
                   result.retained / 1024.0f / 1024.0f,
                   result.metadata / 1024.0f / 1024.0f,
                   result.metadata_thp / 1024.0f / 1024.0f,
                   result.num_threads,
                   result.back_runtime / 1000 / 1000);

        sleep(60);
        __LEAVE_FUNCTION
    }
}
