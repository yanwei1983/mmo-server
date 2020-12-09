#include <iostream>

#include <google/protobuf/stubs/common.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "LoggingMgr.h"
#include "MemoryHelp.h"
#include "NetworkService.h"
#include "RobotClientManager.h"
#include "event2/event.h"
#include "event2/thread.h"
#include "get_opt.h"

void log_cb(int32_t severity, const char* msg)
{
    LOGNETERROR("{}", msg);
}

struct NetworkGInit
{
    NetworkGInit()
    {
        event_enable_debug_mode();
        evthread_use_pthreads();
        event_set_log_callback(log_cb);
        // LOGNETDEBUG("{}", "CNetworkService GInit");
    }
} const G_NetworkGInit;

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

#include "pb_luahelper.h"
std::atomic<uint32_t> pb_luahelper::ProtobufMessageWarp::s_nCount{0};
std::atomic<uint32_t> pb_luahelper::ConstProtobufMessageWarp::s_nCount{0};
std::atomic<uint32_t> pb_luahelper::ConstRepeatedProtobufMessageWarp::s_nCount{0};

std::vector<RobotClientManager*> vecManager;

int main(int argc, char* argv[])
{
    get_opt opt(argc, (const char**)argv);

    uint32_t max_thread = 1;
    if(opt.has("--thread"))
    {
        max_thread = std::atoi(opt["--thread"].c_str());
    }
    uint32_t nStart  = 1;
    uint32_t nAmount = 1;
    if(opt.has("--percount"))
    {
        nAmount = std::atoi(opt["--percount"].c_str());
    }
    if(opt.has("--start"))
    {
        nStart = std::atoi(opt["--start"].c_str());
    }
    fmt::print("thread={} percount={} start={} \n", max_thread, nAmount, nStart);

    BaseCode::InitLog("/data/log/robot_client");
    for(uint32_t i = 0; i < max_thread; i++)
    {
        RobotClientManager* pManager = new RobotClientManager(nStart + nAmount * i, nAmount, opt["--script"]);
        pManager->StartIOThread("client_IO");
        vecManager.push_back(pManager);
    }

    BaseCode::InitMonitorLog("comm");
    while(true)
    {
        __ENTER_FUNCTION
        // std::this_thread::yield();
        // PurgeJemalloc();
        auto result = get_memory_status();
        LOGMONITOR("alloc: {:.2f}m, "
                   "active: {:.2f}m, "
                   "rss: {:.2f}m, "
                   "ext: {:.2f}m, "
                   "vm: {:.2f}m, "
                   "meta: {:.2f}m, "
                   "meta_thp: {:.2f}m, "
                   "n_thread: {} "
                   "t_runtime: {}ms",
                   result.allocted / 1024.0f / 1024.0f,
                   result.active / 1024.0f / 1024.0f,
                   result.resident / 1024.0f / 1024.0f,
                   result.mapped / 1024.0f / 1024.0f,
                   result.retained / 1024.0f / 1024.0f,
                   result.metadata / 1024.0f / 1024.0f,
                   result.metadata_thp / 1024.0f / 1024.0f,
                   result.num_threads,
                   result.back_runtime/1000/1000);

        LOGMONITOR("CNetSocket:{} CNetworkMessage: {}", CNetSocket::s_Heap.GetAllocedSize(), CNetworkMessage::s_Heap.GetAllocedSize());
        LOGMONITOR("ProtobufMessageWarp:{} ConstProtobufMessageWarp:{} ConstRepeatedProtobufMessageWarp:{} ",
                   pb_luahelper::ProtobufMessageWarp::s_nCount.load(),
                   pb_luahelper::ConstProtobufMessageWarp::s_nCount.load(),
                   pb_luahelper::ConstRepeatedProtobufMessageWarp::s_nCount.load());

        __LEAVE_FUNCTION
        sleep(10);
    }

    for(auto& v: vecManager)
    {
        SAFE_DELETE(v);
    }
}