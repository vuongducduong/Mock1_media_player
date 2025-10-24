#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "MediaFile.h"
#include <vector>
#include <memory>
#include <filesystem>

class MediaManager {
private:
    std::string folder;
    std::vector<std::shared_ptr<MediaFile>> mediafiles;
    int currentIndex;

public:
    MediaManager(const std::string& folder = "");
    MediaManager(const MediaManager& other) 
        : folder(other.folder), mediafiles(other.mediafiles), currentIndex(other.currentIndex) {}
    
    MediaManager& operator=(const MediaManager& other) {
        if (this != &other) {
            folder = other.folder;
            mediafiles = other.mediafiles;
            currentIndex = other.currentIndex;
        }
        return *this;
    }
    void load();
    void clear();
    
    void setFolder(const std::string& folder) { this->folder = folder; }
    const std::string& getFolder() const { return folder; }
    
    int size() const { return mediafiles.size(); }
    std::shared_ptr<MediaFile> getMediaFile(int index) const;
    std::shared_ptr<MediaFile> getCurrentMediaFile() const;
    
    void setCurrentIndex(int index) { currentIndex = index; }
    int getCurrentIndex() const { return currentIndex; }
    
    bool hasNext() const { return currentIndex < (int)mediafiles.size() - 1; }
    bool hasPrevious() const { return currentIndex > 0; }
    
    std::vector<std::string> getMediaFileNames() const;
    const std::vector<std::shared_ptr<MediaFile>>& getAllMediaFiles() const { return mediafiles; }
};

#endif