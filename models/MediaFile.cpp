#include "MediaFile.h"

MediaFile::MediaFile(const std::string& filename, const std::string& folder) 
    : filename(filename), fullPath(folder + "/" + filename), metadataLoaded(false) {}

void MediaFile::loadMetadata() {
    if (metadataLoaded) return;
    
    TagLib::FileRef f(fullPath.c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag* tag = f.tag();
        metadata.title = tag->title().toCString(true);
        metadata.artist = tag->artist().toCString(true);
        metadata.album = tag->album().toCString(true);
        metadata.year = tag->year();
        
        if (f.audioProperties()) {
            metadata.duration = f.audioProperties()->length();
            metadata.bitrate = f.audioProperties()->bitrate();
            metadata.sampleRate = f.audioProperties()->sampleRate();
        }
    }
    metadataLoaded = true;
}

MediaMetadata MediaFile::getMediaMetadata() {
    loadMetadata();
    return metadata;
}

int MediaFile::getDuration() {
    if (isVideo()) return 180;
    loadMetadata();
    return metadata.duration;
}

bool MediaFile::isVideo() const {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return false;
    
    std::string ext = filename.substr(pos);
    for (char& c : ext) c = std::tolower(c);
    
    return (ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".flv");
}