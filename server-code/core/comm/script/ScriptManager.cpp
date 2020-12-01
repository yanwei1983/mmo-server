#include "ScriptManager.h"

#include "BaseCode.h"
#include "FileUtil.h"
#include "MD5.h"
#include "lua_tinker.h"

//////////////////////////////////////////////////////////////////////////////////////
// static void *my_l_alloc (void *ud, void *ptr, size_t osize, size_t nsize)
//{
//    (void)osize;
//    if (nsize == 0)
//    {
//        if(ptr == NULL)
//            return NULL;
//		je_free(ptr);
//        return NULL;
//    }
//	else
//	{
//		return je_realloc(ptr, nsize)
//	}
//}

///////////////////////////////////////////////////////////////////////////////////////
static int32_t my_panic(lua_State* L)
{
    (void)L; /* to avoid warnings */
    LOGERROR("PANIC: unprotected error in call to Lua API ({})", lua_tostring(L, -1));
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
LUALIB_API lua_State* my_luaL_newstate()
{
    // lua_State *L = lua_newstate(my_l_alloc, (void*)pManager);
    lua_State* L = luaL_newstate();
    if(L)
        lua_atpanic(L, &my_panic);
    return L;
}

//为decoda打开lua调试函数
#ifdef _DEBUG
#pragma comment(linker, "/include:_luaL_ref")
#pragma comment(linker, "/include:_luaL_unref")
#pragma comment(linker, "/include:_luaL_newmetatable")
#pragma comment(linker, "/include:_lua_cpcall")
#endif

///////////////////////////////////////////////////////////////////////////////////////
CLUAScriptManager::CLUAScriptManager()
    : m_pLua(nullptr)
    , m_nLuaGCStep(1)
    , m_nLuaGCStepTick(100)
    , m_tNextGCStepTime(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////
CLUAScriptManager::~CLUAScriptManager()
{
    Destory();
}

void InitBaseCodeInLua(lua_State* L)
{
    extern void basecode2lua(lua_State*);
    basecode2lua(L);
}

void LogLuaError(const char* txt)
{
    LOGLUAERROR("{}", txt);
}

void LogLuaDebug(const char* txt)
{
    LOGLUADEBUG("{}", txt);
}

///////////////////////////////////////////////////////////////////////////////////////
bool CLUAScriptManager::Init(const std::string& name, InitRegisterFunc func, void* pInitParam, const char* search_path /*= "script"*/, bool bExecMain)
{
    m_pInitRegisterFunc = func;
    m_pInitParam        = pInitParam;

    if(m_pLua == nullptr)
    {
        m_pLua = my_luaL_newstate();
        CHECKF(m_pLua);

        luaL_openlibs(m_pLua);
        lua_tinker::init(m_pLua);
        lua_tinker::class_add<lua_State>(m_pLua, "lua_State");
        lua_tinker::set(m_pLua, "this_lua", m_pLua);

        lua_tinker::class_add<CLUAScriptManager>(m_pLua, "CLUAScriptManager");
        lua_tinker::class_def<CLUAScriptManager>(m_pLua, "RegistFucName", &CLUAScriptManager::RegistFucName);
        lua_tinker::class_def<CLUAScriptManager>(m_pLua, "LoadFilesInDir", &CLUAScriptManager::LoadFilesInDir);

        lua_tinker::def(m_pLua, "InitBaseCodeInLua", &InitBaseCodeInLua);
        lua_tinker::def(m_pLua, "_ALERT", &LogLuaError);
        lua_tinker::def(m_pLua, "logdebug", &LogLuaDebug);

        lua_tinker::set(m_pLua, "script_manager", this);
    }
    CHECKF(m_pLua);

    if(m_pInitRegisterFunc)
    {
        m_pInitRegisterFunc(m_pLua, m_pInitParam);
    }

    if(search_path)
    {
        lua_tinker::table_onstack table(m_pLua, "package");
        std::string               load_path = table.get<std::string>("path");
        table.set("path", load_path + ";./" + search_path + "/?.lua");

        lua_tinker::dofile(m_pLua, (std::string(search_path) + "/main.lua").c_str());
        m_search_path = std::string("./") + search_path;
    }
    else
    {
        lua_tinker::dofile(m_pLua, "main.lua");
    }
    // 调用初始化函数
    if(bExecMain)
        lua_tinker::call<void>(m_pLua, "main");
    LOGDEBUG("ScriptManager Init Succ");
    return true;
}

void CLUAScriptManager::Destory()
{
    if(m_pLua)
    {
        m_script_table.clear();
        lua_close(m_pLua);
        m_pLua = nullptr;
        LOGDEBUG("ScriptManager Destory Succ");
    }
}

///////////////////////////////////////////////////////////////////////////////////////
void CLUAScriptManager::Reload(const std::string& name, bool bExecMain)
{
    Init(name, m_pInitRegisterFunc, m_pInitParam, m_search_path.c_str(), bExecMain);
}

void CLUAScriptManager::LoadFilesInDir(const std::string& dir, bool bRecursive)
{
    //遍历文件夹
    scan_dir(m_search_path, dir, bRecursive, [this](const std::string& dirname, const std::string& filename) {
        if(get_file_ext(filename) != "lua")
            return;

        try
        {
            lua_tinker::dofile(m_pLua, (dirname + "/" + filename).c_str());
        }
        catch(...)
        {
        }
    });
}

void CLUAScriptManager::OnTimer(time_t tTick)
{
    if(m_pLua)
    {
        if(tTick > m_tNextGCStepTime)
        {
            m_tNextGCStepTime = tTick + m_nLuaGCStepTick;
            lua_gc(m_pLua, LUA_GCSTEP, m_nLuaGCStep);
        }
    }
}

void CLUAScriptManager::FullGC()
{
    lua_gc(m_pLua, LUA_GCCOLLECT, 0);
}


void CLUAScriptManager::RegistScriptType(uint32_t idScriptType, const std::string& table_name)
{
    m_ScriptTableName[idScriptType] = table_name;
}

std::string_view CLUAScriptManager::ScriptTypeToName(uint32_t idScriptType) const
{
    auto it = m_ScriptTableName.find(idScriptType);
    if(it != m_ScriptTableName.end())
        return it->second;
    return {};
}

//注册一个函数回调名
void CLUAScriptManager::RegistFucName(uint32_t idScriptType, uint64_t idScript)
{
    m_Data[idScriptType].insert(idScript);
}

bool CLUAScriptManager::IsRegisted(uint32_t idScriptType, uint64_t idScript) const
{
    auto it_find = m_Data.find(idScriptType);
    if(it_find == m_Data.end())
        return false;
    const auto& refSet = it_find->second;
    if(refSet.count(idScript) > 0)
    {
        return true; 
    }
    else
    {
        return false;
    }
    
}


const lua_tinker::table_ref& CLUAScriptManager::QueryScriptTable(const std::string_view& table_name)
{
    auto it = m_script_table.find(table_name);
    if(it == m_script_table.end())
        return it->second;
    
    auto table_ref = lua_tinker::get<lua_tinker::table_ref>(m_pLua, table_name.data());
    m_script_table.emplace(table_name, std::move(table_ref));

    it = m_script_table.find(table_name);
    return it->second;
}

bool CLUAScriptManager::QueryScriptFunc(uint32_t idScriptType, uint64_t idScript, const std::string_view& FuncName)
{
    if(idScript == 0)
        return false;
    if(IsRegisted(idScriptType, idScript) == false)
        return false;
    const auto& table_ref = QueryScriptTable(ScriptTypeToName(idScriptType));
    auto table_onstack = table_ref.push_table_to_stack();
    bool succ = table_onstack.get_to_stack(FuncName.data());
    if(succ == true)
    {
        if(lua_isfunction(m_pLua, -1) == true)
            return true;
        
        lua_pop(m_pLua, 1);
    }
    return false;
}