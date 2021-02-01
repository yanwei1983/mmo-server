#ifndef BTREE_EDITOR_H
#define BTREE_EDITOR_H

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
struct ImVec2;

class btnode;
namespace imnodes
{
    struct EditorContext;
}

static const int32_t MAX_NODE_COUNT = 1000;
static const int32_t MAX_EDITOR_COUNT = 100;
class BTTreeEditor
{
public:
    BTTreeEditor(int32_t editor_id, const std::string& name);
    ~BTTreeEditor();
public:
    void OnInit();
    void OnShutDown();
    void OnShow();

    int32_t GenNodeIdx();

    btnode* QueryNode(int32_t node_id) const;
    void RemoveNode(int32_t node_id);

    bool WantClose()const {return m_wantclose;}
    void _AddNode(btnode* pnode);
    void LoadFrom(const std::string& filename);
    void SaveTo(const std::string& filename);
private:
    void DestoryNode(int32_t node_id);
    void AddLink(int32_t start_attr, int32_t end_attr);
    void RemoveLink(int32_t link_id);
    void UpperLink(int32_t node_id);
    void DownLink(int32_t node_id);
    void Tidy(bool vertical);
    void DestoryAllNode();
    void SelectChildNode(int32_t node_id);
    btnode* CloneNode(int32_t node_id, const ImVec2& new_pos, const ImVec2& base_node_pos,  bool clone_child);

    void serializeNodeToClipBoard();
    void deserializeNodeFromClipBoard();
private:
    int32_t m_editor_id;
    std::string m_editor_name;
    std::unordered_map<int32_t, std::unique_ptr<btnode> >  m_allnode;
    
    imnodes::EditorContext* m_editor_context = nullptr;
    int32_t m_node_idx = 0;

    std::vector<int32_t> m_RemoveNodeList;
    std::vector<int32_t> m_RemoveLinkList;

    int32_t m_rootnode_id = 0;
    bool m_open = true;
    bool m_wantclose = false;
    bool m_saved = true;
    int32_t m_num_selected_links = 0;
    int32_t m_num_selected_nodes = 0;

    

    std::string m_filename;
};

#endif /* BTREE_EDITOR_H */
