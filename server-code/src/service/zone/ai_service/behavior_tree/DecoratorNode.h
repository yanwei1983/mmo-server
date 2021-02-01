#ifndef DECORATORNODE_H
#define DECORATORNODE_H

#include "BTNode.h"
namespace BT
{
    export_lua class DecoratorNode : public BTNode
    {
    protected:
        BTNodePtr m_Child;

    public:
        DecoratorNode()
        {
        }
        virtual std::string GetNodeType()const override {return "DecoratorNode";}

        virtual BTNodePtr clone() const override;

        virtual bool deserialize(const json& js_doc, lua_State* m_pLua) override;

        export_lua virtual size_t GetChildCount() const override;
        export_lua virtual BTNode* GetChild(size_t nIdx)const override;
    };



}

#endif /* DECORATORNODE_H */
