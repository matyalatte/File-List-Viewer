#include <wx/filename.h>
#include <wx/cmdline.h>
#include <wx/msgout.h>
#include "main_frame.h"
#include "cmd_util.h"

// Metadata
constexpr char* TOOL_NAME = "File List Viewer";
constexpr char* VERSION = "0.2.0";
constexpr char* AUTHOR = "matyalatte";

// command-line arguments
static const wxCmdLineEntryDesc cmd_line_desc[] = {
    { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "c", "cmd", "uses command-line utils",
        wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_USAGE_TEXT, NULL, NULL,
        "                          lower: converts paths to lower case strings\n"
        "                          sort : sorts paths in alphabetical order\n"
        "                          mkdir: creates folders that exist in the list"
        },
    { wxCMD_LINE_OPTION, "o", "out",
        "output directory for commands (defaults to the directory of the input file)",
        wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_OPTION, "f", "file",
        "output filename for commands (defaults to the input filename + '.new')",
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
    MainApp(): wxApp(), m_frame(nullptr), m_input_file(wxEmptyString) {}
    bool OnInit() override;
    void OnInitCmdLine(wxCmdLineParser& parser) override;
    bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

bool MainApp::OnInit() {
    if (!wxApp::OnInit())
        return false;
    // make main window
    m_frame = new MainFrame(std::string(TOOL_NAME) + " v" + VERSION);
    m_frame->Show();
    if (m_input_file != wxEmptyString) {
        m_frame->OpenFileList(m_input_file);
    }
    return true;
}

void MainApp::OnInitCmdLine(wxCmdLineParser& parser) {
    parser.SetDesc(cmd_line_desc);
    parser.SetSwitchChars(wxT("-"));
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
    const std::string msg = std::string(" ") + TOOL_NAME + " v" + VERSION + " by " + AUTHOR + " ";

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
    wxString output_dir = "";
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
    bool success = PathToAbsolute(input_file);
    if (!success) return 1;


    if (output_filename == "") {
        // defaults to the input file name + ".new"
        wxString fname;
        wxString ext;
        wxFileName::SplitPath(input_file, nullptr, nullptr, &fname, &ext);
        output_filename = fname + "." + ext + ".new";
    }
    if (output_dir == "") {
        // defaults to the directory of the input file
        output_dir = wxFileName(input_file).GetPath();
    }

    success = PathToAbsolute(output_dir);
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
            ret = cmd_util::Lower(input_file, output_dir, output_filename);
            break;
        case CMD_SORT:
            ret = cmd_util::Sort(input_file, output_dir, output_filename);
            break;
        case CMD_MKDIR:
            ret = cmd_util::MakeDir(input_file, output_dir);
            break;
    }
    return ret;
}
