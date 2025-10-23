#ifndef SONG_H
#define SONG_H

#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

struct SongMetadata {
    std::string title;
    std::string artist;
    std::string album;
    int year;
    int duration;
    int bitrate;
    int sampleRate;
};

class Song {
private:
    std::string filename;
    std::string fullPath;
    SongMetadata metadata;
    bool metadataLoaded;

public:
    Song(const std::string& filename, const std::string& folder);
    
    const std::string& getFilename() const { return filename; }
    const std::string& getFullPath() const { return fullPath; }
    
    SongMetadata getMetadata();
    int getDuration();
    bool isVideo() const;
    
private:
    void loadMetadata();
};

#endif