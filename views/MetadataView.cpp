#include "MetadataView.h"

MetadataView::MetadataView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x), metadata(nullptr) {}  

void MetadataView::setMetadata(MediaMetadata* meta) { 
    if (!meta) {
        metadata = nullptr;
        return;
    }
    
    if (auto* audioMeta = dynamic_cast<AudioMetadata*>(meta)) {
        auto newMeta = std::make_unique<AudioMetadata>();
        newMeta->setTitle(audioMeta->getTitle());
        newMeta->setDuration(audioMeta->getDuration());
        newMeta->setBitrate(audioMeta->getBitrate());
        newMeta->setArtist(audioMeta->getArtist());
        newMeta->setAlbum(audioMeta->getAlbum());
        newMeta->setGenre(audioMeta->getGenre());
        newMeta->setYear(audioMeta->getYear());
        newMeta->setSampleRate(audioMeta->getSampleRate());
        metadata = std::move(newMeta);
    } 
    else if (auto* videoMeta = dynamic_cast<VideoMetadata*>(meta)) {
        auto newMeta = std::make_unique<VideoMetadata>();
        newMeta->setTitle(videoMeta->getTitle());
        newMeta->setDuration(videoMeta->getDuration());
        newMeta->setBitrate(videoMeta->getBitrate());
        newMeta->setCodec(videoMeta->getCodec());
        newMeta->setSize(videoMeta->getSize());
        metadata = std::move(newMeta);
    }
}

void MetadataView::draw() {
    werase(window);
    box(window, 0, 0);
    
    mvwprintw(window, 1, 2, "Metadata for: %s", filename.c_str());
    
    if (!metadata) {  // THÊM: kiểm tra nullptr
        mvwprintw(window, 3, 4, "No metadata available");
        mvwprintw(window, height - 2, 2, "Press ESC to return.");
        wrefresh(window);
        return;
    }
    
    int line = 3;
    mvwprintw(window, line++, 4, "Title:  %s", metadata->getTitle().c_str());  
    
    // Kiểm tra xem có phải AudioMetadata không
    if (auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get())) {
        mvwprintw(window, line++, 4, "Artist: %s", audioMeta->getArtist().c_str());
        mvwprintw(window, line++, 4, "Album:  %s", audioMeta->getAlbum().c_str());
        mvwprintw(window, line++, 4, "Year:   %d", audioMeta->getYear());
        mvwprintw(window, line++, 4, "Length: %d sec", audioMeta->getDuration());
        mvwprintw(window, line++, 4, "Bitrate: %d kbps", audioMeta->getBitrate());
        mvwprintw(window, line++, 4, "Sample rate: %d Hz", audioMeta->getSampleRate());
    }
    // Kiểm tra xem có phải VideoMetadata không
    else if (auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get())) {
        mvwprintw(window, line++, 4, "Codec:  %s", videoMeta->getCodec().c_str());
        mvwprintw(window, line++, 4, "Length: %d sec", videoMeta->getDuration());
        mvwprintw(window, line++, 4, "Bitrate: %d kbps", videoMeta->getBitrate());
        mvwprintw(window, line++, 4, "Size: %ld bytes", videoMeta->getSize());
    }
    
    mvwprintw(window, height - 2, 2, "Press ESC to return.");
    
    wrefresh(window);
}