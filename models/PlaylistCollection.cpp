#include "PlaylistCollection.h"

PlaylistCollection::PlaylistCollection() 
    : baseFolder("./playlist"), selectedIndex(-1) {
    if (!std::filesystem::exists(baseFolder)) {
        std::filesystem::create_directory(baseFolder);
    }
}

void PlaylistCollection::load() {
    playlists.clear();
    for (const auto& entry : std::filesystem::directory_iterator(baseFolder)) {
        if (entry.is_directory()) {
            auto pl = std::make_shared<Playlist>(
                entry.path().filename().string(), baseFolder);
            playlists.push_back(pl);
        }
    }
}

void PlaylistCollection::createPlaylist(const std::string& name, 
                                       const std::vector<SelectedSongInfo>& songs) {
    std::string safeName = name;
    for (char& c : safeName) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || 
            c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    
    // Tạo thư mục playlist
    std::string playlistPath = baseFolder + "/" + safeName;
    if (!std::filesystem::exists(playlistPath)) {
        std::filesystem::create_directories(playlistPath);
    }
    
    // Copy files từ PC và USB
    for (const auto& songInfo : songs) {
        std::string sourceFolder = songInfo.isFromPC ? "./music" : "/music/usb";
        std::string sourcePath = sourceFolder + "/" + songInfo.name;
        std::string destPath = playlistPath + "/" + songInfo.name;
        
        try {
            if (std::filesystem::exists(sourcePath)) {
                std::filesystem::copy_file(sourcePath, destPath, 
                    std::filesystem::copy_options::overwrite_existing);
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to copy: " << songInfo.name << std::endl;
        }
    }
    
    load();
}

void PlaylistCollection::deletePlaylist(int index) {
    if (index >= 0 && index < (int)playlists.size()) {
        playlists[index]->remove();
        load();
    }
}

std::shared_ptr<Playlist> PlaylistCollection::getPlaylist(int index) const {
    if (index >= 0 && index < (int)playlists.size()) {
        return playlists[index];
    }
    return nullptr;
}

std::shared_ptr<Playlist> PlaylistCollection::getSelectedPlaylist() const {
    return getPlaylist(selectedIndex);
}

std::vector<std::string> PlaylistCollection::getPlaylistNames() const {
    std::vector<std::string> names;
    for (const auto& pl : playlists) {
        names.push_back(pl->getName());
    }
    return names;
}
void PlaylistCollection::updatePlaylist(int index, const std::string& name, 
                                       const std::vector<SelectedSongInfo>& songs) {
    if (index < 0 || index >= (int)playlists.size()) return;
    
    auto playlist = playlists[index];
    std::string oldName = playlist->getName();
    
    // Sanitize tên
    std::string safeName = name;
    for (char& c : safeName) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || 
            c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    
    std::string oldFolder = baseFolder + "/" + oldName;
    std::string newFolder = baseFolder + "/" + safeName;
    
    // Rename folder nếu đổi tên
    if (safeName != oldName) {
        if (std::filesystem::exists(oldFolder)) {
            try {
                std::filesystem::rename(oldFolder, newFolder);
            } catch (const std::exception& e) {
                std::cerr << "Rename error: " << e.what() << std::endl;
                return;
            }
        }
    } else {
        newFolder = oldFolder;
    }
    
    // Xóa files cũ
    if (std::filesystem::exists(newFolder)) {
        for (const auto& entry : std::filesystem::directory_iterator(newFolder)) {
            std::filesystem::remove(entry.path());
        }
    } else {
        std::filesystem::create_directories(newFolder);
    }
    
    // Copy files mới
    for (const auto& songInfo : songs) {
        std::string sourceFolder = songInfo.isFromPC ? "./music" : "/music/usb";
        std::string sourcePath = sourceFolder + "/" + songInfo.name;
        std::string destPath = newFolder + "/" + songInfo.name;
        
        if (std::filesystem::exists(sourcePath)) {
            try {
                std::filesystem::copy_file(sourcePath, destPath, 
                    std::filesystem::copy_options::overwrite_existing);
            } catch (const std::exception& e) {
                std::cerr << "Copy error: " << e.what() << std::endl;
            }
        }
    }
    
    // QUAN TRỌNG: Update tên và reload
    playlist->setName(safeName);
    playlist->load();  // Load lại songs từ folder mới
    
    std::cerr << "Playlist updated and reloaded" << std::endl;
}