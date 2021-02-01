    #ifndef BEHAVIORTREE_H
#define BEHAVIORTREE_H

#include <memory>
#include <unordered_map>
#include <string>

#include "CompositeNode.h"
#include "DecoratorNode.h"
#include "ParallelNode.h"
#include "SelectorNode.h"
#include "SequenceNode.h"

#include "BaseCode.h"
#include "Noncopyable.h"

namespace BT
{

    class BTTree : public SelectorNode
    {
    public:
        BTTree() {}
        virtual ~BTTree() {}
        virtual std::string GetNodeType() const override { return "BTTree"; }
        virtual Status tick(BTContext& ctx) override
        {
            if(ctx.GetLastRunningNode() != nullptr && ctx.GetLastRunningNode() != this)
            {
                auto status = ctx.GetLastRunningNode()->tick(ctx);
                if(status == Status::RUNNING)
                    return status;
            }
            
            return SelectorNode::tick(ctx);
        }
    };
    using BTTreePtr = std::unique_ptr<BTTree>;
    
    struct BTContext;
    using BTContextPtr = std::unique_ptr<BTContext>;

    class BTNode;
    using BTNodePtr = std::unique_ptr<BTNode>;

    class BTFactory
    {
    public:
        static BTNodePtr Create(const std::string& name);
    };

    class BTManager
    {
        BTManager()
        {}
        bool Init(const std::string& path, lua_State* pLua, const std::unordered_set<std::string>& all_bt_file);
    public:
        ~BTManager()
        {}
        CreateNewImpl(BTManager);

        void load_all(const std::string& path, lua_State* pLua);
        void load_file(const std::string& path, const std::string& filename, lua_State* pLua);
        BTTreePtr clone(const std::string& filename);
    private:
        std::unordered_map<std::string, BTTreePtr> m_TemplateBTTree;
    };




    



    

} // namespace BT

#endif /* BEHAVIORTREE_H */
