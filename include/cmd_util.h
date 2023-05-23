#pragma once
#include <iostream>
#include <fstream>
#include "wx/filename.h"
#include "file_tree.h"

namespace cmd_util {
    int Lower(const wxString& i_file,
              const wxString& o_dir,
              const wxString& o_fname);
    int Sort(const wxString& i_file,
             const wxString& o_dir,
             const wxString& o_fname);
    int MakeDir(const wxString& i_file,
                const wxString& o_dir);
}
