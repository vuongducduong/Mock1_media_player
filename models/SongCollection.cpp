#include "SongCollection.h"

SongCollection::SongCollection(const std::string& folder) 
    : folder(folder), currentIndex(-1) {}

void SongCollection::load() {
    songs.clear();
    if (!std::filesystem::exists(folder)) return;
    
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file() || entry.is_symlink()) {
            auto path = entry.path();
            auto ext = path.extension().string();
            
            for (char& c : ext) c = std::tolower(c);
            
            if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || 
                ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".flv") {
                songs.push_back(std::make_shared<Song>(
                    path.filename().string(), folder));
            }
        }
    }
}

void SongCollection::clear() {
    songs.clear();
    currentIndex = -1;
}

std::shared_ptr<Song> SongCollection::getSong(int index) const {
    if (index >= 0 && index < (int)songs.size()) {
        return songs[index];
    }
    return nullptr;
}

std::shared_ptr<Song> SongCollection::getCurrentSong() const {
    return getSong(currentIndex);
}

std::vector<std::string> SongCollection::getSongNames() const {
    std::vector<std::string> names;
    for (const auto& song : songs) {
        names.push_back(song->getFilename());
    }
    return names;
}