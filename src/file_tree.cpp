#include "file_tree.h"

std::string operator * (std::string a, unsigned int b) {
    std::string output = "";
    while (b--) {
        output += a;
    }
    return output;
}

char* GetChildPath(char* path) {
    // split char* by "/"" or "\"
    // "path" will be root dir name, "chr" will be the rest.
    char* chr;
    for (chr = path; *chr; ++chr) {
        if (chr[0] == 0x2F || chr[0] == 0x5C) {
            chr[0] = 0;
            chr += 1;
            break;
        }
    }
    return chr;
}

void FileTree::AddItem(char* path) {
    char* item_path = GetChildPath(path);
    std::string item_name = std::string(path);
    if (item_name == "" && item_path[0] == 0) {
        return;
    }
    if (m_items.count(item_name) == 0) {
        FileTree* new_item = new FileTree(this, item_name);
        new_item->AddItem(item_path);
        m_items[item_name] = new_item;
    } else {
        m_items[item_name]->AddItem(item_path);
    }
}

void FileTree::AddToTreeCtrl(CustomTreeCtrl* tree_ctrl) {
    wxTreeItemId parent_item;
    if (HasParent()) {
        parent_item = m_parent->m_wx_item;
        int image_id;
        if (HasChild()) {
            image_id = static_cast<int>(ImageID::FOLDER);
        } else {
            image_id = static_cast<int>(ImageID::FILE);
        }
        m_wx_item = tree_ctrl->AppendItem(parent_item, m_name, image_id);
    } else {
        m_wx_item = tree_ctrl->GetRootItem();
    }

    // Add children to tree ctrl
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        iter->second->AddToTreeCtrl(tree_ctrl);
    }
}

FileTree::~FileTree() {
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        delete iter->second;
    }
}

void FileTree::DumpPaths(std::ostream& out, CustomTreeCtrl* tree_ctrl,
                         const std::string& parent_path) {
    std::string current_path;
    if (HasParent() && !m_parent->HasParent()) {
        current_path = m_name;
    } else {
        current_path = parent_path + "/" + m_name;
    }

    if (!HasChild()) {
        if (tree_ctrl->IsChecked(m_wx_item)) {
            // Out put a file path
            out << current_path << "\n";
        }
        return;
    }

    // Move to deeper nodes
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        iter->second->DumpPaths(out, tree_ctrl, current_path);
    }
}

std::ostream& operator<<(std::ostream& out, const FileTree& file_tree) {
    std::string indent = " ";
    indent = indent * file_tree.m_depth * 2;
    for (auto iter = file_tree.m_items.begin(); iter != file_tree.m_items.end(); ++iter) {
        out << indent << iter->first << "\n";
        out << *(iter->second);
    }
    return out;
}
