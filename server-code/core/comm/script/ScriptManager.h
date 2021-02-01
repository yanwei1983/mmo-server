#ifndef script_manager_h__
#define script_manager_h__

#include <map>
#include <unordered_map>

#include <fmt/format.h>

#include "BaseCode.h"
#include "ObjectHeap.h"
#include "lua_tinker.h"

class CLUAScriptManager : public NoncopyableT<CLUAScriptManager>
{
public:
    //每个lua被创建时都会调用一下该函数来向lua注册一些必备的c++函数,比如类注册函数等
    using InitRegisterFunc = void (*)(lua_State*, void*);

private:
    CLUAScriptManager();
    bool Init(const std::string& name,
              InitRegisterFunc   func,
              void*              pInitParam,
              const char*        search_path    = "script",
              const char*        main_file_name = "main.lua",
              bool               bExecMain      = true);

public:
    CreateNewImpl(CLUAScriptManager);
    ~CLUAScriptManager();

    void Destroy();
    void Reload(const std::string& name, bool bExecMain);

    void LoadFilesInDir(const std::string& dir, bool bRecursive);

    void OnTimer(time_t tTick);
    void FullGC();

public:
    lua_State* GetRawPtr()const {return m_pLua;}
    int32_t GetLuaGCStep() const { return m_nLuaGCStep; }
    void    SetLuaGCStep(int32_t val) { m_nLuaGCStep = val; }
    int32_t GetLuaGCStepTick() const { return m_nLuaGCStepTick; }
    void    SetLuaGCStepTick(int32_t val) { m_nLuaGCStepTick = val; }

public:
    void               RegistScriptType(uint32_t idScriptType, const std::string& table_name);
    const std::string& ScriptTypeToName(uint32_t idScriptType) const;

    //注册一个函数回调名
    void RegistFucName(uint32_t idScriptType, uint64_t idScript);
    bool IsRegisted(uint32_t idScriptType, uint64_t idScript) const;

    template<typename RVal, typename... Args>
    RVal ExecScript(uint32_t idScriptType, uint64_t idScript, const std::string& FuncName, Args&&... args)
    {
        __ENTER_FUNCTION
        if(idScript == 0)
            return RVal();
        if(IsRegisted(idScriptType, idScript) == false)
            return RVal();
        std::string funcName = fmt::format("{}[{}].{}", ScriptTypeToName(idScriptType), idScript, FuncName);

        return _ExecScript<RVal>(funcName.c_str(), std::forward<Args>(args)...);
        __LEAVE_FUNCTION
        return RVal();
    }

    bool QueryScriptFunc(uint32_t idScriptType, uint64_t idScript, const std::string& FuncName);

    template<typename RVal, typename... Args>
    RVal ExecStackScriptFunc(Args&&... args)
    {
        __ENTER_FUNCTION
        if constexpr(std::is_same_v<void, RVal>)
        {
            CHECK(lua_isfunction(m_pLua, -1));
        }
        else
        {
            CHECK_RETTYPE(lua_isfunction(m_pLua, -1), RVal);
        }

        return lua_tinker::call_stackfunc<RVal>(m_pLua, std::forward<Args>(args)...);
        __LEAVE_FUNCTION
        return RVal();
    }

    template<typename RVal, typename... Args>
    RVal TryExecScript(uint32_t idScriptType, uint64_t idScript, const std::string& FuncName, Args&&... args)
    {
        __ENTER_FUNCTION
        if(idScript == 0)
            return RVal();
        if(IsRegisted(idScriptType, idScript) == false)
            return RVal();
        auto table_ref_ptr = QueryScriptTable(ScriptTypeToName(idScriptType));
        if(table_ref_ptr == nullptr)
            return RVal();
        auto table_onstack = table_ref_ptr->push_table_to_stack();
        bool succ          = table_onstack.get_to_stack(FuncName.data());
        if(succ == false)
            return RVal();

        auto func = lua_tinker::detail::stack_obj::get_top(m_pLua);
        if(func.is_function() == false)
        {
            func.remove();
            return RVal();
        }

        return lua_tinker::call_stackfunc<RVal>(m_pLua, std::forward<Args>(args)...);
        __LEAVE_FUNCTION
        return RVal();
    }

public:
    template<typename RVal, typename... Args>
    RVal _ExecScript(const char* pszFuncName, Args&&... args)
    {
        __ENTER_FUNCTION
        if(pszFuncName == nullptr)
            return RVal();

        return lua_tinker::call<RVal>(m_pLua, pszFuncName, std::forward<Args>(args)...);
        __LEAVE_FUNCTION
        return RVal();
    }

private:
    const lua_tinker::table_ref* QueryScriptTable(const std::string& table_name) const;

private:
    lua_State* m_pLua;
    int32_t    m_nLuaGCStep;      //每次GCStep执行几步GC
    int32_t    m_nLuaGCStepTick;  //每多少Tick执行1次GCStep
    time_t     m_tNextGCStepTime; //下1次执行GC的Time

