#pragma once
#include <wx/treectrl.h>
#include <wx/menu.h>
#include "image_gen.h"

class CustomTreeCtrl : public wxTreeCtrl {
 private:
    wxTreeItemId m_event_item;
    bool m_is_saved;
    void OnActivated(wxTreeEvent& event);
    void OnItemMenu(wxTreeEvent& event);
    void SelectAll();
    void CheckStatus(const wxTreeItemId& id);
    void CheckChildren(const wxTreeItemId& id);
    void UncheckChildren(const wxTreeItemId& id);
    wxString GetCheckMenuText(const wxTreeItemId& id);

 public:
    CustomTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);
    bool IsChecked(const wxTreeItemId& id);
    bool IsSaved() { return m_is_saved; }
    void SetSaveStatus(bool is_saved) { m_is_saved = is_saved; }
};
