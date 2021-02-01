#ifndef BTNODE_H
#define BTNODE_H

#include <string>
#include <unordered_map>

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "imnode/imnodes.h"
#include "json.hpp"


struct BtTypeInfo
{
    std::string_view type;
    std::string_view type_nick;
    int         max_child_num;
};

 constexpr BtTypeInfo g_bttype_infos[] = {  {"BTTree", "root", -1},
                                            {"ActionNode", "Act", 0},
                                            {"ConditionNode","Cond", 0},
                                            {"DecoratorNode","Dec", 1},
                                            {"ParallelNode","Parall", -1},
                                            {"SelectorNode", "Sel",-1},
                                            {"SequenceNode", "Seq",-1}};


class BTTreeEditor;
class btnode
{
public:
    btnode(BTTreeEditor* owner,const nlohmann::json& js_doc);
    btnode(BTTreeEditor* owner, uint32_t type, int32_t max_child_num);
    btnode* clone() const;
    void OnShow();
    int32_t GetNodeID() const{return m_id;}
    int32_t GetParentNodeID() const{return m_parent_id;}
    bool IsParentChain(int32_t node_id) const;
    size_t GetChildCount()const{return m_child_count;}
    void AddLink(int32_t child_node_id);
    void RemoveLink(int32_t link_id);
    void RemoveAllLink();
    void DrawLink();
    void RemoveChildNode(int32_t child_node_id);
    void DownLink(int32_t child_node_id);
    void UpperLink(int32_t child_node_id);

    void SetParentNodeID(int32_t v){m_parent_id = v;}
    void SetLinkID(int32_t v){m_link_id = v;}


    void serialize(nlohmann::json& js_doc)const;
    void deserialize(const nlohmann::json& js_doc);

    void Tidy(const ImVec2& new_pos, bool vertical);
    void foreach(const std::function<void(int32_t)>& func) const;
    std::tuple<ImVec2,ImVec2> GetAABB() const;
    void DisplayChild();
private:
    BTTreeEditor* m_pOwner;
    uint32_t m_type;
    std::string m_desc;
    std::string m_script_OnUpdate;
    std::string m_script_OnEnter;
    std::string m_script_OnLeave;
    int m_SuccessPolicy = 0;
    int m_FailurePolicy = 0;
    int m_max_child_num;
    int32_t m_id;
    int32_t m_id_input;
    int32_t m_id_static;
    int32_t m_id_output;
    int32_t m_linkidx;

    int32_t m_parent_id = 0;
    int32_t m_link_id = 0;
    int32_t m_child_count = 0;
    std::map<int32_t, int32_t> m_link;

    bool m_display_body = false;
    bool m_display = true;
    bool m_display_child = true;

    ImVec2 m_pos;
};

#endif /* BTNODE_H */
