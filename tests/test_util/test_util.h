#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "wx/filename.h"

namespace test_util {
    bool HasSameList(const std::string& file1, const std::string& file2);
    bool HasDirs(const std::string& dir, const std::string& list);
    bool IsEmptyFile(const std::string& file);
}
