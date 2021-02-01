#include "btnode.h"
#include "LoopHelper.h"
#include "StringAlgo.h"
#include "btree_editor.h"
#include "json.hpp"



btnode::btnode(BTTreeEditor* owner, const nlohmann::json& js_doc)
    : m_pOwner(owner)
{
    deserialize(js_doc);
}

btnode::btnode(BTTreeEditor* owner, uint32_t type, int32_t max_child_num)
    : m_pOwner(owner)
    , m_id(owner->GenNodeIdx() * 100)
    , m_id_input(m_id + 1)
    , m_id_static(m_id + 2)
    , m_id_output(m_id + 3)
    , m_linkidx(m_id + 4)
    , m_type(type)
    , m_max_child_num(max_child_num)

{
}

btnode* btnode::clone() const
{
    btnode* node = new btnode(m_pOwner, m_type, m_max_child_num);
    node->m_desc = m_desc;
    node->m_script_OnUpdate = m_script_OnUpdate;
    node->m_script_OnEnter = m_script_OnEnter;
    node->m_script_OnLeave = m_script_OnLeave;
    return node;
}

void btnode::OnShow()
{
    if(m_type != 0 && m_display == false)
    {
        imnodes::BeginNode(m_id, m_display);
        imnodes::EndNode();
        return;
    }

    ImFont* pChineseFont = ImGui::GetIO().Fonts->Fonts[1];
    imnodes::BeginNode(m_id);
    ImGui::PushItemWidth(-FLT_MIN);
    
    imnodes::BeginNodeTitleBar();
    if(ImGui::SmallButton(g_bttype_infos[m_type].type_nick.data()) == true)
    {
        m_display_body = !m_display_body;
    }
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
    {
        auto node_pos = imnodes::GetNodeGridSpacePos(m_id);
        ImGui::SetTooltip("id:%d pos:%d,%d", m_id,(int)node_pos.x, (int)node_pos.y);
    }
    
    if(GetChildCount() > 0)
    {
        if(m_display_child)
        {
            ImGui::SameLine();
            if(ImGui::ArrowButton("display_child", ImGuiDir_Right))
            {
                DisplayChild();
            }
        }
        else 
        {
            ImGui::SameLine();
            if(ImGui::ArrowButton("display_child", ImGuiDir_Up))
            {
                DisplayChild();
            }
        }
    }

    if(m_desc.empty() == false)
    {
        ImGui::PushFont(pChineseFont);
        ImGui::Text(m_desc.c_str());
        ImGui::PopFont();
    }
    imnodes::EndNodeTitleBar();

    if(m_type != 0)
    {
        imnodes::BeginInputAttribute(m_id_input);
        auto child_idx = (m_link_id % 100) - 3;
        ImGui::Text("linkid:%d", child_idx);
        imnodes::EndInputAttribute();
    }

    imnodes::BeginStaticAttribute(m_id_static);

    if(m_display_body)
    {
        if(ImGui::TreeNode("desc"))
        {
            ImGui::PushFont(pChineseFont);
            ImGui::InputText("##desc", &m_desc);
            ImGui::PopFont();

            ImGui::TreePop();
        }
        if(ImGui::TreeNode("OnEnter", "OnEnter(%d)", m_script_OnEnter.size()))
        {
            
            ImGui::PushFont(pChineseFont);
            ImGui::InputTextMultiline("##script", &m_script_OnEnter, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);
            ImGui::PopFont();

            ImGui::TreePop();
        }
        if(ImGui::TreeNode("OnUpdate", "OnUpdate(%d)", m_script_OnUpdate.size()))
        {
            
            ImGui::PushFont(pChineseFont);
            ImGui::InputTextMultiline("##script", &m_script_OnUpdate, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);
            ImGui::PopFont();

            ImGui::TreePop();
        }
        if(ImGui::TreeNode("OnLeave", "OnLeave(%d)", m_script_OnLeave.size()))
        {
            
            ImGui::PushFont(pChineseFont);
            ImGui::InputTextMultiline("##script", &m_script_OnLeave, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);
            ImGui::PopFont();

            ImGui::TreePop();
        }

        if(std::string(g_bttype_infos[m_type].type) == "ParallelNode")
        {
            if(ImGui::TreeNode("SuccessPolicy"))
            {
                
                ImGui::RadioButton("AnySucc", &m_SuccessPolicy,0);
                ImGui::SameLine();
                ImGui::RadioButton("AllSucc", &m_SuccessPolicy,1);
                

                ImGui::TreePop();
            }
            if(ImGui::TreeNode("m_FailurePolicy"))
            {
                
                ImGui::RadioButton("AnyFail", &m_FailurePolicy,0);
                ImGui::SameLine();
                ImGui::RadioButton("AllFail", &m_FailurePolicy,1);

                ImGui::TreePop();
            }
        }
    }
    

    imnodes::EndStaticAttribute();

    if(m_max_child_num != 0)
    {
        imnodes::BeginOutputAttribute(m_id_output);
        ImGui::Text("child:%d", m_child_count);
        imnodes::EndOutputAttribute();
    }
    ImGui::PopItemWidth();

    imnodes::EndNode();

    for(auto& [k, child_node_id]: m_link)
    {
        if(child_node_id != 0)
        {
            auto child_node = m_pOwner->QueryNode(child_node_id);
            child_node->OnShow();
        }
    }
}

