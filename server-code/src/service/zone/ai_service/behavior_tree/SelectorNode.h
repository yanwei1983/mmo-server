#ifndef SELECTORNODE_H
#define SELECTORNODE_H

#include "CompositeNode.h"
namespace BT
{
    class SelectorNode : public CompositeNode
    {
    public:
        virtual ~SelectorNode() {}

        virtual std::string GetNodeType()const override {return "SelectorNode";}
    protected:
        virtual Status OnEnter(BTContext& ctx) override;
        virtual Status OnUpdate(BTContext& ctx) override;
    private:
        size_t m_nCurNode = 0;
    };
}
#endif /* SELECTORNODE_H */
