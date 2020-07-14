#include "CallStackDumper.h"

#include <cstring>
#include <memory>

#include <cxxabi.h>
#include <dlfcn.h>

#include "BaseCode.h"
#include "LoggingMgr.h"
#include "fmt/chrono.h"
#include "TimeUtil.h"

std::string demangle(const char* name)
{
    // mangled_name
    //   A NULL-terminated character string containing the name to
    //   be demangled.
    // output_buffer:
    //   A region of memory, allocated with malloc, of *length bytes,
    //   into which the demangled name is stored. If output_buffer is
    //   not long enough, it is expanded using realloc. output_buffer
    //   may instead be NULL; in that case, the demangled name is placed
    //   in a region of memory allocated with malloc.
    // length
    //   If length is non-NULL, the length of the buffer containing the
    //   demangled name is placed in *length.
    // status
    //   *status is set to one of the following values:
    //    0: The demangling operation succeeded.
    //   -1: A memory allocation failure occurred.
    //   -2: mangled_name is not a valid name under the C++ ABI
    //       mangling rules.
    //   -3: One of the arguments is invalid.
    int32_t status = 0;
    char*   buf    = abi::__cxa_demangle(name, NULL, NULL, &status);
    if(status == 0)
    {
        std::string s(buf);
        free(buf);
        return s;
    }
    return std::string(name);
}

namespace
{
    // The prefix used for mangled symbols, per the Itanium C++ ABI:
    // http://www.codesourcery.com/cxx-abi/abi.html#mangling
    const char kMangledSymbolPrefix[] = "_Z";
    // Characters that can be used for symbols, generated by Ruby:
    // (('a'..'z').to_a+('A'..'Z').to_a+('0'..'9').to_a + ['_']).join
    const char kSymbolCharacters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    // Demangles C++ symbols in the given text. Example:
    // "out/Debug/base_unittests(_ZN10StackTraceC1Ev+0x20) [0x817778c]"
    // =>
    // "out/Debug/base_unittests(StackTrace::StackTrace()+0x20) [0x817778c]"
    std::string DemangleSymbol(const char* input_symbol)
    {
        std::string            symbol      = input_symbol;
        std::string::size_type search_from = 0;
        while(search_from < symbol.size())
        {
            // Look for the start of a mangled symbol from search_from
            std::string::size_type mangled_start = symbol.find(kMangledSymbolPrefix, search_from);
            if(mangled_start == std::string::npos)
            {
                break; // Mangled symbol not found
            }
            // Look for the end of the mangled symbol
            std::string::size_type mangled_end = symbol.find_first_not_of(kSymbolCharacters, mangled_start);
            if(mangled_end == std::string::npos)
            {
                mangled_end = symbol.size();
            }
            std::string mangled_symbol = symbol.substr(mangled_start, mangled_end - mangled_start);
            // Try to demangle the mangled symbol candidate
            int32_t status = -4; // some arbitrary value to eliminate the compiler warning
            std::unique_ptr<char, void (*)(void*)> demangled_symbol(
                abi::__cxa_demangle(mangled_symbol.c_str(), nullptr, 0, &status),
                std::free);
            // 0 Demangling is success
            if(0 == status)
            {
                // Remove the mangled symbol
                symbol.erase(mangled_start, mangled_end - mangled_start);
                // Insert the demangled symbol
                symbol.insert(mangled_start, demangled_symbol.get());
                // Next time, we will start right after the demangled symbol
                search_from = mangled_start + std::strlen(demangled_symbol.get());
            }
            else
            {
                // Failed to demangle. Retry after the "_Z" we just found
                search_from = mangled_start + 2;
            }
        }

        return symbol;
    }

    std::string getProcessID()
    {
        std::string pid      = "0";
        char        buf[260] = {0};
#ifdef WIN32
        uint32_t winPID = GetCurrentProcessId();
        sprintf(buf, "%06u", winPID);
        pid = buf;
#else
        sprintf(buf, "%06d", getpid());
        pid = buf;
#endif
        return pid;
    }

    std::string getProcessName()
    {
        std::string name     = "process";
        char        buf[260] = {0};
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
        sprintf(buf, "/proc/%d/cmdline", (int)getpid());
        FILE* fp = fopen(buf, "rb");
        if(!fp)
        {
            return name;
        }

        fgets(buf, 260, fp);
        name = buf;
        fclose(fp);

        std::string::size_type pos = name.rfind("/");
        if(pos != std::string::npos)
        {
            name = name.substr(pos + 1, std::string::npos);
        }
#endif

        return name;
    }

} // namespace

#include <bfd.h>

struct line_data
{
    asymbol**   symbol_table; /* Symbol table.  */
    bfd_vma     addr;
    std::string filename;
    std::string function_name;
    uint32_t    line;
    int32_t     line_found;
};

