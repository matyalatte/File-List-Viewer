#include "file_tree.h"

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

void FileTree::InitializeTreeCtrl(CustomTreeCtrl* tree_ctrl) {
    if (HasParent()) {
        if (HasChild()) {
            m_image_id = IMAGE_ID_FOLDER;
        } else {
            m_image_id = IMAGE_ID_FILE;
        }
        m_wx_item = tree_ctrl->AppendItem(m_parent->GetId(), m_wx_name, m_image_id);
    } else {
        m_wx_item = tree_ctrl->GetRootItem();
    }

    // Add children to tree ctrl
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        iter->second->InitializeTreeCtrl(tree_ctrl);
    }
}

FileTree::~FileTree() {
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        delete iter->second;
    }
}

void FileTree::DumpPaths(std::ostream& out, CustomTreeCtrl* tree_ctrl,
                         const std::string& parent_path) {
    if (!IsVisible()) { return; }

    std::string current_path;
    if (HasParent() && !m_parent->HasParent()) {
        current_path = m_name;
    } else {
        current_path = parent_path + "/" + m_name;
    }

    if (!HasChild()) {
        if (!tree_ctrl || tree_ctrl->IsChecked(m_wx_item)) {
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

void FileTree::Filter(const std::string& filter, CustomTreeCtrl* tree_ctrl) {
    if (!HasChild()) {
        if (m_name.find(filter) == std::string::npos) {
            RemoveFromCtrl(tree_ctrl);
        }
        else {
            AddToCtrl(tree_ctrl);
        }
        return;
    }

    // if HasChild()
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        iter->second->Filter(filter, tree_ctrl);
    }
}

void FileTree::RemoveFromCtrl(CustomTreeCtrl* tree_ctrl) {
    if (!IsVisible() || !HasParent()) return;
    m_visible = false;
    tree_ctrl->Delete(m_wx_item);
    if (tree_ctrl->ItemHasChildren(m_parent->GetId())) return;
    m_parent->RemoveFromCtrl(tree_ctrl);
}

void FileTree::AddToCtrl(CustomTreeCtrl* tree_ctrl) {
    if (IsVisible()) return;
    if (HasParent()) m_parent->AddToCtrl(tree_ctrl);
    m_visible = true;
    m_wx_item = tree_ctrl->AppendItem(m_parent->GetId(), m_wx_name, m_image_id);
}

void FileTree::MakeDir(const wxString& o_dir) {
    if (!HasChild()) return;
    wxString new_o_dir = o_dir + wxFILE_SEP_PATH + m_wx_name;
    if (!wxDirExists(new_o_dir) && !wxMkdir(new_o_dir)) {
        wxString msg = "Error: Failed to make " + new_o_dir;
        throw std::runtime_error(msg);
    }
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter) {
        iter->second->MakeDir(new_o_dir);
    }
}
