#include "SelectorNode.h"
#include "BehaviorTree.h"

namespace BT
{
    Status SelectorNode::OnEnter(BTContext& ctx) 
    { 
        m_nCurNode = 0; 
        return CompositeNode::OnEnter(ctx);
    }

    Status SelectorNode::OnUpdate(BTContext& ctx)
    {
        __ENTER_FUNCTION
        for(;;)
        {
            auto   node = GetChild(m_nCurNode);
            Status s    = node->tick(ctx);

            if(s != Status::FAILURE)
            {
                return s;
            }

            if(++m_nCurNode >= GetChildCount())
            {
                return Status::FAILURE;
            }
        }
        __LEAVE_FUNCTION
        return Status::FAILURE;
    }

} // namespace BT