void btnode::DisplayChild()
{
    auto node_pos = imnodes::GetNodeGridSpacePos(m_id);

    m_display_child = !m_display_child;
    for(auto& [k, child_node_id]: m_link)
    {
        if(child_node_id != 0)
        {
            auto child_node = m_pOwner->QueryNode(child_node_id);
            if(child_node->m_display)
            {
                //record pos
                auto child_node_pos = imnodes::GetNodeGridSpacePos(child_node_id);
                child_node->m_pos = ImVec2(child_node_pos.x - node_pos.x, child_node_pos.y - node_pos.y);
            }
            else
            {
                //restore pos
                auto child_node_pos = ImVec2(child_node->m_pos.x + node_pos.x, child_node->m_pos.y + node_pos.y);
                imnodes::SetNodeGridSpacePos(child_node_id, child_node_pos);
            }
            child_node->m_display = !child_node->m_display;
            child_node->DisplayChild();
        }
    }
}

bool btnode::IsParentChain(int32_t node_id) const
{
    int32_t parent_id = GetParentNodeID();
    while(parent_id != 0)
    {
        if(parent_id == node_id)
            return true;
        auto parent_node_ptr = m_pOwner->QueryNode(parent_id);
        if(parent_node_ptr == nullptr)
            return true;
        parent_id = parent_node_ptr->GetParentNodeID();
    }

    return false;
}

void btnode::foreach(const std::function<void(int32_t)>& func) const
{
    for(auto& [k, child_node_id]: m_link)
    {
        if(child_node_id != 0)
        {
            func(child_node_id);
        }
    }
    
}

void btnode::DownLink(int32_t child_node_id)
{
    auto child_node_ptr = m_pOwner->QueryNode(child_node_id);
    if(child_node_ptr == nullptr)
        return;
    if(child_node_ptr->GetParentNodeID() == 0)
        return;
    auto it_child = m_link.find(child_node_ptr->m_link_id);
    if(it_child == m_link.end()) 
        return;
    auto it_swap = it_child;
    std::advance(it_swap,1);
    auto swap_link_id = it_swap->first;
    auto swap_child_node_id = it_swap->second;

    RemoveLink(child_node_ptr->m_link_id);
    RemoveLink(swap_link_id);

    if(swap_child_node_id)
        AddLink(swap_child_node_id);
    AddLink(child_node_id);
    
    
}

void btnode::UpperLink(int32_t child_node_id)
{
    auto child_node_ptr = m_pOwner->QueryNode(child_node_id);
    if(child_node_ptr == nullptr)
        return;
    if(child_node_ptr->GetParentNodeID() == 0)
        return;
    auto it_child = m_link.find(child_node_ptr->m_link_id);
    if(it_child == m_link.begin()) 
        return;
    auto it_swap = it_child;
    std::advance(it_swap,-1);
    auto swap_link_id = it_swap->first;
    auto swap_child_node_id = it_swap->second;

    RemoveLink(child_node_ptr->m_link_id);
    RemoveLink(swap_link_id);

    AddLink(child_node_id);
    if(swap_child_node_id)
        AddLink(swap_child_node_id);
    
}

