// Tests for main_frame.cpp

#include <gtest/gtest.h>
#include "wx/app.h"
#include "wx/modalhook.h"
#include "main_frame.h"
#include "test_util.h"

char const * test_source_dir;
char const * test_binary_dir;

std::string GetTestList() {
    return std::string(test_source_dir) + "/test_list.txt";
}

std::string GetOutDir() {
    return std::string(test_binary_dir) + "/test_out";
}

// Hook to skip message dialogues
class DialogSkipper : public wxModalDialogHook {
 protected:
    virtual int Enter(wxDialog* dialog) {
        if (wxDynamicCast(dialog, wxMessageDialog)) {
            return wxID_CANCEL;
        }
        return wxID_NONE;
    }
    virtual void Exit(wxDialog* dialog){}
};

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

    // Make dummy app
    wxApp* app = new wxApp();

    // Initialize app
    int argc_ = 1;
    char *argv_[1] = { argv[0] };
    wxEntryStart(argc_, argv_);
    app->OnInit();

    // Make hook to skip message dialogues
    DialogSkipper* hook = new DialogSkipper();
    hook->Register();

    return RUN_ALL_TESTS();
}

class TestFrame: public MainFrame {
 public:
    void SelectAll(bool status = true) {
        std::map<std::string, FileTree*> items = m_file_tree->GetItems();
        for (auto iter = items.begin(); iter != items.end(); ++iter) {
            wxTreeItemId id = iter->second->GetId();
            m_tree_ctrl->SetEventItem(id);
            if (m_tree_ctrl->IsChecked(id) != status) m_tree_ctrl->SelectAll();
        }
    }
    void Filter(const std::string& filter) {
        std::cout << "Filtering... ";
        m_file_tree->Filter(filter, m_tree_ctrl);
        m_tree_ctrl->UpdateAllStatus(m_tree_ctrl->GetRootItem());
        m_tree_ctrl->SortAll(m_tree_ctrl->GetRootItem());
        std::cout << "Done." << std::endl;
    }
};

TEST(MainFrameTest, ExportAllPaths) {
    std::string test_list = GetTestList();
    TestFrame* main_frame = new TestFrame();
    main_frame->OpenFileList(test_list);
    main_frame->SelectAll(true);
    std::string file = GetOutDir() + "/test_list.txt";
    main_frame->SaveFileList(file);
    EXPECT_TRUE(test_util::HasSameList(test_list, file));
}

TEST(MainFrameTest, AbsolutePaths) {
    std::string test_list = std::string(test_source_dir) + "/abs_path.txt";
    TestFrame* main_frame = new TestFrame();
    main_frame->OpenFileList(test_list);
    main_frame->SelectAll(true);
    std::string file = GetOutDir() + "/abs_path.txt";
    main_frame->SaveFileList(file);
    EXPECT_TRUE(test_util::HasSameList(test_list, file));
}

TEST(MainFrameTest, BackSlashPaths) {
    std::string test_list = std::string(test_source_dir) + "/back_slash.txt";
    TestFrame* main_frame = new TestFrame();
    main_frame->OpenFileList(test_list);
    main_frame->SelectAll(true);
    std::string file = GetOutDir() + "/back_slash.txt";
    main_frame->SaveFileList(file);
    EXPECT_TRUE(test_util::HasSameList(GetTestList(), file));
}

TEST(MainFrameTest, UncheckAllPaths) {
    TestFrame* main_frame = new TestFrame();
    main_frame->OpenFileList(GetTestList());
    main_frame->SelectAll(true);
    main_frame->SelectAll(false);
    std::string file = GetOutDir() + "/empty.txt";
    main_frame->SaveFileList(file);
    EXPECT_TRUE(test_util::IsEmptyFile(file));
}

TEST(MainFrameTest, ApplyFilter) {
    TestFrame* main_frame = new TestFrame();
    main_frame->OpenFileList(GetTestList());
    main_frame->Filter(".txt");
    main_frame->SelectAll(true);
    std::string file = "/filtered.txt";
    main_frame->SaveFileList(GetOutDir() + file);
    EXPECT_TRUE(test_util::HasSameList(test_source_dir + file, GetOutDir() + file));
}

TEST(MainFrameTest, ClearFilter) {
    TestFrame* main_frame = new TestFrame();
    main_frame->OpenFileList(GetTestList());
    main_frame->Filter(".txt");
    main_frame->SelectAll(true);
    main_frame->Filter("");
    main_frame->SaveFileList(GetOutDir() + "/filtered2.txt");
    EXPECT_TRUE(test_util::HasSameList(std::string(test_source_dir) + "/filtered.txt",
                                       GetOutDir() + "/filtered2.txt"));
}

TEST(MainFrameTest, ClearFilter2) {
    TestFrame* main_frame = new TestFrame();
    std::string test_list = GetTestList();
    main_frame->OpenFileList(test_list);
    main_frame->Filter(".png");
    main_frame->SelectAll(true);
    main_frame->Filter("");
    main_frame->SelectAll(true);
    std::string file = GetOutDir() + "/filtered3.txt";
    main_frame->SaveFileList(file);
    EXPECT_TRUE(test_util::HasSameList(test_list, file));
}
