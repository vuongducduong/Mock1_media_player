#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "../models/MediaPlayer.h"
#include "../models/MediaManager.h"
#include <thread>
#include <atomic>
#include <memory>
#include <unistd.h>

class PlayerController {
private:
    MediaPlayer* model;
    MediaManager* currentCollection;
    
    std::thread progressThread;
    std::atomic<bool> running;

public:
    PlayerController(MediaPlayer* model);
    ~PlayerController();
    
    void setCollection(MediaManager* collection);
    
    void play(int index);
    void playNext();
    void playPrevious();
    void togglePlayPause();
    void stop();
    
    void increaseVolume();
    void decreaseVolume();
    
private:
    void updateProgressLoop();
    void playVideoExternal(const std::string& path);
};

#endif