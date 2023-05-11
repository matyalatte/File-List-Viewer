#pragma once
#include <wx/artprov.h>
#include <wx/imaglist.h>

// IDs for image list
enum ImageID : int {
    IMAGE_ID_FOLDER,  // wxART_FOLDER
    IMAGE_ID_FILE,    // wxART_NORMAL_FILE
    IMAGE_ID_CHECK,   // wxART_TICK_MARK
    IMAGE_ID_EMPTY,
    IMAGE_ID_MAX
};

// Get icons for tree items
wxImageList* GetImageList();
