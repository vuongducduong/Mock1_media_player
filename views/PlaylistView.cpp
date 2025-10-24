#include "PlaylistView.h"

PlaylistView::PlaylistView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x), selectedIndex(-1) {}

void PlaylistView::draw() {
    werase(window);
    box(window, 0, 0);
    
    // Vẽ nút Add và Remove, edit
    std::string addBtn = "[ + Add ]";
    std::string removeBtn = "[ - Remove ]";
    std::string editBtn = "[ * Edit ]";
    mvwprintw(window, 1, 2, "%s", addBtn.c_str());
    mvwprintw(window, 1, 2 + addBtn.size() + 4, "%s", removeBtn.c_str());
    mvwprintw(window, 1, 2 + addBtn.size() + removeBtn.size() + 8, "%s", editBtn.c_str());
    
    // Tiêu đề
    mvwprintw(window, 2, 2, "Playlists (Click to view media):");
    
    // Danh sách playlist
    int y = 3;
    for (size_t i = 0; i < playlists.size(); ++i) {
        if ((int)i == selectedIndex) {
            wattron(window, A_REVERSE);
        }
        mvwprintw(window, y + i, 4, "%s", playlists[i].c_str());
        if ((int)i == selectedIndex) {
            wattroff(window, A_REVERSE);
        }
    }
    
    wrefresh(window);
}

int PlaylistView::getPlaylistAtY(int localY) const {
    int listStart = 3;
    int index = localY - listStart;
    if (index >= 0 && index < (int)playlists.size()) {
        return index;
    }
    return -1;
}

bool PlaylistView::isAddButtonClicked(int x, int localY) const {
    std::string addBtn = "[ + Add ]";
    return (localY == 1 && x >= 2 && x <= 2 + (int)addBtn.size());
}

bool PlaylistView::isRemoveButtonClicked(int x, int localY) const {
    std::string addBtn = "[ + Add ]";
    std::string removeBtn = "[ - Remove ]";
    int removeX = 2 + addBtn.size() + 4;
    return (localY == 1 && x >= removeX && x <= removeX + (int)removeBtn.size());
}

bool PlaylistView::isEditButtonClicked(int x, int localY) const {
    std::string addBtn = "[ + Add ]";
    std::string removeBtn = "[ - Remove ]";
    std::string editBtn = "[ * Edit ]";
    int editX = 2 + addBtn.size() + removeBtn.size() + 8;
    return (localY == 1 && x >= editX && x <= editX + (int)editBtn.size());
}