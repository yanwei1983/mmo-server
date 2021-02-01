#ifndef COMPOSITENODE_H
#define COMPOSITENODE_H

#include <vector>

#include "BTNode.h"
namespace BT
{
    export_lua class CompositeNode : public BTNode
    {
    public:
        virtual std::string GetNodeType()const override {return "CompositeNode";}
        virtual BTNodePtr clone() const override;
  

        virtual bool deserialize(const json& js_doc, lua_State* m_pLua) override;

        export_lua virtual size_t GetChildCount() const override ;
        export_lua virtual BTNode* GetChild(size_t nIdx)const override ;

    protected:
        void AddChild(BTNodePtr&& child);

    protected:
        using NodeList=std::vector<BTNodePtr>;
        NodeList m_ChildNode;
    };
}
#endif /* COMPOSITENODE_H */
