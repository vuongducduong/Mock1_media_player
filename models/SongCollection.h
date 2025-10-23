#ifndef SONGCOLLECTION_H
#define SONGCOLLECTION_H

#include "Song.h"
#include <vector>
#include <memory>
#include <filesystem>

class SongCollection {
private:
    std::string folder;
    std::vector<std::shared_ptr<Song>> songs;
    int currentIndex;

public:
    SongCollection(const std::string& folder = "");
    
    void load();
    void clear();
    
    void setFolder(const std::string& folder) { this->folder = folder; }
    const std::string& getFolder() const { return folder; }
    
    int size() const { return songs.size(); }
    std::shared_ptr<Song> getSong(int index) const;
    std::shared_ptr<Song> getCurrentSong() const;
    
    void setCurrentIndex(int index) { currentIndex = index; }
    int getCurrentIndex() const { return currentIndex; }
    
    bool hasNext() const { return currentIndex < (int)songs.size() - 1; }
    bool hasPrevious() const { return currentIndex > 0; }
    
    std::vector<std::string> getSongNames() const;
};

#endif