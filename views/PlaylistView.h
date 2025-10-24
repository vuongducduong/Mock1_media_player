#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include "BaseView.h"

class PlaylistView : public BaseView {
private:
    std::vector<std::string> playlists;
    int selectedIndex;

public:
    PlaylistView(int h, int w, int y, int x);
    
    void draw() override;
    void setPlaylists(const std::vector<std::string>& p) { playlists = p; }
    void setSelectedIndex(int i) { selectedIndex = i; }
    
    int getPlaylistAtY(int y) const;
    bool isAddButtonClicked(int x, int localY) const;
    bool isRemoveButtonClicked(int x, int localY) const;
    bool isEditButtonClicked(int x, int localY) const;
};

#endif
