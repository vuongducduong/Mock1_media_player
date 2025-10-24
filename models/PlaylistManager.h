#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include "Playlist.h"
#include <vector>
#include <memory>
#include "../views/AddPlaylistView.h"

class PlaylistManager {
private:
    std::string baseFolder;
    std::vector<std::shared_ptr<Playlist>> playlists;
    int selectedIndex;

public:
    PlaylistManager();
    
    void load();
    void createPlaylist(const std::string& name, 
                    const std::vector<SelectedMediaFileInfo>& mediafiles);
    void deletePlaylist(int index);
    
    int size() const { return playlists.size(); }
    std::shared_ptr<Playlist> getPlaylist(int index) const;
    std::shared_ptr<Playlist> getSelectedPlaylist() const;
    
    void setSelectedIndex(int index) { selectedIndex = index; }
    int getSelectedIndex() const { return selectedIndex; }
    
    std::vector<std::string> getPlaylistNames() const;
    void updatePlaylist(int index, const std::string& name, 
                    const std::vector<SelectedMediaFileInfo>& mediafiles);
};

#endif
