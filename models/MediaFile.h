#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

struct MediaMetadata {
    std::string title;
    std::string artist;
    std::string album;
    int year;
    int duration;
    int bitrate;
    int sampleRate;
};

class MediaFile {
private:
    std::string filename;
    std::string fullPath;
    MediaMetadata metadata;
    bool metadataLoaded;

public:
    MediaFile(const std::string& filename, const std::string& folder);
    
    const std::string& getFilename() const { return filename; }
    const std::string& getFullPath() const { return fullPath; }
    
    MediaMetadata getMediaMetadata();
    int getDuration();
    bool isVideo() const;
    
private:
    void loadMetadata();
};

#endif