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
void MetadataView::drawField(int line, const char* label, const std::string& value, 
                              bool isEditable, bool isSelected) {
    if (isSelected && isEditing) {
        wattron(window, A_REVERSE);
        mvwprintw(window, line, 4, "%-12s [%s_]", label, editBuffer.c_str());
        wattroff(window, A_REVERSE);
    } else if (isEditable) {
        if (isSelected) {
            wattron(window, A_BOLD | A_UNDERLINE);
        }
        mvwprintw(window, line, 4, "%-12s %s", label, value.c_str());
        if (isSelected) {
            wattroff(window, A_BOLD | A_UNDERLINE);
        }
        mvwprintw(window, line, width - 10, "[EDIT]");
    } else {
        mvwprintw(window, line, 4, "%-12s %s", label, value.c_str());
    }
}
void MetadataView::draw() {
    werase(window);
    box(window, 0, 0);
    
    mvwprintw(window, 1, 2, "Metadata for: %s", filename.c_str());
    
    if (!metadata) {
        mvwprintw(window, 3, 4, "No metadata available");
        mvwprintw(window, height - 2, 2, "Press ESC to return.");
        wrefresh(window);
        return;
    }
    
    fieldPositions.clear();
    int line = 3;
    
    fieldPositions.push_back({EditField::TITLE, line});
    drawField(line++, "Title:", metadata->getTitle(), true, 
              currentEditField == EditField::TITLE);
    
    if (auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get())) {
        fieldPositions.push_back({EditField::ARTIST, line});
        drawField(line++, "Artist:", audioMeta->getArtist(), true,
                  currentEditField == EditField::ARTIST);
        
        fieldPositions.push_back({EditField::ALBUM, line});
        drawField(line++, "Album:", audioMeta->getAlbum(), true,
                  currentEditField == EditField::ALBUM);
        
        fieldPositions.push_back({EditField::GENRE, line});
        drawField(line++, "Genre:", audioMeta->getGenre(), true,
                  currentEditField == EditField::GENRE);
        
        fieldPositions.push_back({EditField::YEAR, line});
        drawField(line++, "Year:", std::to_string(audioMeta->getYear()), true,
                  currentEditField == EditField::YEAR);
        
        mvwprintw(window, line++, 4, "Length:      %d sec", audioMeta->getDuration());
        mvwprintw(window, line++, 4, "Bitrate:     %d kbps", audioMeta->getBitrate());
        mvwprintw(window, line++, 4, "Sample rate: %d Hz", audioMeta->getSampleRate());
    }
    else if (auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get())) {
        fieldPositions.push_back({EditField::CODEC, line});
        drawField(line++, "Codec:", videoMeta->getCodec(), true,
                  currentEditField == EditField::CODEC);
        
        mvwprintw(window, line++, 4, "Length:      %d sec", videoMeta->getDuration());
        mvwprintw(window, line++, 4, "Bitrate:     %d kbps", videoMeta->getBitrate());
        mvwprintw(window, line++, 4, "Size:        %ld bytes", videoMeta->getSize());
    }
    
    if (isEditing) {
        mvwprintw(window, height - 3, 2, "Editing: Type to modify, ENTER to save, ESC to cancel");
    } else {
        mvwprintw(window, height - 3, 2, "Click on highlighted fields to edit, or press TAB to cycle");
    }
    mvwprintw(window, height - 2, 2, "Press ESC to return | Changes are saved automatically");  
    
    wrefresh(window);
}
bool MetadataView::saveToFile() {
    if (!sourceMediaFile || !metadata) {
        return false;
    }
    
    // Lấy metadata gốc từ sourceMediaFile
    MediaMetadata* originalMetadata = sourceMediaFile->getMediaMetadata();
    if (!originalMetadata) {
        return false;
    }
    
    // Copy dữ liệu từ metadata đã chỉnh sửa sang metadata gốc
    if (auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get())) {
        if (auto* origAudioMeta = dynamic_cast<AudioMetadata*>(originalMetadata)) {
            origAudioMeta->setTitle(audioMeta->getTitle());
            origAudioMeta->setArtist(audioMeta->getArtist());
            origAudioMeta->setAlbum(audioMeta->getAlbum());
            origAudioMeta->setGenre(audioMeta->getGenre());
            origAudioMeta->setYear(audioMeta->getYear());
            // Gọi saveMetadata của MediaFile để lưu vào file thật
            return sourceMediaFile->saveMetadata();
        }
    }
    else if (auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get())) {
        if (auto* origVideoMeta = dynamic_cast<VideoMetadata*>(originalMetadata)) {
            origVideoMeta->setTitle(videoMeta->getTitle());
            origVideoMeta->setCodec(videoMeta->getCodec());
            return sourceMediaFile->saveMetadata();
        }
    }
    
    return false;
}
bool MetadataView::handleClick(int x, int y) {
    if (isEditing) {
        return false;
    }
    
    EditField clickedField = getFieldAtY(y);
    if (clickedField != EditField::NONE) {
        startEditing(clickedField);
        return true;
    }
    return false;
}

