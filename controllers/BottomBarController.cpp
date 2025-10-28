#include "BottomBarController.h"

BottomBarController::BottomBarController(int termWidth, int startY) {
    view = std::make_unique<BottomBarView>(termWidth, startY);
}

void BottomBarController::handleClick(int x, int y) {
    int localY = y;
    
    // Chỉ xử lý click ở dòng control (dòng thứ 2 của bottom bar)
    if (localY != 2) return;
    
    int ctrlIndex = view->getControlAtX(x);
    view->setActiveControl(ctrlIndex);
    
    switch (ctrlIndex) {
        case 0:
            if (onPrevious) onPrevious();
            break;
        case 1:
            if (onPlayPause) onPlayPause();
            break;
        case 2:
            if (onNext) onNext();
            break;
        case 3:
            if (onVolumeDown) onVolumeDown();
            break;
        case 4:
            if (onVolumeUp) onVolumeUp();
            break;
    }
}

void BottomBarController::setOnPrevious(std::function<void()> callback) {
    onPrevious = callback;
}

void BottomBarController::setOnPlayPause(std::function<void()> callback) {
    onPlayPause = callback;
}

void BottomBarController::setOnNext(std::function<void()> callback) {
    onNext = callback;
}

void BottomBarController::setOnVolumeDown(std::function<void()> callback) {
    onVolumeDown = callback;
}

void BottomBarController::setOnVolumeUp(std::function<void()> callback) {
    onVolumeUp = callback;
}

void BottomBarController::updateMediaInfo(const std::string& filename, float progress,
                                          int currentTime, int duration, bool paused) {
    view->setMediaFileName(filename);
    view->setProgress(progress);
    view->setTime(currentTime, duration);
    view->setPaused(paused);
}

void BottomBarController::setVolume(int volume) {
    view->setVolume(volume);
}

void BottomBarController::draw() {
    view->draw();
}

void BottomBarController::resize(int termWidth, int startY) {
    view = std::make_unique<BottomBarView>(termWidth, startY);
}