#include "MetadataController.h"

MetadataController::MetadataController(int h, int w, int y, int x)
    : metadataView(std::make_unique<MetadataView>(h, w, y, x)),
      sourceMediaFile(nullptr),
      isEditing(false),
      editBuffer(""){}

void MetadataController::setMetadata(MediaMetadata* meta) { 
    metadata = nullptr;
    if (!meta) {
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
void MetadataController::startEditing(EditField field) {
    metadataView->setCurrentEditField(field);
    isEditing = true;
    editBuffer = getFieldValue(field);

    metadataView->setIsEditing(true);
    metadataView->setEditBuffer(editBuffer);

    draw();
}
void MetadataController::finishEditing(bool save) {
    if (save && !editBuffer.empty()) {
        setFieldValue(metadataView->getCurrentEditField(), editBuffer);
    }
    isEditing = false;
    metadataView->setCurrentEditField(EditField::NONE);
    editBuffer.clear();

    metadataView->setIsEditing(false);
    metadataView->setEditBuffer("");

    draw();
}
std::string MetadataController::getFieldValue(EditField field) {
    if (!metadata) return "";
    
    auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get());
    auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get());
    
    if (!audioMeta && !videoMeta) return "";
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
void MetadataController::setFieldValue(EditField field, const std::string& value) {
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
EditField MetadataController::getFieldAtY(int y) {
    for (const auto& [field, fieldY] : metadataView->getFieldPositions()) {
        if (fieldY == y) {
            return field;
        }
    }
    return EditField::NONE;
}

void MetadataController::draw() {
    MetadataDisplayData displayData;

    if (metadata) {
        if (auto* audioMeta = dynamic_cast<AudioMetadata*>(metadata.get())) {
            displayData.isAudio = true;
            displayData.title = audioMeta->getTitle();
            displayData.artist = audioMeta->getArtist();
            displayData.album = audioMeta->getAlbum();
            displayData.genre = audioMeta->getGenre();
            displayData.year = std::to_string(audioMeta->getYear());
            displayData.duration = audioMeta->getDuration();
            displayData.bitrate = audioMeta->getBitrate();
            displayData.sampleRate = audioMeta->getSampleRate();
        }
        else if (auto* videoMeta = dynamic_cast<VideoMetadata*>(metadata.get())) {
            displayData.isVideo = true;
            displayData.title = videoMeta->getTitle();
            displayData.codec = videoMeta->getCodec();
            displayData.duration = videoMeta->getDuration();
            displayData.bitrate = videoMeta->getBitrate();
            displayData.size = videoMeta->getSize();
        }
    }
    metadataView->setIsEditing(isEditing);
    metadataView->setEditBuffer(editBuffer);
    metadataView->draw(displayData);
}

bool MetadataController::handleClick(int x, int y) {
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

bool MetadataController::handleKey(int ch) {
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
                if (!metadataView->getFieldPositions().empty()) {
                    auto it = std::find_if(metadataView->getFieldPositions().begin(), metadataView->getFieldPositions().end(),
                        [this](const auto& p) { return p.first == metadataView->getCurrentEditField(); });
                    
                    if (it != metadataView->getFieldPositions().end() && ++it != metadataView->getFieldPositions().end()) {
                        metadataView->setCurrentEditField(it->first);
                    } else {
                        metadataView->setCurrentEditField(metadataView->getFieldPositions()[0].first);
                    }
                    draw();
                }
                return true;
            case KEY_UP:
                if (!metadataView->getFieldPositions().empty()) {
                    auto it = std::find_if(metadataView->getFieldPositions().begin(), metadataView->getFieldPositions().end(),
                        [this](const auto& p) { return p.first == metadataView->getCurrentEditField(); });
                    
                    if (it != metadataView->getFieldPositions().begin()) {
                        metadataView->setCurrentEditField((--it)->first);
                    } else {
                        metadataView->setCurrentEditField(metadataView->getFieldPositions().back().first);
                    }
                    draw();
                }
                return true;
            case '\n':
                if (metadataView->getCurrentEditField() != EditField::NONE) {
                    startEditing(metadataView->getCurrentEditField());
                }
                return true;
        }
    }
    return false;
}

bool MetadataController::saveToFile() {
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
