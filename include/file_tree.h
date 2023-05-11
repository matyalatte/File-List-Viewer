#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include "image_gen.h"
#include "custom_tree_ctrl.h"

class FileTree {
 private:
    std::string m_name;  // node name
    wxString m_wx_name;  // node name

    // child nodes. keys are child node names
    std::map<std::string, FileTree*> m_items;

    FileTree* m_parent;  // parent node
    size_t m_depth;
    wxTreeItemId m_wx_item;  // id for tree ctrl
    bool m_visible;
    int m_image_id;

 public:
    FileTree() {  // for root
        m_name = "";
        m_wx_name = "";
        m_parent = nullptr;
        m_depth = 0;
        m_wx_item = nullptr;
        m_visible = true;
    }

    FileTree(FileTree* parent, const std::string& name) {
        m_name = name;
        m_wx_name = wxString::FromUTF8(name);
        m_parent = parent;
        m_depth = parent->m_depth + 1;
        m_wx_item = nullptr;
        m_visible = true;
    }

    ~FileTree();
    void AddItem(char* path);
    void InitializeTreeCtrl(CustomTreeCtrl* tree_ctrl);
    void DumpPaths(std::ostream& out, CustomTreeCtrl* tree_ctrl,
                   const std::string& parent_path = "");
    void Filter(const std::string& filter, CustomTreeCtrl* tree_ctrl);
    void MakeDir(const wxString& o_dir);
    void RemoveFromCtrl(CustomTreeCtrl* tree_ctrl);
    void AddToCtrl(CustomTreeCtrl* tree_ctrl);
    wxTreeItemId GetId() { return m_wx_item; }
    bool HasParent() { return m_parent; }
    bool HasChild() { return m_items.size() > 0; }
    bool IsVisible() { return m_visible; }
};