EditField MetadataView::getFieldAtY(int y) {
    for (const auto& [field, fieldY] : fieldPositions) {
        if (fieldY == y) {
            return field;
        }
    }
    return EditField::NONE;
}

void MetadataView::startEditing(EditField field) {
    currentEditField = field;
    isEditing = true;
    editBuffer = getFieldValue(field);
    draw();
}

void MetadataView::finishEditing(bool save) {
    if (save && !editBuffer.empty()) {
        setFieldValue(currentEditField, editBuffer);
    }
    isEditing = false;
    currentEditField = EditField::NONE;
    editBuffer.clear();
    draw();
}

std::string MetadataView::getFieldValue(EditField field) {
    if (!metadata) return "";
    
    auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get());
    auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get());
    
    switch (field) {
        case EditField::TITLE:
            return metadata->getTitle();
        case EditField::ARTIST:
            return audioMeta ? audioMeta->getArtist() : "";
        case EditField::ALBUM:
            return audioMeta ? audioMeta->getAlbum() : "";
        case EditField::GENRE:
            return audioMeta ? audioMeta->getGenre() : "";
        case EditField::YEAR:
            return audioMeta ? std::to_string(audioMeta->getYear()) : "";
        case EditField::CODEC:
            return videoMeta ? videoMeta->getCodec() : "";
        default:
            return "";
    }
}

void MetadataView::setFieldValue(EditField field, const std::string& value) {
    if (!metadata) return;
    
    auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get());
    auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get());
    
    switch (field) {
        case EditField::TITLE:
            metadata->setTitle(value);
            break;
        case EditField::ARTIST:
            if (audioMeta) audioMeta->setArtist(value);
            break;
        case EditField::ALBUM:
            if (audioMeta) audioMeta->setAlbum(value);
            break;
        case EditField::GENRE:
            if (audioMeta) audioMeta->setGenre(value);
            break;
        case EditField::YEAR:
            if (audioMeta) {
                try {
                    audioMeta->setYear(std::stoi(value));
                } catch (...) {}
            }
            break;
        case EditField::CODEC:
            if (videoMeta) videoMeta->setCodec(value);
            break;
        default:
            break;
    }
}

bool MetadataView::handleKey(int ch) {
    if (isEditing) {
        switch (ch) {
            case '\n':
                finishEditing(true);
                saveToFile();
                return true;
            case 27:
                finishEditing(false);
                return true;
            case KEY_BACKSPACE:
            case 127:
            case '\b':
                if (!editBuffer.empty()) {
                    editBuffer.pop_back();
                    draw();
                }
                return true;
            default:
                if (ch >= 32 && ch < 127) {
                    editBuffer += (char)ch;
                    draw();
                }
                return true;
        }
    } else {
        switch (ch) {
            case '\t':
            case KEY_DOWN:
                if (!fieldPositions.empty()) {
                    auto it = std::find_if(fieldPositions.begin(), fieldPositions.end(),
                        [this](const auto& p) { return p.first == currentEditField; });
                    
                    if (it != fieldPositions.end() && ++it != fieldPositions.end()) {
                        currentEditField = it->first;
                    } else {
                        currentEditField = fieldPositions[0].first;
                    }
                    draw();
                }
                return true;
            case KEY_UP:
                if (!fieldPositions.empty()) {
                    auto it = std::find_if(fieldPositions.begin(), fieldPositions.end(),
                        [this](const auto& p) { return p.first == currentEditField; });
                    
                    if (it != fieldPositions.begin()) {
                        currentEditField = (--it)->first;
                    } else {
                        currentEditField = fieldPositions.back().first;
                    }
                    draw();
                }
                return true;
            case '\n':
                if (currentEditField != EditField::NONE) {
                    startEditing(currentEditField);
                }
                return true;
        }
    }
    return false;
}