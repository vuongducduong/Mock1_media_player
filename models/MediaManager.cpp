#include "MediaManager.h"

MediaManager::MediaManager(const std::string& folder) 
    : folder(folder), currentIndex(-1) {}

void MediaManager::load() {
    mediafiles.clear();
    if (!std::filesystem::exists(folder)) return;
    
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file() || entry.is_symlink()) {
            auto path = entry.path();
            auto ext = path.extension().string();
            
            for (char& c : ext) c = std::tolower(c);
            
            if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || 
                ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".flv") {
                mediafiles.push_back(std::make_shared<MediaFile>(
                    path.filename().string(), folder));
            }
        }
    }
}

void MediaManager::clear() {
    mediafiles.clear();
    currentIndex = -1;
}

std::shared_ptr<MediaFile> MediaManager::getMediaFile(int index) const {
    if (index >= 0 && index < (int)mediafiles.size()) {
        return mediafiles[index];
    }
    return nullptr;
}

std::shared_ptr<MediaFile> MediaManager::getCurrentMediaFile() const {
    return getMediaFile(currentIndex);
}

std::vector<std::string> MediaManager::getMediaFileNames() const {
    std::vector<std::string> names;
    for (const auto& MediaFile : mediafiles) {
        names.push_back(MediaFile->getFilename());
    }
    return names;
}
