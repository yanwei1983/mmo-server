#include "CompositeNode.h"
#include "BehaviorTree.h"

namespace BT
{
    BTNodePtr CompositeNode::clone() const
    {
        __ENTER_FUNCTION
        BTNodePtr new_node = BTNode::clone();
        auto new_composite_ndoe = static_cast<CompositeNode*>(new_node.get());
        for(const auto& child: m_ChildNode)
        {
            new_composite_ndoe->AddChild(child->clone());
        }
        return new_node;
        __LEAVE_FUNCTION
        return {};
    }

    bool CompositeNode::deserialize(const json& js_doc, lua_State* m_pLua) 
    {
        __ENTER_FUNCTION
        if(BTNode::deserialize(js_doc, m_pLua) == false)
            return false;

        if(js_doc.contains("child"))
        {
            for(const json& child: js_doc["child"])
            {
                if(child.contains("type") == false)
                {
                    LOGERROR("{} 's child no type", m_Desc);
                    continue;
                }
                std::string type_str = child["type"];
                BTNodePtr pNode = BTFactory::Create(type_str);
                CHECKF(pNode);
                CHECKF(pNode->deserialize(child, m_pLua));
                AddChild(std::move(pNode));
            }
        }
        return true;
        __LEAVE_FUNCTION
        return false;
    }

    size_t  CompositeNode::GetChildCount() const { return m_ChildNode.size(); }
    BTNode* CompositeNode::GetChild(size_t nIdx) const
    {
        CHECKF(nIdx < GetChildCount());
        return m_ChildNode[nIdx].get();
    }

    void CompositeNode::AddChild(BTNodePtr&& child) { m_ChildNode.emplace_back(std::move(child)); }
} // namespace BT