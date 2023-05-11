#include <wx/filename.h>
#include "main_frame.h"

// Main
class MainApp : public wxApp {
 private:
    MainFrame* m_frame;
 public:
    MainApp(): wxApp() {
        m_frame = nullptr;
    }
    bool OnInit() override;
};

bool MainApp::OnInit() {
    if (!wxApp::OnInit())
        return false;
    // make main window
    m_frame = new MainFrame();
    m_frame->Show();

    if (argc > 1) {
        m_frame->OpenFileList(argv[argc - 1]);
    }

    return true;
}

wxDECLARE_APP(MainApp);
wxIMPLEMENT_APP_NO_MAIN(MainApp);

void PrintHelp() {
    static const char* const usage =
        "Usage: FileListViewer <command> <options> <input_file>\n"
        "\n"
        "  <command>:\n"
        "    --help: Show this massage.\n"
        "    --lower: Convert paths to lower case.\n"
        "    --sort: Sort paths in alphabetical order.\n"
        "    --mkdir: Generate all folders that are in the path lists.\n"
        "    `no command`: Open a file with GUI.\n"
        "\n"
        "  <options>:\n"
        "    -o <dir>: Output folder for commands.\n"
        "              (defaults to 'out')\n"
        "    -f <file>: Output file name for commands.\n"
        "               (defaults to the input file name.)\n"
        "\n"
        "Examples:\n"
        "    FileListViewer input.txt\n"
        "    FileListViewer --lower -o lowercase -f new.txt input.txt\n"
        "    FileListViewer --sort -o sorted -f new.txt input.txt\n"
        "    FileListViewer --mkdir -o dirs input.txt\n"
        "\n";

    printf("%s", usage);
}

#ifdef _WIN32
const wxWCharBuffer wxStrToChar(const wxString& str) {
    return str.wc_str();
}
wxString CharToWxStr(const wchar_t* arg) {
    return wxString(arg);
}
#else
const wxScopedCharBuffer wxStrToChar(const wxString& str) {
    return str.utf8_str();
}
wxString CharToWxStr(const char* arg) {
    return wxString::FromUTF8(arg);
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
    CMD_HELP,
    CMD_LOWER,
    CMD_SORT,
    CMD_MKDIR,
    CMD_MAX
};

int CommandToInt(const wxString& command) {
    static const std::map<wxString, int> COMMAND_MAP = {
        {"--help", CMD_HELP},
        {"--lower", CMD_LOWER},
        {"--sort", CMD_SORT},
        {"--mkdir", CMD_MKDIR}
    };

    if (COMMAND_MAP.count(command) == 0) {
        return CMD_UNKNOWN;
    }
    return COMMAND_MAP.at(command);
}

bool ParseOptions(int argc, wchar_t* argv[],
                  wxString& output_dir,
                  wxString& output_filename,
                  wxString& input_file) {
    if (argc <= 2) return true;
    
    for (int i = 2; i < argc; i++) {
        wxString arg(CharToWxStr(argv[i]));
        if (arg[0] == "-"[0]) {
            if (arg != "-o" && arg != "-f") {
                PrintHelp();
                std::cout << "Error: Unknown ooption. (" << arg << ")" << std::endl;
                return false;
            }
            if (i + 1 >= argc) {
                PrintHelp();
                std::cout << "Error: '" << arg << "' option requires a path." << std::endl;
                return false;
            }
            wxString path(CharToWxStr(argv[i + 1]));
            if (arg == "-o") {
                output_dir = path;
            }
            else {
                output_filename = path;
            }
            i += 1;
        }
        else {
            if (i + 1 < argc) {
                PrintHelp();
                std::cout << "Error: Unused arguments. (" << argv[i + 1] << ")" << std::endl;
                return false;
            }
            input_file = arg;
        }
    }
    return true;
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
    // Use GUI
    if (argc <= 1) return wxEntry(argc, argv);

    // Use command-line tool
    wxString command(CharToWxStr(argv[1]));

    if (command[0] != "-"[0]) return wxEntry(argc, argv);  // Open a file with GUI.

    int cmd_id = CommandToInt(command);

    if (cmd_id == CMD_UNKNOWN) {
        PrintHelp();
        std::cout << "Error: Unknown command. (" << command << ")" << std::endl;
        return 1;
    }

    wxString output_dir = "out";
    wxString output_filename = "";
    wxString input_file = "";

    bool success = ParseOptions(argc, argv, output_dir, output_filename, input_file);
    if (!success) return 1;

    if (cmd_id != CMD_HELP && input_file == "") {
        PrintHelp();
        std::cout << "Error: '" << command << "' command requires a file path." << std::endl;
        return 1;
    }

    if (output_filename == "") {
        wxString fname;
        wxString ext;
        wxFileName::SplitPath(input_file, nullptr, nullptr, &fname, &ext);
        output_filename = fname + "." + ext;
    }

    success = PathToAbsolute(output_dir);
    std::cout << output_dir << std::endl;
    if (!success) return 1;

    int ret = 0;
    switch (cmd_id) {
        case CMD_HELP:
            PrintHelp();
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