void btnode::AddLink(int32_t child_node_id)
{
    if(m_linkidx % 100 == 99)
        return;

    if(m_max_child_num > 0 && m_child_count >= m_max_child_num)
        return;

    auto    child_node_ptr = m_pOwner->QueryNode(child_node_id);
    if(child_node_ptr == nullptr)
        return;
    if(child_node_ptr->GetParentNodeID() != 0)
        return;
    //检查回环，即不允许任何自己的parent/parent的parent成为自己的link
    if(IsParentChain(child_node_id) == true)
        return;

    m_child_count++;
    child_node_ptr->SetParentNodeID(GetNodeID());
    for(auto& [k, v]: m_link)
    {
        if(v == 0)
        {
            v = child_node_id;
            child_node_ptr->SetLinkID(k);
            return;
        }
    }

    auto link_id = m_linkidx++;
    child_node_ptr->SetLinkID(link_id);
    m_link.emplace(link_id, child_node_id);
    
}

void btnode::RemoveChildNode(int32_t child_node_id)
{
    for(auto& [k, v]: m_link)
    {
        if(v == child_node_id)
        {
            v = 0;
            m_child_count--;
        }
    }
}

void btnode::RemoveLink(int32_t link_id)
{
    auto child_node_id  = m_link[link_id];
    auto child_node_ptr = m_pOwner->QueryNode(child_node_id);
    if(child_node_ptr == nullptr)
        return;
    child_node_ptr->SetParentNodeID(0);
    child_node_ptr->SetLinkID(0);
    m_link[link_id] = 0;
    m_child_count--;
}

void btnode::RemoveAllLink()
{
    for(auto& [k, child_node_id]: m_link)
    {
        if(child_node_id == 0)
            continue;
        auto child_node_ptr = m_pOwner->QueryNode(child_node_id);
        if(child_node_ptr == nullptr)
            continue;
        child_node_ptr->SetParentNodeID(0);
        child_node_ptr->SetLinkID(0);
        child_node_id = 0;
        m_child_count--;
    }
}

void btnode::DrawLink()
{
    if(m_display == false)
        return;
    if(m_display_child == false)
        return;
    for(auto [k, v]: m_link)
    {
        if(v != 0)
            imnodes::Link(k, m_id_output, v + 1);
    }

    for(auto& [k, child_node_id]: m_link)
    {
        if(child_node_id != 0)
        {
            auto child_node = m_pOwner->QueryNode(child_node_id);
            child_node->DrawLink();
        }
    }
}

std::tuple<ImVec2,ImVec2> btnode::GetAABB() const
{
    auto dimens = imnodes::GetNodeDimensions(GetNodeID());
    auto node_pos = imnodes::GetNodeGridSpacePos(GetNodeID());
    auto lt = node_pos;
    auto rb = ImVec2(node_pos.x+dimens.x,node_pos.y+dimens.y);
    foreach([this, &lt, &rb](auto child_node_id)
    {
        auto child_node = m_pOwner->QueryNode(child_node_id);
        const auto& [child_lt, child_rb] = child_node->GetAABB();
        lt.x = std::min(lt.x, child_lt.x);
        lt.y = std::min(lt.y, child_lt.y);
        rb.x = std::max(rb.x, child_rb.x);
        rb.y = std::max(rb.y, child_rb.y);
    });
    return std::make_tuple(lt,rb);
}
        
void btnode::Tidy(const ImVec2& new_pos, bool vertical)
{
    imnodes::SetNodeGridSpacePos(m_id, new_pos);
    auto dimens = imnodes::GetNodeDimensions(m_id);


    float child_pos_x = new_pos.x + dimens.x + 10;
    float child_pos_y = new_pos.y;
    for(auto [k, child_node_id]: m_link)
    {
        if(child_node_id == 0)
            continue;
        auto child_node_ptr = m_pOwner->QueryNode(child_node_id);
        if(child_node_ptr == nullptr)
            continue;
        child_node_ptr->Tidy( ImVec2(child_pos_x, child_pos_y), vertical );

        const auto& [child_lt, child_rt] = child_node_ptr->GetAABB();
        ImVec2 child_dimens(child_rt.x - child_lt.x, child_rt.y - child_lt.y);
        //竖排
        if(vertical || child_node_ptr->GetChildCount() > 0)
        {
            child_pos_y += child_dimens.y + 10;
        }   
        else
        {
            child_pos_x += child_dimens.x + 10;
        }
        
       
    }
}

