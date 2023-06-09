#include "custom_tree_ctrl.h"

CustomTreeCtrl::CustomTreeCtrl(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size, long style)
                               : wxTreeCtrl(parent, id, pos, size, style) {
    m_is_saved = true;
    Bind(wxEVT_TREE_ITEM_ACTIVATED, &CustomTreeCtrl::OnActivated, this);
    Bind(wxEVT_TREE_ITEM_MENU, &CustomTreeCtrl::OnItemMenu, this);
}

// check or unckeck when a file item is double-clicked
void CustomTreeCtrl::OnActivated(wxTreeEvent& event) {
    wxTreeItemId id = event.GetItem();
    if (id && GetChildrenCount(id, false) == 0) {
        int new_id;
        if (IsChecked(id)) {
            new_id = IMAGE_ID_FILE;
        } else {
            new_id = IMAGE_ID_CHECK;
        }
        SetItemImage(id, new_id);
        UpdateParentStatus(GetItemParent(id));
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
        UncheckAll(m_event_item);
    } else {
        CheckAll(m_event_item);
    }
    m_is_saved = false;
    UpdateParentStatus(GetItemParent(m_event_item));
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
    return GetItemImage(id) == IMAGE_ID_CHECK;
}

// Check all child nodes
void CustomTreeCtrl::CheckAll(const wxTreeItemId& id) {
    if (IsChecked(id)) return;
    SetItemImage(id, IMAGE_ID_CHECK);
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(id, cookie);
    while (item.IsOk()) {
        CheckAll(item);
        item = GetNextChild(id, cookie);
    }
}

// Uncheck all child nodes
void CustomTreeCtrl::UncheckAll(const wxTreeItemId& id) {
    if (!IsChecked(id)) return;
    int new_image_id;
    if (GetChildrenCount(id, false) > 0) {
        new_image_id = IMAGE_ID_FOLDER;
    } else {
        new_image_id = IMAGE_ID_FILE;
    }
    SetItemImage(id, new_image_id);
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(id, cookie);
    while (item.IsOk()) {
        UncheckAll(item);
        item = GetNextChild(id, cookie);
    }
}

void CustomTreeCtrl::UpdateParentStatus(const wxTreeItemId& root) {
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
        SetItemImage(root, IMAGE_ID_CHECK);
        UpdateParentStatus(GetItemParent(root));  // check parent as well
    }
    if (!all_checked && IsChecked(root)) {
        SetItemImage(root, IMAGE_ID_FOLDER);
        UpdateParentStatus(GetItemParent(root));  // check parent as well
    }
}

void CustomTreeCtrl::UpdateAllStatus(const wxTreeItemId& root, bool is_root) {
    if (!root.IsOk() || !ItemHasChildren(root)) return;

    // check if all children are checked
    bool all_checked = true;
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(root, cookie);
    while (item.IsOk()) {
        UpdateAllStatus(item, false);
        if (!IsChecked(item) && !is_root) {
            all_checked = false;
            break;
        }
        item = GetNextChild(root, cookie);
    }

    // update status
    if (all_checked) {
        SetItemImage(root, IMAGE_ID_CHECK);
    } else {
        SetItemImage(root, IMAGE_ID_FOLDER);
    }
}

void CustomTreeCtrl::SortAll(const wxTreeItemId& root) {
    if (!root.IsOk() || !ItemHasChildren(root)) return;
    SortChildren(root);
    wxTreeItemIdValue cookie;
    wxTreeItemId item;
    item = GetFirstChild(root, cookie);
    while (item.IsOk()) {
        SortAll(item);
        item = GetNextChild(root, cookie);
    }
}
