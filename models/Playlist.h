#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "SongCollection.h"

class Playlist {
private:
    std::string name;
    std::string path;
    SongCollection songs;

public:
    Playlist(const std::string& name, const std::string& basePath);
    
    const std::string& getName() const { return name; }
    const std::string& getPath() const { return path; }
    
    void create(const std::vector<std::string>& songFiles, 
                const std::string& sourceFolder);
    void load();
    void remove();
    
    SongCollection& getSongs() { return songs; }
    const SongCollection& getSongs() const { return songs; }
};

#endif