    std::string      m_search_path;
    std::string      m_main_file_name;
    InitRegisterFunc m_pInitRegisterFunc;
    void*            m_pInitParam;

private:
    std::unordered_map<uint32_t, std::unordered_set<uint64_t> > m_Data;
    std::unordered_map<uint32_t, std::string>                   m_ScriptTableName;

    std::unordered_map<std::string, lua_tinker::table_ref> m_script_table;
};

/*
lua_tinker使用：
因为lua_tinker对于引用的解析有部分问题，可能导致解析为传值，所以def函数时请避免

//向lua中设置一个全局变量,尽量不要,保持脚本干净
lua_tinker::set(L, "g_test", &g_test);
//从lua中获取一个全局变量
int32_t lua_int = lua_tinker::get<int32_t>(L, "lua_int");

//将test函数注册给lua,命名为ctest
lua_tinker::def(L, "ctest", test);

//调用lua中的function test,参数为1,返回void
lua_tinker::call<void>(L, "test", 1);

//向lua中注册一个类LUAScript
lua_tinker::class_add<LUAScript>(m_pLua, "LUAScript");
//向lua中注册一个类LUAScript2,继承自LUAScript
lua_tinker::class_inh<LUAScript2, LUAScript>(L);
//注册类构造函数,一般来说没必要在lua中产生c++对象,因为内存管理不同步,
//可以用在struct上,比如Vector2,Point之类的
lua_tinker::class_con<Vector2>(L,lua_tinker::constructor<int32_t,int32_t>());
//向lua中注册一个类成员函数LUAScript::Reload
lua_tinker::class_def<LUAScript>(L, "Reload", &LUAScript::Reload);
//向lua中注册一个成员变量
lua_tinker::class_mem<LUAScript>(L, "m_pLua", &LUAScript::m_pLua);


*/
/*
注册函数使用:

void InitFreeFunction(lua_State* L)
{
    lua_tinker::def(L, "rand", &rand);
    lua_tinker::def(L, "TimdDiff", &TimdDiff);
    lua_tinker::def(L, "TimeGet", &TimeGet);
}
void InitUser(lua_State* L)
{
    lua_tinker::class_add<CRole>(L, "CRole");
    lua_tinker::class_def<CRole>(L, "Attack", &CRole::Attack);
    lua_tinker::class_add<CUser>(L, "CUser");
    lua_tinker::class_inh<CUser, CRole>(L);
    lua_tinker::class_def<CUser>(L, "Attack", &CUser::Attack);

    typedef void(CUser::*overload_func1)(const char*);
    typedef void(CUser::*overload_func2)(std::string);

    lua_tinker::class_def<CUser>(L, "SetName", (overload_func1)&CUser::SetName);
    lua_tinker::class_def<CUser>(L, "SetNameString", (overload_func2)&CUser::SetName);
}
void InitRegisterFunction(lua_State* L)
{
   lua_tinker::def(L, "InitFreeFunc", &InitFreeFunc);
   lua_tinker::def(L, "InitUser", &InitUser);
   lua_tinker::def(L, "InitNpc", &InitNpc);
   lua_tinker::def(L, "InitSceneService", &InitSceneService);
}
 CreateNew<LUAScriptManager>(&InitRegisterFunction);

*/
/*
task_example.lua

--当KillNpc时c++会回调该函数,传入的参数为CTaskData*,CUser*,CRole*三个指针
function OnKillNpc(pTask,pUser,pDeadRole)
    if pDeadRole:GetID() == 30113310 then
        local data = pTask:GetData(0);		--所有变量前必须加local作为局部变量，一般情况下不允许使用全局变量
        pTask:SetData(0,  data + 1);		--调用成员函数
        pTask.state = 1;					--设置成员变量state=1
    end
end
*/

#endif // script_manager_h__
