#include "MediaFileListController.h"

MediaFileListController::MediaFileListController(int height, int width, int startY, int startX) 
    : currentCollection(nullptr) {
    view = std::make_unique<MediaFileListView>(height, width, startY, startX);
}

void MediaFileListController::setCollection(MediaManager* collection) {
    currentCollection = collection;
    if (currentCollection) {
        view->setMediaFile(currentCollection->getMediaFileNames(), true);
    }
}

void MediaFileListController::handleClick(int x, int y, int button) {
    // Kiểm tra click vào nút pagination
    if (view->isPrevButtonClicked(x, y)) {
        view->prevPage();
        return;
    }
    if (view->isNextButtonClicked(x, y)) {
        view->nextPage();
        return;
    }
    
    // Click vào bài hát
    int mediafileIndex = view->getSongAtY(y);
    if (mediafileIndex < 0) return;
    
    if (button == 3) { // Right click
        if (onMediaFileRightClick) {
            onMediaFileRightClick(mediafileIndex);
        }
    } else if (button == 1) { // Left click
        if (onMediaFileClick) {
            onMediaFileClick(mediafileIndex);
        }
    }
}

void MediaFileListController::handleKeyUp() {
    view->scrollUp();
}

void MediaFileListController::handleKeyDown() {
    view->scrollDown();
}

void MediaFileListController::handlePageUp() {
    view->prevPage();
}

void MediaFileListController::handlePageDown() {
    view->nextPage();
}

void MediaFileListController::handleKeyLeft() {
    view->prevPage();
}

void MediaFileListController::handleKeyRight() {
    view->nextPage();
}

void MediaFileListController::handleScrollUp() {
    view->scrollUp();
}

void MediaFileListController::handleScrollDown() {
    view->scrollDown();
}

void MediaFileListController::setOnMediaFileClick(std::function<void(int)> callback) {
    onMediaFileClick = callback;
}

void MediaFileListController::setOnMediaFileRightClick(std::function<void(int)> callback) {
    onMediaFileRightClick = callback;
}

void MediaFileListController::setTitle(const std::string& title) {
    view->setTitle(title);
}

void MediaFileListController::updateView() {
    if (currentCollection) {
        view->setMediaFile(currentCollection->getMediaFileNames(), false);
        view->setCurrentPlayingIndex(currentCollection->getCurrentIndex());
    }
}

void MediaFileListController::draw() {
    view->draw();
}

void MediaFileListController::resize(int height, int width, int startY, int startX) {
    view = std::make_unique<MediaFileListView>(height, width, startY, startX);
}