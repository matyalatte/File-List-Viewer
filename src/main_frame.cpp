#include "main_frame.h"

const char* TOOL_NAME = "File List Viewer";
const char* VERSION = "0.0.1";

// Main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, TOOL_NAME) {
    std::cout << TOOL_NAME << " v" << VERSION << " by matyalatte" << std::endl;
    m_file_tree = std::make_unique<FileTree>();
    CreateFrame();
}

void MainFrame::CreateFrame() {
    // make menu bar
    wxMenuBar* menu_bar = new wxMenuBar;
    wxMenu* menu_file = new wxMenu;

    menu_file->Append(wxID_OPEN, "Open");
    menu_file->Append(wxID_SAVE, "Save");
    menu_file->Append(wxID_EXIT, "Quit");
    menu_bar->Append(menu_file, "Menu");

    SetMenuBar(menu_bar);

    // set events
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { OpenFileList(); }, wxID_OPEN);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { SaveFileList(); }, wxID_SAVE);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    // make tree viewer
    m_tree_ctrl = new CustomTreeCtrl(this, wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
    wxImageList* image_list = GetImageList();
    m_tree_ctrl->SetImageList(image_list);
    m_tree_ctrl->SetDropTarget(new FileDropTarget(this));
    SetSize(wxSize(320, 320));
    Fit();
}

const size_t MAX_PATH_LENGTH = 1000;

void MainFrame::ReadFileList(const char* file) {
    // Read txt and store path list as tree
    std::cout << "Reading " << file << "..." << std::endl;
    std::ifstream istream(file);
    char line[MAX_PATH_LENGTH];
    m_file_tree = std::make_unique<FileTree>();
    for (line; istream.getline(line, MAX_PATH_LENGTH); ) {
        m_file_tree->AddItem(&line[0]);
    }
}

void MainFrame::OpenFileList(const wxString& filename) {
    if (!HasEmptyList() && !IsSaved()) {
        // Check if current tree is saved or not
        int ret = wxMessageBox(_("Current content has not been saved! Proceed?"),
            _("Please confirm"),
            wxICON_QUESTION | wxYES_NO, this);
        if (ret == wxNO) return;
    }

    if (filename == "") {
        // File select
        wxFileDialog open_file_dialog(this, _("Open File List"), "", "",
                                      "file list (*)|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (open_file_dialog.ShowModal() == wxID_CANCEL)
            return;
        ReadFileList(open_file_dialog.GetPath().ToUTF8().data());
    } else {
        ReadFileList(filename.ToUTF8().data());
    }

    // Initialize tree viewer
    m_tree_ctrl->DeleteAllItems();
    m_tree_ctrl->AddRoot("root");

    // Load tree to tree viewer
    std::cout << "Constructiong GUI..." << std::endl;
    m_file_tree->AddToTreeCtrl(m_tree_ctrl);

    std::cout << "Done." << std::endl;
    m_tree_ctrl->SetSaveStatus(true);
}

void MainFrame::SaveFileList() {
    if (HasEmptyList()) {
        ShowSuccessDialog("File list is empty.", "Empty List");
        return;
    }

    // File select
    wxFileDialog save_file_dialog(this, _("Save File List"), "", "",
                                  "file list (*)|*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (save_file_dialog.ShowModal() == wxID_CANCEL)
        return;

    // Dump path list as txt
    std::cout << "Saving " << save_file_dialog.GetPath() << "..." << std::endl;
    std::ofstream ostream(save_file_dialog.GetPath().ToUTF8().data());
    m_file_tree->DumpPaths(ostream, m_tree_ctrl);
    std::cout << "Done." << std::endl;

    ShowSuccessDialog("Saved as " + save_file_dialog.GetPath() + ".", "Saved");
    m_tree_ctrl->SetSaveStatus(true);
}

void MainFrame::ShowErrorDialog(const wxString& msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Error", wxICON_ERROR | wxOK | wxCENTRE);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::ShowSuccessDialog(const wxString& msg, const wxString& title) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, title);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::OnClose(wxCloseEvent& event) {
    if (!HasEmptyList() && !IsSaved()) {
        // Check if current tree is saved or not
        int ret = wxMessageBox(_("Current content has not been saved! Proceed?"),
                                  _("Quit"),
                                  wxICON_QUESTION | wxYES_NO, this);
        if (ret == wxNO) return;
    }
    Destroy();
}
