#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "MediaManager.h"

class Playlist {
private:
    std::string playlistName;
    std::string path;
    MediaManager mediaManager;

public:
    Playlist(const std::string& playlistName, const std::string& basePath);
    
    const std::string& getPlayListName() const { return playlistName; }
    const std::string& getPath() const { return path; }
    
    void create(const std::vector<std::string>& songFiles, 
                const std::string& sourceFolder);
    void load();
    void remove();
    void setplaylistName(const std::string& newplaylistName) { playlistName = newplaylistName; } 
    
    MediaManager& getMediaFiles() { return mediaManager; }
    const MediaManager& getMediaFiles() const { return mediaManager; }
};

#endif
