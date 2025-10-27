#include "MediaFile.h"

MediaFile::MediaFile(const std::string& filename, const std::string& folder) 
    : filename(filename), fullPath(folder + "/" + filename), metadataLoaded(false) {}

void MediaFile::loadMetadata() {
    if (metadataLoaded) return;
    
    if (isVideo()) {
        auto videoMeta = std::make_unique<VideoMetadata>();
        // Khởi tạo các giá trị cơ bản
        videoMeta->setTitle(filename);
        videoMeta->setDuration(180);
        videoMeta->setBitrate(1200);
        // Thêm các thuộc tính riêng của video
        videoMeta->setCodec("H.264");
        videoMeta->setSize(1024 * 1024 * 50);
        metadata = std::move(videoMeta);
    } else {
        auto audioMeta = std::make_unique<AudioMetadata>();
        TagLib::FileRef f(fullPath.c_str());
        
        if (!f.isNull() && f.tag()) {
            TagLib::Tag* tag = f.tag();
            // Thiết lập các thuộc tính cơ bản
            audioMeta->setTitle(tag->title().toCString(true));
            audioMeta->setBitrate(0); // Sẽ được cập nhật nếu có audioProperties
            audioMeta->setDuration(0); // Sẽ được cập nhật nếu có audioProperties
            
            // Thiết lập các thuộc tính riêng của audio
            audioMeta->setArtist(tag->artist().toCString(true));
            audioMeta->setAlbum(tag->album().toCString(true));
            audioMeta->setGenre(tag->genre().toCString(true));
            audioMeta->setYear(tag->year());

            if (f.audioProperties()) {
                auto props = f.audioProperties();
                audioMeta->setDuration(props->length());
                audioMeta->setBitrate(props->bitrate());
                audioMeta->setSampleRate(props->sampleRate());
            }
        }
        metadata = std::move(audioMeta);
    }
    metadataLoaded = true;
}

MediaMetadata* MediaFile::getMediaMetadata() {
    if (!metadataLoaded) {
        loadMetadata();
    }
    return metadata.get();
}

int MediaFile::getDuration() {
    if (!metadataLoaded) {
        loadMetadata();
    }
    return metadata->getDuration();
}

bool MediaFile::isVideo() const {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return false;
    
    std::string ext = filename.substr(pos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    static const std::set<std::string> videoExtensions = {
        ".mp4", ".avi", ".mkv", ".flv"
    };
    
    return videoExtensions.find(ext) != videoExtensions.end();
}