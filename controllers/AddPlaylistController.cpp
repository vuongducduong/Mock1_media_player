#include "AddPlaylistController.h"

AddPlaylistController::AddPlaylistController(int height, int width, int startY, int startX) {
    view = std::make_unique<AddPlaylistView>(height, width, startY, startX);
}

void AddPlaylistController::handleClick(int x, int y) {
    // Click vào ô nhập tên
    if (view->isNameInputClicked(x, y)) {
        view->editPlaylistName();
        return;
    }
    
    // Click vào tab PC
    if (view->isPCTabClicked(x, y)) {
        view->switchToPC();
        return;
    }
    
    // Click vào tab USB
    if (view->isUSBTabClicked(x, y)) {
        view->switchToUSB();
        return;
    }
    
    // Click vào bài hát
    int mediafileIndex = view->getSongAtY(y);
    if (mediafileIndex >= 0) {
        view->toggleSong(mediafileIndex);
        return;
    }
    
    // Click nút Save
    if (view->isSaveButtonClicked(x, y)) {
        if (getSelectedCount() > 0 && !getPlaylistName().empty()) {
            if (onSave) onSave();
        }
        return;
    }
    
    // Click nút Cancel
    if (view->isCancelButtonClicked(x, y)) {
        if (onCancel) onCancel();
        return;
    }
}

void AddPlaylistController::handleScrollUp() {
    view->scrollUp();
}

void AddPlaylistController::handleScrollDown() {
    view->scrollDown();
}

void AddPlaylistController::setOnSave(std::function<void()> callback) {
    onSave = callback;
}

void AddPlaylistController::setOnCancel(std::function<void()> callback) {
    onCancel = callback;
}

void AddPlaylistController::setupForCreate(MediaManager* pcMedia, MediaManager* usbMedia) {
    view->setMode(AddPlaylistMode::CREATE);
    view->reset();
    view->setAvailableMediaFilesPC(pcMedia->getMediaFileNames());
    view->setAvailableMediaFilesUSB(usbMedia->getMediaFileNames());
}

void AddPlaylistController::setupForEdit(int playlistIndex, Playlist* playlist,
                                         MediaManager* pcMedia, MediaManager* usbMedia) {
    playlist->load();
    
    view->setMode(AddPlaylistMode::EDIT);
    view->setEditingPlaylistIndex(playlistIndex);
    view->setPlaylistName(playlist->getPlayListName());
    
    // Load danh sách available songs
    view->setAvailableMediaFilesPC(pcMedia->getMediaFileNames());
    view->setAvailableMediaFilesUSB(usbMedia->getMediaFileNames());
    
    std::vector<SelectedMediaFileInfo> selectedMediaFiles;
    auto& playlistMediaFiles = playlist->getMediaFiles();
    
    auto pcList = pcMedia->getMediaFileNames();
    auto usbList = usbMedia->getMediaFileNames();
    
    for (const auto& mediafile : playlistMediaFiles.getAllMediaFiles()) {
        SelectedMediaFileInfo info;
        info.name = mediafile->getFilename();
        
        // Tìm trong PC trước
        bool foundInPC = false;
        for (const auto& pcMediaFile : pcList) {
            if (pcMediaFile == info.name) {
                info.isFromPC = true;
                foundInPC = true;
                break;
            }
        }
        
        // Nếu không có trong PC, tìm trong USB
        if (!foundInPC) {
            for (const auto& usbMediaFile : usbList) {
                if (usbMediaFile == info.name) {
                    info.isFromPC = false;
                    break;
                }
            }
        }
        
        selectedMediaFiles.push_back(info);
    }
    
    view->setSelectedMediaFilesWithSource(selectedMediaFiles);
}

std::string AddPlaylistController::getPlaylistName() const {
    return view->getPlaylistName();
}

std::vector<SelectedMediaFileInfo> AddPlaylistController::getSelectedMediaFiles() const {
    return view->getSelectedMediaFilesWithSource();
}

int AddPlaylistController::getSelectedCount() const {
    return view->getSelectedCount();
}

AddPlaylistMode AddPlaylistController::getMode() const {
    return view->getMode();
}

int AddPlaylistController::getEditingPlaylistIndex() const {
    return view->getEditingPlaylistIndex();
}

void AddPlaylistController::reset() {
    view->reset();
}

void AddPlaylistController::draw() {
    view->draw();
}

void AddPlaylistController::resize(int height, int width, int startY, int startX) {
    view = std::make_unique<AddPlaylistView>(height, width, startY, startX);
}