#include "ParallelNode.h"
#include "BehaviorTree.h"
#include "IntUtil.h"

namespace BT
{
    BTNodePtr ParallelNode::clone() const
    {
        __ENTER_FUNCTION
        auto new_node             = CompositeNode::clone();
        auto new_parallel_ndoe = static_cast<ParallelNode*>(new_node.get());
        new_parallel_ndoe->m_SuccessPolicy = m_SuccessPolicy;
        new_parallel_ndoe->m_FailurePolicy = m_FailurePolicy;
        return new_node;
        __LEAVE_FUNCTION
        return {};
    }

    bool ParallelNode::deserialize(const json& js_doc, lua_State* m_pLua) 
    {
        __ENTER_FUNCTION
        if(CompositeNode::deserialize(js_doc, m_pLua) == false)
            return false;
        if(js_doc.contains("SuccessPolicy"))
        {
            m_SuccessPolicy = to_enum<Policy>(js_doc["SuccessPolicy"]);
        }
        if(js_doc.contains("FailurePolicy"))
        {
            m_FailurePolicy = to_enum<Policy>(js_doc["FailurePolicy"]);
        }
        return true;
        __LEAVE_FUNCTION
        return false;
    }

    Status ParallelNode::OnUpdate(BTContext& ctx)
    {
        __ENTER_FUNCTION
        size_t iSuccessCount = 0;
        size_t iFailureCount = 0;

        for(const auto& pNode: m_ChildNode)
        {
            auto& node = *pNode;
            if(!node.IsTerminated())
            {
                node.tick(ctx);
            }

            if(node.GetStatus() == Status::SUCCESS)
            {
                ++iSuccessCount;
                if(m_SuccessPolicy == Policy::REQUIRE_ONE)
                {
                    return Status::SUCCESS;
                }
            }

            if(node.GetStatus() == Status::FAILURE)
            {
                ++iFailureCount;
                if(m_FailurePolicy == Policy::REQUIRE_ONE)
                {
                    return Status::FAILURE;
                }
            }
        }

        if(m_FailurePolicy == Policy::REQUIRE_ALL && iFailureCount == m_ChildNode.size())
        {
            return Status::FAILURE;
        }

        if(m_SuccessPolicy == Policy::REQUIRE_ALL && iSuccessCount == m_ChildNode.size())
        {
            return Status::SUCCESS;
        }

        
        return Status::RUNNING;
        __LEAVE_FUNCTION
        return Status::FAILURE;
    }

    void ParallelNode::OnLeave(BTContext& ctx)
    {
        for(const auto& pNode: m_ChildNode)
        {
            auto& node = *pNode;
            if(node.IsRunning())
            {
                node.abort(ctx);
            }
        }
    }

} // namespace BT