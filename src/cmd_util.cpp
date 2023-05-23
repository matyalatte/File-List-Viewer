#include "cmd_util.h"

namespace cmd_util {
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

    static const size_t MAX_PATH_LENGTH = 1000;

    int Lower(const wxString& i_file,
            const wxString& o_dir,
            const wxString& o_fname) {
        std::cout << "Converting all paths to lower case..." << std::endl;
        OPEN_ISTREAM;
        if (!Mkdir(o_dir)) { return 1; }
        wxString o_file = o_dir + wxFileName::GetPathSeparator() + o_fname;
        OPEN_OSTREAM;
        char line[MAX_PATH_LENGTH];
        while (istream.getline(line, MAX_PATH_LENGTH)) {
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

        char line[MAX_PATH_LENGTH];
        FileTree* file_tree = new FileTree();
        while (istream.getline(line, MAX_PATH_LENGTH)) {
            file_tree->AddItem(&line[0]);
        }

        wxString o_file = o_dir + wxFileName::GetPathSeparator() + o_fname;
        OPEN_OSTREAM;
        try {
            file_tree->DumpPaths(ostream, nullptr);
        }
        catch (std::exception& e) {
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

        char line[MAX_PATH_LENGTH];
        FileTree* file_tree = new FileTree();
        while (istream.getline(line, MAX_PATH_LENGTH)) {
            file_tree->AddItem(&line[0]);
        }
        try {
            file_tree->MakeDir(o_dir);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            return 1;
        }
        std::cout << "Created a folder tree in " << o_dir << std::endl;
        return 0;
    }

    #undef OPEN_ISTREAM
    #undef OPEN_OSTREAM
}  // namespace cmd_util
