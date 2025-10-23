#include "Playlist.h"

Playlist::Playlist(const std::string& name, const std::string& basePath) 
    : name(name), path(basePath + "/" + name), songs(path) {}

void Playlist::create(const std::vector<std::string>& songFiles, 
                     const std::string& sourceFolder) {
    if (std::filesystem::exists(path)) {
        std::filesystem::remove_all(path);
    }
    
    std::filesystem::create_directory(path);
    
    for (const auto& songFile : songFiles) {
        std::string sourcePath = sourceFolder + "/" + songFile;
        std::string destPath = path + "/" + songFile;
        
        try {
            std::filesystem::copy_file(sourcePath, destPath, 
                std::filesystem::copy_options::overwrite_existing);
        } catch (...) {
            try {
                std::filesystem::create_symlink(
                    std::filesystem::absolute(sourcePath), destPath);
            } catch (...) {}
        }
    }
}

void Playlist::load() {
    songs.load();
}

void Playlist::remove() {
    if (std::filesystem::exists(path)) {
        std::filesystem::remove_all(path);
    }
}