void process_section(bfd* abfd, asection* section, void* _data)
{
    line_data* data = (line_data*)_data;
    if(data->line_found)
    {
        // If we already found the line, exit
        return;
    }
    if((bfd_get_section_flags(abfd, section) & SEC_ALLOC) == 0)
    {
        return;
    }

    bfd_vma section_vma = bfd_get_section_vma(abfd, section);
    if(data->addr < section_vma)
    {
        // If the addr lies above the section, exit
        return;
    }

    bfd_size_type section_size = bfd_section_size(abfd, section);
    if(data->addr >= section_vma + section_size)
    {
        // If the addr lies below the section, exit
        return;
    }

    // Calculate the correct offset of our line in the section
    bfd_vma offset = data->addr - section_vma - 1;

    // Finds the line corresponding to the offset

    const char* filename      = NULL;
    const char* function_name = NULL;
    data->line_found =
        bfd_find_nearest_line(abfd, section, data->symbol_table, offset, &filename, &function_name, &data->line);

    if(filename == NULL)
    {
        data->filename = "";
    }
    else
    {
        data->filename = filename;
    }

    if(function_name == NULL)
    {
        data->function_name = "";
    }
    else
    {
        data->function_name = function_name;
    }
}

/* Loads the symbol table into 'data->symbol_table'.  */
int32_t load_symbol_table(bfd* abfd, line_data* data)
{
    if((bfd_get_file_flags(abfd) & HAS_SYMS) == 0)
    {
        // If we don't have any symbols, return
        return 0;
    }

    void**   symbol_table_ptr = reinterpret_cast<void**>(&data->symbol_table);
    long     n_symbols;
    uint32_t symbol_size;
    n_symbols = bfd_read_minisymbols(abfd, false, symbol_table_ptr, &symbol_size);
    if(n_symbols == 0)
    {
        // If the bfd_read_minisymbols() already allocated the table, we need
        // to free it first:
        if(data->symbol_table != NULL)
            free(data->symbol_table);
        // dynamic
        n_symbols = bfd_read_minisymbols(abfd, true, symbol_table_ptr, &symbol_size);
    }

    if(n_symbols < 0)
    {
        // bfd_read_minisymbols() failed
        return 1;
    }

    return 0;
}

std::string addr2line(const std::string& file_name, size_t addr)
{
    bfd* abfd;
    abfd = bfd_openr(file_name.c_str(), NULL);
    if(abfd == NULL)
        return "Cannot open the binary file '" + file_name + "'\n";

    line_data data;
    data.addr         = addr;
    data.symbol_table = NULL;
    data.line_found   = false;

    scope_guards scope_exit;
    scope_exit += [&abfd]() {
        bfd_close(abfd);
    };
    scope_exit += [&data]() {
        if(data.symbol_table != NULL)
        {
            free(data.symbol_table);
        }
    };

    if(bfd_check_format(abfd, bfd_archive))
    {
        return "Cannot get addresses from the archive '" + file_name + "'\n";
    }

    if(!bfd_check_format_matches(abfd, bfd_object, nullptr))
    {
        return "Unknown format of the binary file '" + file_name + "'\n";
    }

    // This allocates the symbol_table:
    if(load_symbol_table(abfd, &data) == 1)
    {
        return "Failed to load the symbol table from '" + file_name + "'\n";
    }
    // Loops over all sections and try to find the line
    bfd_map_over_sections(abfd, process_section, &data);
    // Deallocates the symbol table
    if(data.line_found)
        return data.filename + ":" + std::to_string(data.line);
    else
        return fmt::format("Failed to find {:X} from {}", addr, file_name);
}

std::string GetStackTraceString(const CallFrameMap& data)
{
    std::string result;
    Dl_info     dlinfo;
    time_t      t = _TimeGetSecond();
    result = fmt::format("%Y/%m/%d %H:%M:%S", timeToLocalTime(t));
    result += fmt::format("{}\n====================start=============================\n", BaseCode::getNdcStr());
    for(const auto& pair_v: data.m_Addr)
    {
        //尝试输出基址差
        if(dladdr(pair_v.first, &dlinfo) != 0)
        {
            std::string symbol = DemangleSymbol(pair_v.second.c_str());
            size_t      addr   = (size_t)(pair_v.first) - (size_t)(dlinfo.dli_fbase);
            if((size_t)(dlinfo.dli_fbase) <= 0x400000)
                addr = (size_t)(pair_v.first);
            result += fmt::format("ADDR:[{:X}]{}\n"
                                  "FILE:{}\n",
                                  addr,
                                  symbol.c_str(),
                                  addr2line(dlinfo.dli_fname, addr).c_str());
        }
        else
        {
            std::string symbol = DemangleSymbol(pair_v.second.c_str());
            result += fmt::format("TRACEADDR:{}\n", symbol.c_str());
        }
    }
    result += "==================end===================================\n";
    return result;
}

