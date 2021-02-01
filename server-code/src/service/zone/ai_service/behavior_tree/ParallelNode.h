#ifndef PARALLELNODE_H
#define PARALLELNODE_H

#include "CompositeNode.h"

namespace BT
{
    class ParallelNode : public CompositeNode
    {
    public:
        enum class Policy
        {
            REQUIRE_ONE,
            REQUIRE_ALL
        };

        ParallelNode()
        {
        }

        virtual ~ParallelNode() {}
        virtual std::string GetNodeType()const override {return "ParallelNode";}

        virtual BTNodePtr clone() const override;

        virtual bool deserialize(const json& js_doc, lua_State* m_pLua) override;
    protected:
        virtual Status OnUpdate(BTContext& ctx) override;
        virtual void OnLeave(BTContext& ctx) override;
    protected:
        Policy m_SuccessPolicy = Policy::REQUIRE_ONE;
        Policy m_FailurePolicy = Policy::REQUIRE_ONE;


    };
}

#endif /* PARALLELNODE_H */
