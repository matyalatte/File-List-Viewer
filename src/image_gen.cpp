#include "image_gen.h"
#include <iostream>

// Get icons for tree items
wxImageList* GetImageList() {
wxArtProvider artprov;
    int x = 16;
    int y = 16;
    wxSize size(x, y);
    wxImageList* image_list = new wxImageList(x, y, false);
    wxArtID art_ids[] = { wxART_FOLDER, wxART_NORMAL_FILE, wxART_TICK_MARK };
    for (auto id : art_ids) {
        wxIcon icon = artprov.GetIcon(id, wxART_OTHER, size);
        image_list->Add(icon);
    }
    return image_list;
}
