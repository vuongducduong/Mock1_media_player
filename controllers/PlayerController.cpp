#include "PlayerController.h"

PlayerController::PlayerController(AudioPlayerModel* model) 
    : model(model), currentCollection(nullptr), running(true) {
    progressThread = std::thread(&PlayerController::updateProgressLoop, this);
}

PlayerController::~PlayerController() {
    running = false;
    if (progressThread.joinable()) {
        progressThread.join();
    }
}

void PlayerController::setCollection(SongCollection* collection) {
    currentCollection = collection;
}

void PlayerController::play(int index) {
    if (!currentCollection) return;
    
    auto song = currentCollection->getSong(index);
    if (!song) return;
    
    currentCollection->setCurrentIndex(index);
    
    if (song->isVideo()) {
        playVideoExternal(song->getFullPath());
        return;
    }
    
    model->setDuration(song->getDuration());
    model->play(song->getFullPath());
}

void PlayerController::playNext() {
    if (!currentCollection || !currentCollection->hasNext()) return;
    play(currentCollection->getCurrentIndex() + 1);
}

void PlayerController::playPrevious() {
    if (!currentCollection || !currentCollection->hasPrevious()) return;
    play(currentCollection->getCurrentIndex() - 1);
}

void PlayerController::togglePlayPause() {
    if (model->getState() == PlayerState::PLAYING) {
        model->pause();
    } else if (model->getState() == PlayerState::PAUSED) {
        model->resume();
    }
}

void PlayerController::stop() {
    model->stop();
}

void PlayerController::increaseVolume() {
    model->increaseVolume(5);
}

void PlayerController::decreaseVolume() {
    model->decreaseVolume(5);
}

void PlayerController::updateProgressLoop() {
    while (running) {
        if (model->isPlaying() && model->getState() == PlayerState::PLAYING) {
            model->updateProgress();
            
            if (model->getCurrentTime() >= model->getDuration()) {
                if (currentCollection && currentCollection->hasNext()) {
                    playNext();
                }
            }
        }
        sleep(1);
    }
}

void PlayerController::playVideoExternal(const std::string& path) {
    std::string command;
    if (system("which vlc > /dev/null 2>&1") == 0) {
        command = "vlc \"" + path + "\" > /dev/null 2>&1 &";
    } else if (system("which mpv > /dev/null 2>&1") == 0) {
        command = "mpv \"" + path + "\" > /dev/null 2>&1 &";
    } else {
        command = "xdg-open \"" + path + "\" > /dev/null 2>&1 &";
    }
    system(command.c_str());
}
