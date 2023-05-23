// Tests for cmd_util.cpp

#include <gtest/gtest.h>
#include "cmd_util.h"
#include "test_util.h"

char const * test_source_dir;
char const * test_binary_dir;

std::string GetTestList() {
    return std::string(test_source_dir) + "/test_list.txt";
}

std::string GetOutDir() {
    return std::string(test_binary_dir) + "/test_out";
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 3);
    test_source_dir = argv[1];
    test_binary_dir = argv[2];
    std::string out_dir = GetOutDir();
    if (!wxDirExists(out_dir) && !wxMkdir(out_dir)) {
        wxString msg = std::string("Error: Failed to make ") + out_dir;
        throw std::runtime_error(msg);
    }
    return RUN_ALL_TESTS();
}

TEST(CmdUtilTest, Lower) {
    std::string file = "lowered.txt";
    std::string out_dir = GetOutDir();
    cmd_util::Lower(GetTestList(), out_dir, file);
    EXPECT_TRUE(test_util::HasSameList(std::string(test_source_dir) + "/" + file,
                                       out_dir + "/" + file));
}

TEST(CmdUtilTest, Sort) {
    std::string file = "sorted.txt";
    std::string out_dir = GetOutDir();
    cmd_util::Sort(GetTestList(), out_dir, file);
    EXPECT_TRUE(test_util::HasSameList(std::string(test_source_dir) + "/" + file,
                                       out_dir + "/" + file));
}

TEST(CmdUtilTest, MakeDir) {
    std::string test_list = GetTestList();
    std::string out_dir = GetOutDir() + "/foo/bar";
    cmd_util::MakeDir(test_list, out_dir);
    EXPECT_TRUE(test_util::HasDirs(out_dir, test_list));
}