void btnode::serialize(nlohmann::json& js_doc) const
{
    js_doc["id"]   = m_id;
    js_doc["type"] = g_bttype_infos[m_type].type;
    js_doc["desc"] = m_desc;

    {
        auto vec_str = split_string(m_script_OnEnter, "\n");
        for(const auto& v: vec_str)
        {
            js_doc["script_onenter"].push_back(v);
        }
    }
    {
        auto vec_str = split_string(m_script_OnLeave, "\n");
        for(const auto& v: vec_str)
        {
            js_doc["script_onleave"].push_back(v);
        }
    }
    {
        auto vec_str = split_string(m_script_OnUpdate, "\n");
        for(const auto& v: vec_str)
        {
            js_doc["script_onupdate"].push_back(v);
        }
    }

    if(std::string(g_bttype_infos[m_type].type) == "ParallelNode")
    {
        js_doc["SuccessPolicy"] = m_SuccessPolicy;
        js_doc["FailurePolicy"] = m_FailurePolicy;
    }

    auto pos = imnodes::GetNodeGridSpacePos(m_id);

    js_doc["pos"]["x"] = pos.x;
    js_doc["pos"]["y"] = pos.y;

    int32_t link_idx = 0;
    for(auto [k, child_node_id]: m_link)
    {
        if(child_node_id == 0)
            continue;
        auto child_node_ptr = m_pOwner->QueryNode(child_node_id);
        if(child_node_ptr == nullptr)
            continue;

        nlohmann::json js_child;
        child_node_ptr->serialize(js_child);
        js_doc["child"].push_back(std::move(js_child));
    }
}

void btnode::deserialize(const nlohmann::json& js_doc)
{
    m_id        = m_pOwner->GenNodeIdx() * 100;
    m_id_input  = m_id + 1;
    m_id_static = m_id + 2;
    m_id_output = m_id + 3;
    m_linkidx = m_id + 4;

    m_max_child_num = -1;
    auto type_str = js_doc["type"];
    for(const auto& [i,v] : ipairs(g_bttype_infos))
    {
        if(std::string(v.type) == type_str)
        {
            m_type = i;
            m_max_child_num = v.max_child_num;
            break;
        }
    }
    m_desc = js_doc["desc"];

    if(js_doc.contains("script_onenter"))
    {
        for(const auto& v: js_doc["script_onenter"])
        {
            m_script_OnEnter += v;
            m_script_OnEnter += "\n";
        }
    }
    if(js_doc.contains("script_onleave"))
    {
        for(const auto& v: js_doc["script_onleave"])
        {
            m_script_OnLeave += v;
            m_script_OnLeave += "\n";
        }
    }
    if(js_doc.contains("script_onupdate"))
    {
        for(const auto& v: js_doc["script_onupdate"])
        {
            m_script_OnUpdate += v;
            m_script_OnUpdate += "\n";
        }
    }

    if(std::string(g_bttype_infos[m_type].type) == "ParallelNode")
    {
        m_SuccessPolicy = js_doc["SuccessPolicy"];
        m_FailurePolicy = js_doc["FailurePolicy"];
    }

    if(js_doc.contains("pos"))
    {
        ImVec2 pos;
        pos.x = js_doc["pos"]["x"];
        pos.y = js_doc["pos"]["y"];
        imnodes::SetNodeGridSpacePos(m_id, pos);
    }

    if(js_doc.contains("child"))
    {
        for(const auto& js_child: js_doc["child"])
        {
            btnode* child = new btnode(m_pOwner, js_child);
            m_pOwner->_AddNode(child);
            auto link_id = m_linkidx++;
            child->SetLinkID(link_id);
            child->SetParentNodeID(GetNodeID());
            m_link.emplace(link_id, child->GetNodeID());
            
            
        }
    }

    m_child_count   = m_link.size();
}