#pragma once
#include <memory>
#include <wx/wx.h>
#include <wx/colour.h>
#include <wx/stopwatch.h>
#include <wx/dnd.h>
#include "file_tree.h"
#include "image_gen.h"

class FileDropTarget;

// Main window
class MainFrame : public wxFrame {
 private:
    wxPanel* m_panel;
    wxImageList* m_image_list;
    FileDropTarget* m_drop_target;
    CustomTreeCtrl* m_tree_ctrl;
    std::unique_ptr<FileTree> m_file_tree;
    wxTextCtrl* m_filter_ctrl;
    wxString m_old_filter;

    void CreateFrame();
    bool ReadFileList(const char* file);
    void SaveFileList();
    void OnClose(wxCloseEvent& event);
    void OnOpenURL(wxCommandEvent& event);
    void ShowErrorDialog(const wxString& msg);
    void ShowSuccessDialog(const wxString& msg, const wxString& title = "Success");
    bool HasEmptyList() { return !m_file_tree || !(m_file_tree->HasChild()); }
    bool IsSaved() { return m_tree_ctrl->IsSaved(); }
    void OnFilter(wxCommandEvent& event);

 public:
    MainFrame(const wxString& title = wxEmptyString);
    void OpenFileList(wxString filename = "");
};

// Set this target to m_tree_ctrl.
class FileDropTarget : public wxFileDropTarget {
 private:
    MainFrame* m_frame;

 public:
    explicit FileDropTarget(MainFrame* frame) : wxFileDropTarget() {
        m_frame = frame;
    }

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
        // Run MainFrame::OpenFileList when a file is dropped on m_tree_ctrl
        m_frame->OpenFileList(filenames[0]);
        return 1;
    }
};