std::string GetStackTraceString(const CALLFRAME_NODE* pFrame)
{
    std::string result;
    Dl_info     dlinfo;
    time_t      t = _TimeGetSecond();
    result = fmt::format("%Y/%m/%d %H:%M:%S", timeToLocalTime(t));
    result += fmt::format("{}\n====================start=============================\n", BaseCode::getNdcStr());
    while(pFrame->m_pParent != nullptr)
    {
        //尝试输出基址差
        const char** funcnamearry = (const char**)backtrace_symbols(&pFrame->m_pCallFunc, 1);
        if(dladdr(pFrame->m_pCallFunc, &dlinfo) != 0)
        {
            std::string symbol = DemangleSymbol(*funcnamearry);
            size_t      addr   = (size_t)(pFrame->m_pCallFunc) - (size_t)(dlinfo.dli_fbase);
            if((size_t)(dlinfo.dli_fbase) <= 0x400000)
                addr = (size_t)(pFrame->m_pCallFunc);
            result += fmt::format("ADDR:[{:X}]{}\n"
                                  "FILE:{}\n",
                                  addr,
                                  symbol.c_str(),
                                  addr2line(dlinfo.dli_fname, addr).c_str());
        }
        else
        {
            std::string symbol = DemangleSymbol(*funcnamearry);
            result += fmt::format("TRACEADDR:{}\n", symbol.c_str());
        }
        free(funcnamearry);
        pFrame = pFrame->m_pParent;
    }
    result += "==================end===================================\n";
    return result;
}

bool DumpStack(const CALLFRAME_NODE* pFrame)
{
    LOGSTACK("{}", GetStackTraceString(pFrame));
    return true;
}

bool DumpStack(const CallFrameMap& data)
{
    LOGSTACK("{}", GetStackTraceString(data));
    return true;
}

bool DumpStackFile(const CallFrameMap& data)
{
    std::string txt = GetStackTraceString(data);

    std::string szLogName = fmt::format("{}_hangup_{}.log", getProcessName(), getProcessID());

    FILE* fp = fopen(szLogName.c_str(), "a+");
    if(nullptr == fp)
        return false;

    fwrite(txt.c_str(), txt.size(), 1, fp);
    fclose(fp);
    return true;
}

CallFrameMap::CallFrameMap(int32_t skip_calldepth, int32_t max_calldepth)
{
    constexpr int32_t MAX_BACKTRACE_SYMBOLS_NUMBER = 100;
    void*             pCallFramearray[MAX_BACKTRACE_SYMBOLS_NUMBER];
    size_t            nTrace       = backtrace(pCallFramearray, MAX_BACKTRACE_SYMBOLS_NUMBER);
    char**            funcnamearry = backtrace_symbols(pCallFramearray, nTrace);
    if(funcnamearry == NULL)
        return;

    for(size_t i = skip_calldepth + 1; i < nTrace && i <= max_calldepth; i++)
    {
        m_Addr.emplace_back(std::make_pair(pCallFramearray[i], std::string(funcnamearry[i])));
    }
    free(funcnamearry);
}

CALLFRAME_NODE::CALLFRAME_NODE(CALLFRAME_NODE* pParent /*= NULL*/, void* p /*= NULL*/)
    : m_pParent(pParent)
    , m_pCallFunc(p)
    , m_bClear(false)
{
}

CALLFRAME_NODE::~CALLFRAME_NODE()
{
    m_bClear = true;
    if(m_pParent)
        m_pParent->remove(this);

    for(CHILD_CALLFRAME_NODE::iterator it = m_setChild.begin(); it != m_setChild.end(); it++)
    {
        CALLFRAME_NODE* pNode = *it;
        delete pNode;
    }
    m_setChild.clear();
}

CALLFRAME_NODE* CALLFRAME_NODE::append(void* p)
{
    CHILD_CALLFRAME_NODE::iterator it = std::find_if(m_setChild.begin(), m_setChild.end(), Equal(p));
    if(it != m_setChild.end())
    {
        return *it;
    }
    else
    {
        CALLFRAME_NODE* pFrame = new CALLFRAME_NODE(this, p);
        m_setChild.push_back(pFrame);
        return pFrame;
    }
}

void CALLFRAME_NODE::remove(CALLFRAME_NODE* pChild)
{
    if(m_bClear)
        return;
    CHILD_CALLFRAME_NODE::iterator it = std::find(m_setChild.begin(), m_setChild.end(), pChild);
    if(it != m_setChild.end())
    {
        m_setChild.erase(it);
    }
}

CALLFRAME_NODE* CALLFRAME_NODE::MakeCallFrame(int32_t skip_calldepth)
{
    constexpr int32_t MAX_BACKTRACE_SYMBOLS_NUMBER = 100;
    void*             pCallFramearray[MAX_BACKTRACE_SYMBOLS_NUMBER];
    size_t            nTrace = backtrace(pCallFramearray, MAX_BACKTRACE_SYMBOLS_NUMBER);
    CALLFRAME_NODE*   pFrame = this;
    for(size_t i = skip_calldepth + 1; i < nTrace; i++)
    {
        pFrame = pFrame->append(pCallFramearray[i]);
    }
    return pFrame;
}

CALLFRAME_NODE::Equal::Equal(void* p)
    : m_pFunc(p)
{
}

bool CALLFRAME_NODE::Equal::operator()(CALLFRAME_NODE* const rht) const
{
    return m_pFunc == rht->m_pCallFunc;
}
