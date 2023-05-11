#include "main_frame.h"

// Main window
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title) {
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

    wxMenu* menu_help = new wxMenu;
    menu_help->Append(wxID_HIGHEST + 1, "README");
    menu_help->Bind(wxEVT_MENU, &MainFrame::OnOpenURL,
                    this, wxID_HIGHEST + 1);
    menu_bar->Append(menu_help, "Help");
    SetMenuBar(menu_bar);

    // set events
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { OpenFileList(); }, wxID_OPEN);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { SaveFileList(); }, wxID_SAVE);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    SetBackgroundColour(wxColour(230, 230, 230));

    // Sizer for all components
    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

    // Add text box for filter
    wxBoxSizer* text_sizer = new wxBoxSizer(wxHORIZONTAL);
    text_sizer->Add(
        new wxStaticText(this, -1, "Filter by Filename:"),
        0, wxALIGN_CENTER | wxALL, 1);
    m_filter_ctrl = new wxTextCtrl(
        this, -1, wxEmptyString,
        wxDefaultPosition, wxSize(100, 25), wxTE_PROCESS_ENTER);
    m_old_filter = wxEmptyString;
    Bind(wxEVT_TEXT_ENTER, &MainFrame::OnFilter, this);
    text_sizer->Add(m_filter_ctrl, 1, wxEXPAND | wxALL, 1);
    topsizer->Add(text_sizer, 0, wxEXPAND | wxALL, 1);

    // Add tree viewer
    m_tree_ctrl = new CustomTreeCtrl(this, wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
    wxImageList* image_list = GetImageList();
    m_tree_ctrl->SetImageList(image_list);
    m_tree_ctrl->SetDropTarget(new FileDropTarget(this));
    topsizer->Add(m_tree_ctrl, 1, wxEXPAND);

    SetSizerAndFit(topsizer);
    SetSize(wxSize(320, 320));
}

const size_t MAX_PATH_LENGTH = 1000;

bool MainFrame::ReadFileList(const char* file) {
    // Read txt and store path list as tree
    std::cout << "Reading " << file << "..." << std::endl;
    std::ifstream istream(file);
    if (!istream) {
        ShowErrorDialog("Error: Failed to open " + wxString(file));
        return false;
    }
    char line[MAX_PATH_LENGTH];
    m_file_tree = std::make_unique<FileTree>();
    for (line; istream.getline(line, MAX_PATH_LENGTH); ) {
        m_file_tree->AddItem(&line[0]);
    }
    return true;
}

void MainFrame::OpenFileList(wxString filename) {
    if (!HasEmptyList() && !IsSaved()) {
        // Check if current tree is saved or not
        int ret = wxMessageBox(_("Current content has not been saved! Proceed?"),
            _("Please confirm"),
            wxICON_QUESTION | wxYES_NO, this);
        if (ret == wxNO) return;
    }

    bool res = false;
    if (filename == "") {
        // File select
        wxFileDialog open_file_dialog(this, _("Open File List"), "", "",
                                      "file list (*)|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (open_file_dialog.ShowModal() == wxID_CANCEL)
            return;
        filename = open_file_dialog.GetPath();
    }

    wxStopWatch  stop_watch;
    stop_watch.Start();

    res = ReadFileList(filename.ToUTF8().data());
    if (!res) { return; }

    // Initialize tree viewer
    m_tree_ctrl->DeleteAllItems();
    m_tree_ctrl->AddRoot("root");

    // Load tree to tree viewer
    std::cout << "Constructiong GUI..." << std::endl;
    m_file_tree->InitializeTreeCtrl(m_tree_ctrl);
    m_tree_ctrl->SortAll(m_tree_ctrl->GetRootItem());
    std::cout << "Done. (" << stop_watch.Time() << " ms)" << std::endl;
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
    std::cout << "Saving " << save_file_dialog.GetPath() << "... ";
    std::ofstream ostream(save_file_dialog.GetPath().ToUTF8().data());
    if (!ostream) {
        ShowErrorDialog("Error: Failed to open " + save_file_dialog.GetPath());
        return;
    }
    m_file_tree->DumpPaths(ostream, m_tree_ctrl);

    ShowSuccessDialog("Saved as " + save_file_dialog.GetPath() + ".", "Saved");
    m_tree_ctrl->SetSaveStatus(true);
}

void MainFrame::OnOpenURL(wxCommandEvent& event) {
    size_t id = event.GetId() - 1 - wxID_HIGHEST;
    wxString url[1] = {"https://github.com/matyalatte/File-List-Viewer"};
    std::cout << "Opening " << url[id] << "..." << std::endl;
    bool success = wxLaunchDefaultBrowser(url[id]);
    if (!success) {
        ShowErrorDialog("Failed to open url by an unexpected error.");
    }
}

void MainFrame::ShowErrorDialog(const wxString& msg) {
    wxMessageDialog* dialog;
    dialog = new wxMessageDialog(this, msg, "Error", wxICON_ERROR | wxOK | wxCENTRE);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::ShowSuccessDialog(const wxString& msg, const wxString& title) {
    wxMessageDialog* dialog;
    std::cout << msg << std::endl;
    dialog = new wxMessageDialog(this, msg, title);
    dialog->ShowModal();
    dialog->Destroy();
}

void MainFrame::OnFilter(wxCommandEvent& event) {
    wxString new_filter = m_filter_ctrl->GetValue();
    if (m_old_filter == new_filter) { return; }
    wxStopWatch  stop_watch;
    stop_watch.Start();
    std::cout << "Filtering... ";
    m_file_tree->Filter(std::string(new_filter.utf8_str()), m_tree_ctrl);
    m_tree_ctrl->UpdateAllStatus(m_tree_ctrl->GetRootItem());
    m_tree_ctrl->SortAll(m_tree_ctrl->GetRootItem());
    m_old_filter = new_filter;
    std::cout << "Done. (" << stop_watch.Time() << " ms)" << std::endl;
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
