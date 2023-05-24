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

    void Mkdir(const wxString& dir) {
        if (!wxDirExists(dir)) {
            wxString parent = wxFileName(dir).GetPath();
            Mkdir(parent);
            if (!wxMkdir(dir)) {
                throw std::runtime_error("Error: Failed to make " + dir);
            }
        }
    }

    #define OPEN_ISTREAM \
        std::cout << "Reading " << i_file << "..." << std::endl; \
        std::ifstream istream(wxStrToChar(i_file)); \
        if (!istream) { \
            throw std::runtime_error("Error: Failed to open " + i_file); \
        }

    #define OPEN_OSTREAM \
        std::cout << "Writing " << o_file << "..." << std::endl; \
        std::ofstream ostream(wxStrToChar(o_file)); \
        if (!ostream) { \
            throw std::runtime_error("Error: Failed to open " + o_file); \
        }

    static const size_t MAX_PATH_LENGTH = 1000;

    int Lower(const wxString& i_file,
            const wxString& o_dir,
            const wxString& o_fname) {
        std::cout << "Converting all paths to lower case..." << std::endl;
        try {
            OPEN_ISTREAM;
            Mkdir(o_dir);
            wxString o_file = o_dir + wxFileName::GetPathSeparator() + o_fname;
            OPEN_OSTREAM;
            char line[MAX_PATH_LENGTH];
            while (istream.getline(line, MAX_PATH_LENGTH)) {
                ostream << wxString::FromUTF8(line).Lower().ToUTF8().data() << "\n";
            }
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            return 1;
        }
        std::cout << "Done!" << std::endl;
        return 0;
    }

    FileTree* ReadFileList(const wxString& i_file) {
        OPEN_ISTREAM;  // std::ifstream istream(i_file)
        char line[MAX_PATH_LENGTH];
        FileTree* file_tree = new FileTree();
        while (istream.getline(line, MAX_PATH_LENGTH)) {
            if (line[0] == 0) continue;
            file_tree->AddItem(&line[0]);
        }
        return file_tree;
    }

    int Sort(const wxString& i_file,
            const wxString& o_dir,
            const wxString& o_fname) {
        std::cout << "Sorting paths..." << std::endl;
        try {
            FileTree* file_tree = ReadFileList(i_file);
            Mkdir(o_dir);
            wxString o_file = o_dir + wxFileName::GetPathSeparator() + o_fname;
            OPEN_OSTREAM;
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
        try {
            FileTree* file_tree = ReadFileList(i_file);
            Mkdir(o_dir);
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
