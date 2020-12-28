#include "CallStackDumper.h"

#include <fmt/chrono.h>

#include "BaseCode.h"
#include "LoggingMgr.h"
#include "TimeUtil.h"
#include "Thread.h"

#ifdef __linux__
#define BACKWARD_HAS_BFD 1
#endif

#include "backward.hpp"

std::string GetStackTraceString(int skip, int depth)
{
    using namespace backward;  
    StackTrace st;
    st.load_here(depth + skip+3);
    st.skip_n_firsts(skip+3);

    Printer printer;
    printer.address = true;
    printer.reverse = false;


    std::stringstream os;
    printer.print(st, os);
    return os.str();
}

bool DumpStack(int skip, int depth)
{
    LOGSTACK("{}", GetStackTraceString(skip+1,depth));
    return true;
}


bool DumpStackFile(int skip, int depth)
{
    std::string txt = GetStackTraceString(skip+1,depth);

    std::string szLogName = fmt::format("{}_hangup_{}.log", getProcessName(), getProcessID());

    FILE* fp = fopen(szLogName.c_str(), "a+");
    if(nullptr == fp)
        return false;

    fwrite(txt.c_str(), txt.size(), 1, fp);
    fclose(fp);
    return true;
}
