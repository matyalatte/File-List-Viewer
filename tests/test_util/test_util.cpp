#include <string>
#include "test_util.h"

namespace test_util {
    bool HasSameList(const std::string& file1, const std::string& file2) {
        std::ifstream istream1(file1);
        std::ifstream istream2(file2);
        if (!istream1 || !istream2) return false;
        std::string line1;
        std::string line2;
        std::cout << "Result:" << std::endl;
        while (std::getline(istream1, line1)
            && std::getline(istream2, line2)) {
            if (line1 != line2) return false;
            std::cout << "  " << line1 << std::endl;
        }
        if (std::getline(istream1, line1) || std::getline(istream2, line2)) return false;
        return true;
    }

    bool HasDirs(const std::string& dir, const std::string& list) {
        std::ifstream istream(list);
        if (!istream) return false;
        std::string line;
        while (std::getline(istream, line)) {
            wxString path = dir + "/" + wxFileName(line).GetPath();
            std::cout << path << std::endl;
            if (!wxDirExists(path) || wxFileExists(line)) return false;
        }
        return true;
    }

    bool IsEmptyFile(const std::string& file) {
        std::ifstream istream(file);
        if (!istream) return false;
        std::string line;
        while (std::getline(istream, line)) {
            if (line != "") return false;
        }
        return true;
    }
}  // namespace test_util
