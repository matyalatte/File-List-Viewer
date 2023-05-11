#pragma once
#include <wx/artprov.h>
#include <wx/imaglist.h>

// IDs for image list
enum class ImageID {
    FOLDER,  // wxART_FOLDER
    FILE,    // wxART_NORMAL_FILE
    CHECK,   // wxART_TICK_MARK
    EMPTY,
    MAX
};

// Get icons for tree items
wxImageList* GetImageList();
