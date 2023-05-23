#pragma once
#include <wx/treectrl.h>
#include <wx/menu.h>
#include <wx/generic/treectlg.h>
#include "image_gen.h"


class CustomTreeItemData : public wxTreeItemData {
 private:
    bool m_visible;
 public:
    CustomTreeItemData() : m_visible(true) {}
    bool IsVisible() { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
};

class CustomTreeCtrl : public wxTreeCtrl {
 private:
    wxTreeItemId m_event_item;
    bool m_is_saved;
    void OnActivated(wxTreeEvent& event);
    void OnItemMenu(wxTreeEvent& event);
    void UncheckAll(const wxTreeItemId& id);
    void CheckAll(const wxTreeItemId& id);
    void UpdateParentStatus(const wxTreeItemId& id);
    wxString GetCheckMenuText(const wxTreeItemId& id);

 public:
    CustomTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);
    bool IsChecked(const wxTreeItemId& id);
    bool IsSaved() { return m_is_saved; }
    void SetSaveStatus(bool is_saved) { m_is_saved = is_saved; }
    void UpdateAllStatus(const wxTreeItemId& id, bool is_root = true);
    void SortAll(const wxTreeItemId& id);
    void SelectAll();
    void SetEventItem(const wxTreeItemId& id) { m_event_item = id; }
};
