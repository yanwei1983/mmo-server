#include "SequenceNode.h"
#include "BehaviorTree.h"


namespace BT
{
    Status SequenceNode::OnEnter(BTContext& ctx)
    {
        m_nCurNode = 0;
        return CompositeNode::OnEnter(ctx);
    }

    Status SequenceNode::OnUpdate(BTContext& ctx)
    {
        __ENTER_FUNCTION
        for(;;)
        {
            auto   node = GetChild(m_nCurNode);
            Status s    = node->tick(ctx);

            if(s != Status::SUCCESS)
            {
                return s;
            }

            // have we run all the children?
            if(++m_nCurNode >= GetChildCount())
            {
                return Status::SUCCESS;
            }
        }
        __LEAVE_FUNCTION
        return Status::FAILURE;
    }

} // namespace BT