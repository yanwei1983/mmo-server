#include "BTNode.h"
#include "BehaviorTree.h"
#include "LoggingMgr.h"
namespace BT
{
    BTNodePtr BTNode::clone() const
    {
        __ENTER_FUNCTION
        BTNodePtr new_node      = BTFactory::Create(GetNodeType());
        new_node->m_ScriptFunc = m_ScriptFunc;
        new_node->m_Desc = m_Desc;
        new_node->m_Script = m_Script;
        new_node->m_script_onenter_lua = m_script_onenter_lua;
        new_node->m_script_onleave_lua = m_script_onleave_lua;
        new_node->m_script_onupdate_lua = m_script_onupdate_lua;
        return new_node;
        __LEAVE_FUNCTION
        return {};
    }
    
    bool BTNode::deserialize(const json& js_doc, lua_State* m_pLua)
    {
        __ENTER_FUNCTION
        if(js_doc["type"] != GetNodeType())
            return false;

        m_Desc   = js_doc["desc"];
        std::string script_onenter;
        std::string script_onleave;
        std::string script_onupdate;
        if(js_doc.contains("script_onenter"))
        {
            for(const auto& v: js_doc["script_onenter"])
            {
                script_onenter += "\t";
                script_onenter += v;
                script_onenter += "\n";
            }
        }
        if(js_doc.contains("script_onleave"))
        {
            for(const auto& v: js_doc["script_onleave"])
            {
                script_onleave += "\t";
                script_onleave += v;
                script_onleave += "\n";
            }
        }
        if(js_doc.contains("script_onupdate"))
        {
            for(const auto& v: js_doc["script_onupdate"])
            {
                script_onupdate += "\t";
                script_onupdate += v;
                script_onupdate += "\n";
            }
        }

        if(script_onenter.empty() == false)
        {
            static const std::string fmt_str_enter =
                R"(
function this_ainode.OnEnter(ctx,node)
    local ai = ctx:GetAI();
{}
end
            )";
            m_script_onenter_lua = fmt::format(fmt_str_enter, script_onenter);
        }
        if(script_onleave.empty() == false)
        {
            static const std::string fmt_str_leave =
                R"(
function this_ainode.OnLeave(state,ctx,node)
    local ai = ctx:GetAI();
{}
end
            )";
            m_script_onleave_lua = fmt::format(fmt_str_leave, script_onleave);
        }
        if(script_onupdate.empty() == false)
        {
            static const std::string fmt_str_update =
                R"(
function this_ainode.OnUpdate(ctx,node)
    local ai = ctx:GetAI();
{}
end
            )";
            m_script_onupdate_lua = fmt::format(fmt_str_update, script_onupdate);
        }

        if(m_script_onenter_lua.empty() == false || m_script_onleave_lua.empty() == false || m_script_onupdate_lua.empty() == false)
        {

            static const std::string fmt_str =
                R"(
local this_ainode = {{}};
this_ainode.__desc="{}";
{}
{}
{}
return function (name,...)
	local func = this_ainode[name];
	return func(...);
end
            )";

            m_Script = fmt::format(fmt_str, m_Desc, m_script_onenter_lua, m_script_onleave_lua, m_script_onupdate_lua);
            //将脚本转化为lua_function
            m_ScriptFunc = lua_tinker::dostring<lua_tinker::lua_function_ref>(m_pLua, m_Script, m_Script.c_str());
        }
        return true;
        __LEAVE_FUNCTION
        return false;
    }

    Status BTNode::tick(BTContext& ctx)
    {
        __ENTER_FUNCTION
        if(m_Status != Status::RUNNING)
        {
            m_Status = OnEnter(ctx);
            if(m_Status != Status::SUCCESS)
            {
                return m_Status;
            }
        }       

        m_Status = OnUpdate(ctx);

        if(m_Status != Status::RUNNING)
        {
            OnLeave(ctx);
            if(ctx.GetLastRunningNode() == this)
              ctx.SetLastRunningNode(nullptr);
        }
        else
        {
            if(ctx.GetLastRunningNode() == nullptr)
                ctx.SetLastRunningNode(this);
        }
        

        return m_Status;
        __LEAVE_FUNCTION
        return Status::FAILURE;
    }

    Status BTNode::OnUpdate(BTContext& ctx)
    {
        __ENTER_FUNCTION
        
        if(m_script_onupdate_lua.empty() == false)
        {
            auto result = m_ScriptFunc.invoke<Status>("OnUpdate", ctx, this);
            LOGAITRACE(ctx.IsDebug(), ctx.GetActorID(), "BTNoode: OnUpdate {} {}", m_Desc, magic_enum::enum_name(result));
            return result;
        }
        return m_Status;
        __LEAVE_FUNCTION
        return Status::FAILURE;
    }

    Status BTNode::OnEnter(BTContext& ctx)
    {
        __ENTER_FUNCTION
        
        if(m_script_onenter_lua.empty() == false)
        {
            auto result = m_ScriptFunc.invoke<Status>("OnEnter", ctx, this);
            LOGAITRACE(ctx.IsDebug(), ctx.GetActorID(), "BTNoode: OnEnter {} {}", m_Desc, magic_enum::enum_name(result));
            return result;
        }
        __LEAVE_FUNCTION
        return Status::SUCCESS;
    }

    void BTNode::OnLeave(BTContext& ctx)
    {
        __ENTER_FUNCTION
        
        if(m_script_onleave_lua.empty() == false)
        {
            LOGAITRACE(ctx.IsDebug(), ctx.GetActorID(), "BTNoode: OnLeave {}", m_Desc, magic_enum::enum_name(m_Status));
            m_ScriptFunc.invoke<void>("OnLeave", m_Status, ctx, this);
        }
        __LEAVE_FUNCTION
    }

} // namespace BT