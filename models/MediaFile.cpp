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
bool MediaFile::saveMetadata() {
    if (!metadataLoaded || !metadata) {
        return false;
    }
    
    // Chỉ lưu metadata cho audio files (sử dụng TagLib)
    if (!isVideo()) {
        TagLib::FileRef f(fullPath.c_str());
        
        if (f.isNull() || !f.tag()) {
            return false;
        }
        
        auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get());
        if (!audioMeta) {
            return false;
        }
        
        TagLib::Tag* tag = f.tag();
        
        // Lưu các trường metadata
        std::string title = audioMeta->getTitle();
        std::string artist = audioMeta->getArtist();
        std::string album = audioMeta->getAlbum();
        std::string genre = audioMeta->getGenre();
        unsigned int year = audioMeta->getYear();
        
        tag->setTitle(TagLib::String(title, TagLib::String::UTF8));
        tag->setArtist(TagLib::String(artist, TagLib::String::UTF8));
        tag->setAlbum(TagLib::String(album, TagLib::String::UTF8));
        tag->setGenre(TagLib::String(genre, TagLib::String::UTF8));
        tag->setYear(year);
        
        // Lưu file và kiểm tra kết quả
        bool success = f.save();
        
        if (success) {
            // Reload metadata để đảm bảo đồng bộ
            metadataLoaded = false;
            loadMetadata();
        }
        
        return success;
    }
    
    // Video metadata không được lưu (cần thư viện khác như FFmpeg)
    // Có thể implement sau nếu cần
    return false;
}