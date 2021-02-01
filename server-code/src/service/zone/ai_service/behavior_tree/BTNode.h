#ifndef BTNODE_H
#define BTNODE_H

#include "json.hpp"
#include "lua_tinker.h"
#include "export_lua.h"

#include <unordered_map>
#include <variant>

struct lua_State;
class CAIActor;
class CActorAI;

export_lua namespace BT
{

    using json = nlohmann::json;
    export_lua enum class Status
    {
        FAILURE,
        SUCCESS,
        RUNNING,
        ABORTED,
        INVALID
    };
    class BTNode;
    export_lua struct BTContext
    {
    public:
        BTContext(BTNode*    pRootNode, lua_State* pLua, CAIActor*  pActor, CActorAI*  pAI, uint64_t idActor, bool bDebug)
        :m_RootNode(pRootNode),m_pActor(pActor), m_pAI(pAI), m_idActor(idActor), m_bDebug(bDebug)
        ,m_lua_table( lua_tinker::table_ref::make_table_ref(lua_tinker::table_onstack(pLua)) )
        {
             
            
        }

        BTNode* GetRootNode()const {return m_RootNode;}
        BTNode* GetLastRunningNode()const {return m_LastRunningNode;}
        void SetLastRunningNode(BTNode* node){m_LastRunningNode = node;}

        export_lua CAIActor* GetActor() const { return m_pActor; }
        export_lua CActorAI* GetAI() const { return m_pAI; }
        export_lua uint64_t  GetActorID() const { return m_idActor; }
        export_lua bool      IsDebug() const { return m_bDebug; }

        export_lua lua_tinker::table_onstack data_table() const {return m_lua_table.push_table_to_stack();}
    private:
        BTNode*    m_RootNode;
        CAIActor*  m_pActor;
        CActorAI*  m_pAI;
        uint64_t   m_idActor;
        bool       m_bDebug;
        
        BTNode*    m_LastRunningNode = nullptr;

        lua_tinker::table_ref m_lua_table;
    };
    using BTContextPtr = std::unique_ptr<BTContext>;

    class BTNode;
    using BTNodePtr = std::unique_ptr<BTNode>;
    export_lua class BTNode
    {
    public:
        BTNode()
        {
        }
        BTNode(const json& js_doc)
        {

        }
        virtual ~BTNode() {}

        virtual BTNodePtr clone() const;
        virtual bool deserialize(const json& js_doc, lua_State* m_pLua);

        virtual Status tick(BTContext& ctx);

        export_lua void reset() { m_Status = Status::INVALID; }

        export_lua void abort(BTContext& ctx)
        {
            m_Status = Status::ABORTED;
            OnLeave(ctx);
        }

        export_lua bool IsSuccessed() const { return m_Status == Status::SUCCESS; }
        export_lua bool IsFailured() const { return m_Status == Status::FAILURE; }
        export_lua bool IsTerminated() const { return m_Status == Status::SUCCESS || m_Status == Status::FAILURE; }
        export_lua bool IsRunning() const { return m_Status == Status::RUNNING; }

        export_lua Status GetStatus() const { return m_Status; }

        export_lua std::string GetDesc() const { return m_Desc; }
        export_lua void SetDesc(const std::string& v){m_Desc = v;}

        export_lua virtual size_t GetChildCount() const {return 0;}
        export_lua virtual BTNode* GetChild(size_t nIdx)const { return nullptr;}
    protected:
        virtual Status OnUpdate(BTContext& ctx);
        virtual Status OnEnter(BTContext& ctx);
        virtual void OnLeave(BTContext& ctx);

        virtual std::string GetNodeType()const = 0;
    protected:
       
        Status m_Status = Status::INVALID;
        lua_tinker::lua_function_ref   m_ScriptFunc;
        std::string m_Desc;     
        std::string m_Script;   
        std::string m_script_onenter_lua;
        std::string m_script_onleave_lua;
        std::string m_script_onupdate_lua;

    };
    


    class ActionNode : public BTNode
    {
    public:
        virtual std::string GetNodeType()const override {return "ActionNode";}
    };
    
    class ConditionNode : public BTNode
    {
    public:
        virtual std::string GetNodeType()const override {return "ConditionNode";}
    };

    
}


#endif /* BTNODE_H */
