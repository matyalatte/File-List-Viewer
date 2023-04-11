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
    wxString m_name;  // node name

    // child nodes. keys are child node names
    std::map<std::string, FileTree*> m_items;

    FileTree* m_parent;  // parent node
    size_t m_depth;
    wxTreeItemId m_wx_item;  // id for tree ctrl

 public:
    FileTree() {  // for root
        m_name = "";
        m_parent = nullptr;
        m_depth = 0;
        m_wx_item = nullptr;
    }

    FileTree(FileTree* parent, const std::string& name) {
        m_name = wxString::FromUTF8(name);
        m_parent = parent;
        m_depth = parent->m_depth + 1;
        m_wx_item = nullptr;
    }

    ~FileTree();
    void AddItem(char* path);
    void AddToTreeCtrl(CustomTreeCtrl* tree_ctrl);
    void DumpPaths(std::ostream& out, CustomTreeCtrl* tree_ctrl,
                   const std::string& parent_path = "");
    bool HasParent() { return m_parent; }
    bool HasChild() { return m_items.size() > 0; }
    friend std::ostream& operator<<(std::ostream&, const FileTree&);
};
