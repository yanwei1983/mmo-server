#include "BehaviorTree.h"
#include <fstream>
#include "FileUtil.h"

namespace BT
{


    BTNodePtr BTFactory::Create(const std::string& name)
    {
        __ENTER_FUNCTION
        static std::unordered_map<std::string, std::function<BTNode*()> > s_Factory = {
            {"BTTree",
             []() {
                 return new BTTree;
             }},
            {"ActionNode",
             []() {
                 return new ActionNode;
             }},
            {"ConditionNode",
             []() {
                 return new ConditionNode;
             }},
            {"DecoratorNode",
             []() {
                 return new DecoratorNode;
             }},
            {"ParallelNode",
             []() {
                 return new ParallelNode;
             }},
            {"SelectorNode",
             []() {
                 return new SelectorNode;
             }},
            {"SequenceNode",
             []() {
                 return new SequenceNode;
             }},
        };

        auto it = s_Factory.find(name);
        if(it == s_Factory.end())
        {
            LOGERROR("BTFactory::Create fail: {}", name);
            return nullptr;
        } 

        return BTNodePtr{it->second()};
        __LEAVE_FUNCTION
        return nullptr;
    }

    bool BTManager::Init(const std::string& path, lua_State* pLua, const std::unordered_set<std::string>& all_bt_file)
    {
        __ENTER_FUNCTION

        for(const auto& bt_file : all_bt_file) 
        {
            load_file(path, bt_file, pLua);
        }
        //load_all(path, pLua);
        return true;
        __LEAVE_FUNCTION
        return false;
    }

    void BTManager::load_all(const std::string& path, lua_State* pLua)
    {
        __ENTER_FUNCTION
        scan_dir(path, "", true, [this, pLua](const std::string& dir, const std::string& file_name) { load_file(dir,file_name, pLua); });
        __LEAVE_FUNCTION
    }

    void BTManager::load_file(const std::string& path, const std::string& filename, lua_State* pLua)
    {
        __ENTER_FUNCTION
        std::ifstream infile(path + "/" + filename);
        auto          js_doc = json::parse(infile, nullptr, false, true);
        if(js_doc.is_discarded())
            return;
        BTTreePtr pTree = std::make_unique<BTTree>();
        CHECK(pTree->deserialize(js_doc["root_node"], pLua));
        LOGDEBUG("BTManager::load_file: {} succ.", filename);
        m_TemplateBTTree.emplace(filename, std::move(pTree));
        return;
        __LEAVE_FUNCTION
        LOGDEBUG("BTManager::load_file: {} fail.", filename);
    }
    BTTreePtr BTManager::clone(const std::string& filename)
    {
        __ENTER_FUNCTION
        auto it = m_TemplateBTTree.find(filename);
        if(it == m_TemplateBTTree.end())
            return {};
        auto node = it->second->clone();
        BTTreePtr tree_node(static_cast<BTTree*>(node.release()));
        return tree_node;
        __LEAVE_FUNCTION
        return {};
    }

} // namespace BT