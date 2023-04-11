#include "custom_tree_ctrl.h"

CustomTreeCtrl::CustomTreeCtrl(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size, long style)
                               : wxTreeCtrl(parent, id, pos, size, style) {
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &CustomTreeCtrl::OnActivated, this);
    Bind(wxEVT_TREE_ITEM_MENU, &CustomTreeCtrl::OnItemMenu, this);
}

// check or unckeck when a file item is double-clicked
void CustomTreeCtrl::OnActivated(wxTreeEvent& event) {
    wxTreeItemId id = event.GetItem();
    if (id && GetChildrenCount(id, false) == 0) {
        int new_id;
        if (IsChecked(id)) {
            new_id = static_cast<int>(ImageID::FILE);
        } else {
            new_id = static_cast<int>(ImageID::CHECK);
        }
        SetItemImage(id, new_id);
        CheckStatus(GetItemParent(id));
        m_is_saved = false;
    }
}

// Show popup menu when right-clicked
void CustomTreeCtrl::OnItemMenu(wxTreeEvent& event) {
    m_event_item = event.GetItem();
    wxMenu* menu = new wxMenu;
    menu->Append(wxID_SELECTALL, GetCheckMenuText(m_event_item));
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { SelectAll(); }, wxID_SELECTALL);
    bool res = PopupMenu(menu, event.GetPoint());
}

// Check or uncheck all child nodes
void CustomTreeCtrl::SelectAll() {
    if (IsChecked(m_event_item)) {
        UncheckChildren(m_event_item);
    } else {
        CheckChildren(m_event_item);
    }
    m_is_saved = false;
    CheckStatus(GetItemParent(m_event_item));
}

wxString CustomTreeCtrl::GetCheckMenuText(const wxTreeItemId& id) {
    wxString text = "";
    if (IsChecked(id)) {
        text = "Uncheck";
    } else {
        text = "Check";
    }
    if (GetChildrenCount(id, false) > 0) {
        text += " All";
    }
    return text;
}

bool CustomTreeCtrl::IsChecked(const wxTreeItemId& id) {
    return GetItemImage(id) == static_cast<int>(ImageID::CHECK);
}

// Check all child nodes
void CustomTreeCtrl::CheckChildren(const wxTreeItemId& id) {
    if (IsChecked(id)) return;
    SetItemImage(id, static_cast<int>(ImageID::CHECK));
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(id, cookie);
    while (item.IsOk()) {
        CheckChildren(item);
        item = GetNextChild(id, cookie);
    }
}

// Uncheck all child nodes
void CustomTreeCtrl::UncheckChildren(const wxTreeItemId& id) {
    if (!IsChecked(id)) return;
    int new_image_id;
    if (GetChildrenCount(id, false) > 0) {
        new_image_id = static_cast<int>(ImageID::FOLDER);
    } else {
        new_image_id = static_cast<int>(ImageID::FILE);
    }
    SetItemImage(id, new_image_id);
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(id, cookie);
    while (item.IsOk()) {
        UncheckChildren(item);
        item = GetNextChild(id, cookie);
    }
}

void CustomTreeCtrl::CheckStatus(const wxTreeItemId& root) {
    if (!root.IsOk()) return;

    // check if all children are checked
    bool all_checked = true;
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(root, cookie);
    while (item.IsOk()) {
        if (!IsChecked(item)) {
            all_checked = false;
            break;
        }
        item = GetNextChild(root, cookie);
    }

    // update status
    if (all_checked && !IsChecked(root)) {
        SetItemImage(root, static_cast<int>(ImageID::CHECK));
        CheckStatus(GetItemParent(root));  // check parent as well
    }
    if (!all_checked && IsChecked(root)) {
        SetItemImage(root, static_cast<int>(ImageID::FOLDER));
        CheckStatus(GetItemParent(root));  // check parent as well
    }
}
