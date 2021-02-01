#include "DecoratorNode.h"
#include "BehaviorTree.h"

namespace BT
{
    BTNodePtr DecoratorNode::clone() const
    {
        __ENTER_FUNCTION
        BTNodePtr new_node = BTNode::clone();
        auto new_decorator_ndoe = static_cast<DecoratorNode*>(new_node.get());
        new_decorator_ndoe->m_Child  = m_Child->clone();
        return new_node;
        __LEAVE_FUNCTION
        return {};
    }

    bool DecoratorNode::deserialize(const json& js_doc, lua_State* m_pLua) 
    {
        __ENTER_FUNCTION
        if(BTNode::deserialize(js_doc, m_pLua) == false)
            return false;

        if(js_doc.contains("child"))
        {
            const auto& child = js_doc["child"];
            if(child.contains("type") == false)
            {
                LOGERROR("{} 's child no type", m_Desc);
                return false;
            }
            BTNodePtr   pNode = BTFactory::Create(child["type"]);
            CHECKF(pNode);
            CHECKF(pNode->deserialize(child, m_pLua));
            m_Child.reset(pNode.release());
        }
        return true;
        __LEAVE_FUNCTION
        return false;
    }

    size_t  DecoratorNode::GetChildCount() const { return 1; }
    BTNode* DecoratorNode::GetChild(size_t nIdx) const { return m_Child.get(); }
} // namespace BT