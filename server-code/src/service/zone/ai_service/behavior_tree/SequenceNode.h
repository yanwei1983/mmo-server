#ifndef SEQUENCENODE_H
#define SEQUENCENODE_H

#include "CompositeNode.h"
namespace BT
{
    class SequenceNode : public CompositeNode
    {
    public:
        virtual ~SequenceNode() {}
        virtual std::string GetNodeType()const override {return "SequenceNode";}
    protected:
        virtual Status OnEnter(BTContext& ctx) override;
        virtual Status OnUpdate(BTContext& ctx) override;
    private:
        size_t m_nCurNode = 0;
    };


}

#endif /* SEQUENCENODE_H */
