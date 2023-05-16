#include <wx/filename.h>
#include <wx/cmdline.h>
#include <wx/msgout.h>
#include "main_frame.h"

// Metadata
static const std::string TOOL_NAME = "File List Viewer";
static const std::string VERSION = "0.2.0";
static const std::string AUTHOR = "matyalatte";

// command-line arguments
static const wxCmdLineEntryDesc cmd_line_desc[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "c", "cmd", "use command-line utils",
        wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_USAGE_TEXT, NULL, NULL,
        "                          lower: convert paths to lower case strings\n"
        "                          sort : sort paths in alphabetical order\n"
        "                          mkdir: create folders that exist in the list"
        },
    { wxCMD_LINE_OPTION, "o", "out", "output directory for commands (defaults to 'out')",
        wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_OPTION, "f", "file", "output filename for commands (defaults to the input filename)",
        wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_PARAM,  NULL, NULL, "input file",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

class MainApp : public wxApp {
 private:
    MainFrame* m_frame;
    wxString m_input_file;
 public:
    MainApp(): wxApp() {
        m_frame = nullptr;
        m_input_file = wxEmptyString;
    }
    bool OnInit() override;
    void OnInitCmdLine(wxCmdLineParser& parser) override;
    bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

bool MainApp::OnInit() {
    if (!wxApp::OnInit())
        return false;
    // make main window
    m_frame = new MainFrame(TOOL_NAME + " v" + VERSION);
    m_frame->Show();
    if (m_input_file != wxEmptyString) {
        m_frame->OpenFileList(m_input_file);
    }
    return true;
}

void MainApp::OnInitCmdLine(wxCmdLineParser& parser) {
    parser.SetDesc (cmd_line_desc);
    parser.SetSwitchChars (wxT("-"));
}

bool MainApp::OnCmdLineParsed(wxCmdLineParser& parser) {
    size_t file_count = parser.GetParamCount();
    if (file_count > 0) {
        m_input_file = parser.GetParam();
    }
    return true;
}

wxDECLARE_APP(MainApp);
wxIMPLEMENT_APP_NO_MAIN(MainApp);

#ifdef _WIN32
const wxWCharBuffer wxStrToChar(const wxString& str) {
    return str.wc_str();
}
#else
const wxScopedCharBuffer wxStrToChar(const wxString& str) {
    return str.utf8_str();
}
#endif

bool Mkdir(const wxString& dir) {
    if (!wxDirExists(dir)) {
        wxString parent = wxFileName(dir).GetPath();
        if (!Mkdir(parent)) return false;
        if (!wxMkdir(dir)) {
            std::cout << "Error: Failed to make " << dir << std::endl;
            return false;
        }
    }
    return true;
}

#define OPEN_ISTREAM \
    std::cout << "Reading " << i_file << "..." << std::endl; \
    std::ifstream istream(wxStrToChar(i_file)); \
    if (!istream) { \
        std::cout << "Error: Failed to open " << i_file << std::endl; \
        return 1; \
    }

#define OPEN_OSTREAM \
    std::cout << "Writing " << o_file << "..." << std::endl; \
    std::ofstream ostream(wxStrToChar(o_file)); \
    if (!ostream) { \
        std::cout << "Error: Failed to open " << o_file << std::endl; \
        return 1; \
    }

int Lower(const wxString& i_file,
          const wxString& o_dir,
          const wxString& o_fname) {
    std::cout << "Converting all paths to lower case..." << std::endl;
    OPEN_ISTREAM;
    if (!Mkdir(o_dir)) { return 1; }
    wxString o_file = o_dir + wxFileName::GetPathSeparator() + o_fname;
    OPEN_OSTREAM;
    const size_t MAX_PATH_LENGTH = 1000;
    char line[MAX_PATH_LENGTH];
    for (line; istream.getline(line, MAX_PATH_LENGTH); ) {
        ostream << wxString::FromUTF8(line).Lower().ToUTF8().data() << "\n";
    }
    std::cout << "Done!" << std::endl;
    return 0;
}

int Sort(const wxString& i_file,
         const wxString& o_dir,
         const wxString& o_fname) {
    std::cout << "Sorting paths..." << std::endl;
    OPEN_ISTREAM;
    if (!Mkdir(o_dir)) { return 1; }

    const size_t MAX_PATH_LENGTH = 1000;
    char line[MAX_PATH_LENGTH];
    FileTree* file_tree = new FileTree();
    for (line; istream.getline(line, MAX_PATH_LENGTH); ) {
        file_tree->AddItem(&line[0]);
    }

    wxString o_file = o_dir + wxFileName::GetPathSeparator() + o_fname;
    OPEN_OSTREAM;
    try {
        file_tree->DumpPaths(ostream, nullptr);
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    std::cout << "Done!" << std::endl;
    return 0;
}

int MakeDir(const wxString& i_file,
             const wxString& o_dir) {
    std::cout << "Creating a folder tree from path list..." << std::endl;

    OPEN_ISTREAM;
    if (!Mkdir(o_dir)) { return 1; }

    const size_t MAX_PATH_LENGTH = 1000;
    char line[MAX_PATH_LENGTH];
    FileTree* file_tree = new FileTree();
    for (line; istream.getline(line, MAX_PATH_LENGTH); ) {
        file_tree->AddItem(&line[0]);
    }
    try {
        file_tree->MakeDir(o_dir);
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    std::cout << "Created a folder tree in " << o_dir << std::endl;
    return 0;
}

enum COMMANDS : int {
    CMD_UNKNOWN,
    CMD_GUI,
    CMD_LOWER,
    CMD_SORT,
    CMD_MKDIR,
    CMD_MAX
};

int CommandToInt(const wxString& command) {
    static const std::map<wxString, int> COMMAND_MAP = {
        {"lower", CMD_LOWER},
        {"sort", CMD_SORT},
        {"mkdir", CMD_MKDIR}
    };

    if (COMMAND_MAP.count(command) == 0) {
        return CMD_UNKNOWN;
    }
    return COMMAND_MAP.at(command);
}

bool PathToAbsolute(wxString& str) {
    wxFileName path(str);
    bool success = path.MakeAbsolute();
    if (!success) {
        std::cout << "Error: Failed to make an absolute path from (" << str << ")" << std::endl;
        return false;
    }
    str = path.GetFullPath();
    return true;
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    // Print Logo
    const std::string msg = " " + TOOL_NAME + " v" + VERSION + " by " + AUTHOR + " ";
    std::cout << std::string(msg.length(), '-') << std::endl;
    std::cout << msg << std::endl;
    std::cout << std::string(msg.length(), '-') << std::endl;

    // Parse arguments
    wxMessageOutputStderr* msgout = new wxMessageOutputStderr();
    wxMessageOutput::Set(msgout);
    wxCmdLineParser* parser = new wxCmdLineParser(argc, argv);
    parser->SetDesc(cmd_line_desc);
    parser->SetSwitchChars("-");
    int ret = parser->Parse();
    if (ret == -1) return 0;
    if (ret > 0) return 1;

    // Get command
    wxString command = "";
    int cmd_id;
    bool found = parser->Found("c", &command);
    if (found) {
        cmd_id = CommandToInt(command);
        if (cmd_id == CMD_UNKNOWN) {
            parser->Usage();
            std::cout << "Error: Unknown command. (" << command << ")" << std::endl;
            return 1;
        }
    } else {  // no command-line utils
        cmd_id = CMD_GUI;
    }

    // Get options
    wxString output_dir = "out";
    wxString output_filename = "";
    wxString input_file = "";
    found = parser->Found("o", &output_dir) || parser->Found("f", &output_filename);
    if (found && cmd_id == CMD_GUI) {
        parser->Usage();
        std::cout << "Error: No need optional arguments for GUI." << std::endl;
        return 1;
    }

    // Get input file
    size_t file_count = parser->GetParamCount();
    if (file_count > 0) {
        input_file = parser->GetParam();
    }

    // Check arguments
    if (cmd_id != CMD_GUI && input_file == "") {
        parser->Usage();
        std::cout << "Error: '" << command << "' command requires a file path." << std::endl;
        return 1;
    }
    if (output_filename == "") {
        wxString fname;
        wxString ext;
        wxFileName::SplitPath(input_file, nullptr, nullptr, &fname, &ext);
        output_filename = fname + "." + ext;
    }
    bool success = PathToAbsolute(output_dir);
    if (!success) return 1;

    // Run command
    delete parser;
    wxMessageOutput::Set(nullptr);
    delete msgout;
    ret = 0;
    switch (cmd_id) {
        case CMD_GUI:
            ret = wxEntry(argc, argv);
            break;
        case CMD_LOWER:
            ret = Lower(input_file, output_dir, output_filename);
            break;
        case CMD_SORT:
            ret = Sort(input_file, output_dir, output_filename);
            break;
        case CMD_MKDIR:
            ret = MakeDir(input_file, output_dir);
            break;
    }
    return ret;
}
