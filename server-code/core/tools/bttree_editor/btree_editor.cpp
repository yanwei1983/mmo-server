#include "btree_editor.h"

#include <fstream>
#include <iomanip>
#include "LoopHelper.h"
#include "IMGuiFileDialog.h"
#include "btnode.h"
#include "fmt/format.h"
#include "imgui/imgui.h"
#include "imnode/imnodes.h"
#include "json.hpp"

BTTreeEditor::BTTreeEditor(int32_t editor_id, const std::string& name)
    : m_editor_id(editor_id)
    , m_editor_name(name)
{
    m_editor_context = imnodes::EditorContextCreate();
}

BTTreeEditor::~BTTreeEditor()
{
    imnodes::EditorContextFree(m_editor_context);
}

void BTTreeEditor::OnInit() {}
void BTTreeEditor::OnShutDown()
{
    DestoryAllNode();
}

void BTTreeEditor::RemoveNode(int32_t node_id)
{
    m_RemoveNodeList.push_back(node_id);
}

void BTTreeEditor::OnShow()
{
    imnodes::EditorContextSet(m_editor_context);
    ImFont* pChineseFont = ImGui::GetIO().Fonts->Fonts[1];
    ImGui::PushFont(pChineseFont);
    bool bOpen = true;
    if(ImGui::Begin(m_editor_name.c_str(), &bOpen) == false)
    {
        ImGui::PopFont();
        return ImGui::End();
    }
    ImGui::PopFont();
    auto& io                 = ImGui::GetIO();
    bool  bNeedTidy          = false;
    bool  bNeedTidyVertical  = false;
    bool  double_check_close = false;
    bool  bNeedDelFloatNode  = false;
    bool bResize = false;
    if(ImGui::BeginPopupContextItem())
    {
        if(ImGui::MenuItem("Close", "CTRL+W"))
        {
            bOpen = false;
        }
        if(ImGui::MenuItem("Save", "CTRL+S"))
        {
            if(m_filename.empty() == true)
                igfd::ImGuiFileDialog::Instance()->OpenModal("SaveDialog", "Save File", ".json", "res/bttree");
            else
                SaveTo(m_filename);
        }
        if(ImGui::MenuItem("Resize", "CTRL+R"))
        {
            bResize = true;
        }
        if(ImGui::MenuItem("TidyVertical", "CTRL+T"))
        {
            bNeedTidyVertical = true;
        }
        if(ImGui::MenuItem("Tidy", "CTRL+H"))
        {
            bNeedTidy = true;
        }
        if(ImGui::MenuItem("DeleteFlotNode", "CTRL+D"))
        {
            bNeedDelFloatNode = true;
        }

        ImGui::EndPopup();
    }
    bool pop_close = false;
    bool pop_close_and_save = false;
    if(bOpen == false)
    {
        if(m_saved == false)
        {
            pop_close_and_save = true;
        }
        else
        {
            pop_close = true;
        }
    }

    ImGui::PushFont(pChineseFont);
    if(igfd::ImGuiFileDialog::Instance()->FileDialog("SaveDialog"))
    {
        // action if OK
        if(igfd::ImGuiFileDialog::Instance()->IsOk == true)
        {
            std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath     = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
            auto        base_name    = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
            m_filename               = filePathName;
            m_editor_name = base_name;
            SaveTo(m_filename);
            // action
        }
        // close
        igfd::ImGuiFileDialog::Instance()->CloseDialog("SaveDialog");
    }
    ImGui::PopFont();

    

    m_num_selected_nodes = imnodes::NumSelectedNodes();
    m_num_selected_links = imnodes::NumSelectedLinks();
    uint32_t float_node_count = 0;
    for(const auto& [k, node]: m_allnode)
    {
        if(node->GetParentNodeID() == 0 && node->GetNodeID() != m_rootnode_id)
        {
            float_node_count++;
        }
    }


    if(bResize)
        ImGui::SetWindowSize({100, 100}, ImGuiCond_Always);





    imnodes::BeginNodeEditor();

    if(pop_close == true)
    {
        ImGui::OpenPopup("Close?");
    }
    if(pop_close_and_save == true)
    {
        ImGui::OpenPopup("SaveAndClose?");
    }
    ImGui::Text("select_nodes:%d", m_num_selected_nodes);
    ImGui::Text("select_links:%d", m_num_selected_links);
    ImGui::Text("float_node:%u", float_node_count);
    ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");


    std::string create_node_menu_name = attempt_format("{}_create_node", m_editor_name);

    const bool open_popup =
        ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && imnodes::IsEditorHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
    int hovered_node_id = 0;
    int hovered_link_id = 0;
    if(open_popup)
    {

        if(m_num_selected_nodes > 0)
        {
            ImGui::OpenPopup("node_popup");
        }
        else if(m_num_selected_links > 0)
        {
            ImGui::OpenPopup("link_popup");
        }
        else if(ImGui::IsAnyItemHovered() == false)
        {
            ImGui::OpenPopup("create_node");
        }
    }

    if(ImGui::BeginPopup("node_popup"))
    {
        std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(m_num_selected_nodes));
        imnodes::GetSelectedNodes(selected_nodes.data());
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        if(ImGui::MenuItem("DestoryNode"))
        {
            for(auto node_id: selected_nodes)
            {
                DestoryNode(node_id);
            }
            imnodes::ClearNodeSelection();
        }
        // if(ImGui::MenuItem("TidyVertical"))
        // {
        //     bNeedTidyVertical = true;
        // }
        if(ImGui::MenuItem("Tidy"))
        {
            bNeedTidy = true;
        }
        if(ImGui::MenuItem("SelectChild"))
        {
            for(auto node_id: selected_nodes)
            {
                SelectChildNode(node_id);
            }
        }
        if(ImGui::MenuItem("Clone"))
        {
            ImVec2 first_node_pos;
            if(m_num_selected_nodes >= 1)
            {
                first_node_pos = imnodes::GetNodeScreenSpacePos(selected_nodes[0]);
            }
            
            for(auto node_id: selected_nodes)
            {               
                CloneNode(node_id, click_pos, first_node_pos, false);
            }           
        
        }
        if(ImGui::MenuItem("CloneChild"))
        {
            ImVec2 first_node_pos;
            if(m_num_selected_nodes >= 1)
            {
                first_node_pos = imnodes::GetNodeScreenSpacePos(selected_nodes[0]);
            }
            
            //排除所有子节点
            std::vector<int32_t> op_nodes;
            for(auto node_id: selected_nodes)
            {
                //如果选中nodes中有自己的父节点，则自己不放入
                auto node = QueryNode(node_id);
                if(node == nullptr)
                    continue;
                if(std::find(selected_nodes.begin(),selected_nodes.end(), node->GetParentNodeID()) == selected_nodes.end())
                    op_nodes.push_back(node_id);
            }     
            for(auto node_id: op_nodes)
            {
                CloneNode(node_id, click_pos, first_node_pos, true);      
            }
        
        }
        if(m_num_selected_nodes == 1 && ImGui::MenuItem("UpperLink"))
        {            
            for(auto node_id: selected_nodes)
            {               
                UpperLink(node_id);
            }           
        }
        if(m_num_selected_nodes == 1 && ImGui::MenuItem("DownLink"))
        {           
            for(auto node_id: selected_nodes)
            {               
                DownLink(node_id);
            }           
        }
        if(m_num_selected_nodes == 1 && ImGui::MenuItem("serializeNodeToClipBoard"))
        {            
            serializeNodeToClipBoard();     
        }
        if(ImGui::MenuItem("deserializeNodeFromClipBoard"))
        {            
            deserializeNodeFromClipBoard();        
        }
        ImGui::EndPopup();
    }

    if(ImGui::BeginPopup("link_popup"))
    {
        if(ImGui::MenuItem("RemoveLink"))
        {
            std::vector<int> selected_links;
            selected_links.resize(static_cast<size_t>(m_num_selected_links));
            imnodes::GetSelectedLinks(selected_links.data());
            for(auto link_id: selected_links)
            {
                RemoveLink(link_id);
            }
            imnodes::ClearLinkSelection();
        }
        ImGui::EndPopup();
    }

    if(ImGui::BeginPopup("create_node"))
    {
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        ImGui::PushFont(pChineseFont);
        ImGui::Text(m_editor_name.c_str());
        ImGui::PopFont();
        ImGui::Separator();

        if(m_rootnode_id == 0)
        {
            if(ImGui::MenuItem("BTTree"))
            {
                btnode* pnode = new btnode(this, 0, -1);
                _AddNode(pnode);
                m_rootnode_id = pnode->GetNodeID();
            }
        }
        else
        {
            for(const auto& [i,info]: ipairs(g_bttype_infos))
            {
                if(i==0)
                    continue;
                if(ImGui::MenuItem(std::string(info.type).c_str()))
                {
                    btnode* pnode = new btnode(this, i, info.max_child_num);
                    _AddNode(pnode);
                    imnodes::SetNodeScreenSpacePos(pnode->GetNodeID(), click_pos);
                }
            }

            if(ImGui::MenuItem("deserializeNodeFromClipBoard"))
            {            
                deserializeNodeFromClipBoard();        
            }
        }

        ImGui::EndPopup();
    }

    if(ImGui::BeginPopupModal("Close?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if(ImGui::Button("OK", ImVec2(120, 0)))
        {
            m_wantclose = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            bOpen = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if(ImGui::BeginPopupModal("SaveAndClose?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if(ImGui::Button("Save", ImVec2(120, 0)))
        {
            if(m_filename.empty())
            {
                igfd::ImGuiFileDialog::Instance()->OpenModal("SaveDialog", "Save File", ".json", "res/bttree");
            }
            else
            {
                SaveTo(m_filename);
                m_wantclose = true;
            }
            
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Don't Save", ImVec2(120, 0)))
        {
            m_wantclose = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            bOpen = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();

    for(const auto& [k, node]: m_allnode)
    {
        if(node->GetParentNodeID() == 0)
            node->OnShow();
    }
    for(const auto& [k, node]: m_allnode)
    {
        if(node->GetParentNodeID() == 0)
            node->DrawLink();
    }

    imnodes::EndNodeEditor();

    {
        int start_attr = 0;
        int end_attr   = 0;
        if(imnodes::IsLinkCreated(&start_attr, &end_attr))
        {
            AddLink(start_attr, end_attr);
        }
    }

    {
        int link_id;
        if(imnodes::IsLinkDestroyed(&link_id))
        {
            RemoveLink(link_id);
        }
    }
    if(bNeedTidyVertical)
    {
        Tidy(true);
    }
    if(bNeedTidy)
    {
        Tidy(false);
    }
    if(bNeedDelFloatNode)
    {
        for(const auto& [k, node]: m_allnode)
        {
            if(node->GetParentNodeID() == 0 && node->GetNodeID() != m_rootnode_id)
                RemoveNode(k);
        }
    }
    ImGui::End();

    for(auto node_id: m_RemoveNodeList)
    {
        DestoryNode(node_id);
    }
    m_RemoveNodeList.clear();
}

int32_t BTTreeEditor::GenNodeIdx()
{
    m_node_idx++;
    int32_t node_idx = m_editor_id * MAX_NODE_COUNT + m_node_idx;
    return node_idx;
}

btnode* BTTreeEditor::CloneNode(int32_t node_id, const ImVec2& new_pos, const ImVec2& base_node_pos,  bool clone_child)
{
    auto node = QueryNode(node_id);
    auto node_pos = imnodes::GetNodeScreenSpacePos(node_id);
    auto clone_node = node->clone();
    _AddNode(clone_node);
    ImVec2 offset_pos(node_pos.x - base_node_pos.x, node_pos.y - base_node_pos.y);
    ImVec2 clone_pos(new_pos.x + offset_pos.x, new_pos.y + offset_pos.y);
    imnodes::SetNodeScreenSpacePos(clone_node->GetNodeID(), clone_pos);
    
    if(clone_child)
    {
        node->foreach([this,clone_parent_node=clone_node,&node_pos,&clone_pos](auto child_node_id)
        {
            auto clone_node = CloneNode(child_node_id, clone_pos, node_pos, true);
            clone_parent_node->AddLink(clone_node->GetNodeID());
        });
    }

    return clone_node;
}

void BTTreeEditor::SelectChildNode(int32_t node_id)
{
    auto node = QueryNode(node_id);
    if(node == nullptr)
        return;
    node->foreach([this](auto child_node_id)
    {
        SelectChildNode(child_node_id);
    });

    imnodes::SelectNode(node_id);
}

btnode* BTTreeEditor::QueryNode(int32_t node_id) const
{
    auto it = m_allnode.find(node_id);
    if(it == m_allnode.end())
        return nullptr;
    return it->second.get();
}

void BTTreeEditor::AddLink(int32_t start_attr, int32_t end_attr)
{
    // input->output,//首尾倒置
    if(start_attr % 100 == 1 && end_attr % 100 == 3)
    {
        std::swap(start_attr, end_attr);
    }

    if(start_attr % 100 != 3 || end_attr % 100 != 1)
    {
        //只能output->input
        return;
    }

    int32_t node_id = (start_attr / 100) * 100;
    int32_t child_node_id  = (end_attr / 100) * 100;
    auto node = QueryNode(node_id);
    if(node == nullptr)
        return;
    
    node->AddLink(child_node_id);
    m_saved = false;
}

void BTTreeEditor::RemoveLink(int32_t link_id)
{
    int32_t node_id = (link_id / 100) * 100;
    auto    node    = QueryNode(node_id);
    if(node == nullptr)
        return;
    node->RemoveLink(link_id);
    m_saved = false;
}

void BTTreeEditor::DownLink(int32_t node_id)
{
    auto node = QueryNode(node_id);
    if(node == nullptr)
        return;
    auto parent_node = QueryNode(node->GetParentNodeID());
    if(parent_node == nullptr)
        return;
    parent_node->DownLink(node_id);
}

void BTTreeEditor::UpperLink(int32_t node_id)
{
    auto node = QueryNode(node_id);
    if(node == nullptr)
        return;
    auto parent_node = QueryNode(node->GetParentNodeID());
    if(parent_node == nullptr)
        return;
    parent_node->UpperLink(node_id);
}

void BTTreeEditor::DestoryAllNode()
{
    imnodes::EditorContextSet(m_editor_context);
    std::vector<int32_t> nodes;
    for(const auto& [k, v]: m_allnode)
    {
        nodes.push_back(k);
    }
    for(const auto& node_id: nodes)
    {
        DestoryNode(node_id);
    }
}

void BTTreeEditor::DestoryNode(int32_t node_id)
{
    auto it = m_allnode.find(node_id);
    if(it == m_allnode.end())
        return;
    if(m_rootnode_id == node_id)
        return;
    auto& node = it->second;
    node->RemoveAllLink();

    if(node->GetParentNodeID() != 0)
    {
        auto pParent = QueryNode(node->GetParentNodeID());
        if(pParent)
        {
            pParent->RemoveChildNode(node_id);
        }
        node->SetParentNodeID(0);
    }

    m_allnode.erase(it);
    m_saved = false;
}

void BTTreeEditor::_AddNode(btnode* pnode)
{
    m_allnode.emplace(pnode->GetNodeID(), pnode);
    m_saved = false;
}

void BTTreeEditor::LoadFrom(const std::string& filename)
{
    std::ifstream infile(filename);
    auto          js_doc = nlohmann::json::parse(infile, nullptr, false, true);

    if(js_doc.is_discarded() == true)
        return;
    m_filename = filename;
    imnodes::EditorContextSet(m_editor_context);

    btnode* pnode = new btnode(this, js_doc["root_node"]);
    _AddNode(pnode);
    m_rootnode_id = pnode->GetNodeID();

    for(const auto& float_child: js_doc["float_child"])
    {
        btnode* pnode = new btnode(this, float_child);
        _AddNode(pnode);
    }

    m_saved = true;
}

void BTTreeEditor::SaveTo(const std::string& filename)
{
    if(m_rootnode_id == 0)
        return;
    imnodes::EditorContextSet(m_editor_context);
    nlohmann::json js_doc;

    js_doc["m_rootnode_id"] = m_rootnode_id;

    m_allnode[m_rootnode_id]->serialize(js_doc["root_node"]);

    for(const auto& [k, node]: m_allnode)
    {
        if(node->GetParentNodeID() == 0 && node->GetNodeID() != m_rootnode_id)
        {
            nlohmann::json js_float_child;
            node->serialize(js_float_child);
            js_doc["float_child"].push_back(js_float_child);
        }
    }

    std::ofstream ofs(filename.data(), std::ios::out | std::ios::trunc);
    if(ofs.is_open())
    {

        ofs << std::setw(4) << js_doc;
        ofs.close();
        m_saved = true;
    }
}

void BTTreeEditor::serializeNodeToClipBoard()
{
    if(imnodes::NumSelectedNodes() != 1)
        return;
       

    std::vector<int> selected_nodes;
    selected_nodes.resize(static_cast<size_t>(imnodes::NumSelectedNodes()));
    imnodes::GetSelectedNodes(selected_nodes.data());
    for(auto node_id: selected_nodes)
    {
        auto node = QueryNode(node_id);
        if(node)
        {
            nlohmann::json js_doc;
            node->serialize(js_doc);
            std::string txt = js_doc.dump(4);
            ImGui::SetClipboardText(txt.c_str());
            return;
        }
    }

}

void BTTreeEditor::deserializeNodeFromClipBoard()
{
    auto txt = ImGui::GetClipboardText();
    if(txt == nullptr)
        return;
    nlohmann::json js_doc = nlohmann::json::parse(txt, nullptr, false, true);

    if(js_doc.is_discarded() == true)
        return;

    btnode* pnode = new btnode(this, js_doc);
    _AddNode(pnode);
}



void BTTreeEditor::Tidy(bool vertical)
{
    if(imnodes::NumSelectedNodes() > 0)
    {
        std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(imnodes::NumSelectedNodes()));
        imnodes::GetSelectedNodes(selected_nodes.data());
        for(auto node_id: selected_nodes)
        {
            auto node = QueryNode(node_id);
            if(node)
            {
                auto pos = imnodes::GetNodeGridSpacePos(node_id);
                node->Tidy(pos,vertical);
            }
        }
        return;
    }
    else
    {
        if(m_rootnode_id == 0)
            return;
        auto node = m_allnode[m_rootnode_id].get();
        auto pos  = imnodes::GetNodeGridSpacePos(m_rootnode_id);
        node->Tidy(pos,vertical);
    }

}