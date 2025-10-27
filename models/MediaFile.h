#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <string>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <memory>
#include <set>
#include <algorithm> 

class MediaMetadata {
protected:
    std::string title;
    int duration;
    int bitrate;

public:
    virtual ~MediaMetadata() = default;
    
    virtual std::string getTitle() const { return title; }
    virtual int getDuration() const { return duration; }
    virtual int getBitrate() const { return bitrate; }

    // Setters can be added as needed
    void setTitle(const std::string& t) { title = t; }
    void setDuration(int d) { duration = d; }
    void setBitrate(int b) { bitrate = b; }
};

class AudioMetadata : public MediaMetadata {
private:
    std::string artist;
    std::string album;
    std::string genre;
    int year;
    int sampleRate;

public:
    // Audio specific methods
    std::string getArtist() const { return artist; }
    std::string getAlbum() const { return album; }
    std::string getGenre() const { return genre; }
    int getYear() const { return year; }
    int getSampleRate() const { return sampleRate; }

    // Setters can be added as needed
    void setArtist(const std::string& a) { artist = a; }
    void setAlbum(const std::string& a) { album = a; }
    void setGenre(const std::string& g) { genre = g; }
    void setYear(int y) { year = y; }
    void setSampleRate(int sr) { sampleRate = sr; }
};

class VideoMetadata : public MediaMetadata {
private:
    std::string codec;
    long size;

public:
    std::string getCodec() const { return codec; }
    long getSize() const { return size; }

    void setCodec(const std::string& c) { codec = c; }
    void setSize(long s) { size = s; }
};

class MediaFile {
private:
    std::string filename;
    std::string fullPath;
    std::unique_ptr<MediaMetadata> metadata;
    bool metadataLoaded;

public:
    MediaFile(const std::string& filename, const std::string& folder);
    
    const std::string& getFilename() const { return filename; }
    const std::string& getFullPath() const { return fullPath; }
    
    MediaMetadata* getMediaMetadata();
    int getDuration();
    bool isVideo() const;
    bool saveMetadata();
    
private:
    void loadMetadata();
};

#endif