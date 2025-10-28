#include "PlaylistController.h"

PlaylistController::PlaylistController(int height, int width, int startY, int startX)
    : playlistManager(nullptr) {
    view = std::make_unique<PlaylistView>(height, width, startY, startX);
}

void PlaylistController::setPlaylistManager(PlaylistManager* manager) {
    playlistManager = manager;
}

void PlaylistController::handleClick(int x, int y, int button) {
    // Kiểm tra click vào các button
    if (view->isAddButtonClicked(x, y)) {
        if (onAddClick) onAddClick();
        return;
    }
    
    if (view->isRemoveButtonClicked(x, y)) {
        if (onRemoveClick) onRemoveClick();
        return;
    }
    
    if (view->isEditButtonClicked(x, y)) {
        if (onEditClick) onEditClick();
        return;
    }
    
    // Kiểm tra click vào playlist
    int playlistIndex = view->getPlaylistAtY(y);
    if (playlistIndex >= 0) {
        if (button == 1) { // Single click
            if (playlistManager) {
                playlistManager->setSelectedIndex(playlistIndex);
                view->setSelectedIndex(playlistIndex);
            }
            if (onPlaylistSelect) {
                onPlaylistSelect(playlistIndex);
            }
        } else if (button == 2) { // Double click
            if (playlistManager) {
                playlistManager->setSelectedIndex(playlistIndex);
            }
            if (onPlaylistDoubleClick) {
                onPlaylistDoubleClick(playlistIndex);
            }
        }
    }
}

void PlaylistController::setOnPlaylistSelect(std::function<void(int)> callback) {
    onPlaylistSelect = callback;
}

void PlaylistController::setOnPlaylistDoubleClick(std::function<void(int)> callback) {
    onPlaylistDoubleClick = callback;
}

void PlaylistController::setOnAddClick(std::function<void()> callback) {
    onAddClick = callback;
}

void PlaylistController::setOnRemoveClick(std::function<void()> callback) {
    onRemoveClick = callback;
}

void PlaylistController::setOnEditClick(std::function<void()> callback) {
    onEditClick = callback;
}

void PlaylistController::updateView() {
    if (playlistManager) {
        view->setPlaylists(playlistManager->getPlaylistNames());
        view->setSelectedIndex(playlistManager->getSelectedIndex());
    }
}

void PlaylistController::draw() {
    view->draw();
}

void PlaylistController::resize(int height, int width, int startY, int startX) {
    view = std::make_unique<PlaylistView>(height, width, startY, startX);
}   