#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "../models/AudioPlayerModel.h"
#include "../models/SongCollection.h"
#include <thread>
#include <atomic>
#include <memory>
#include <unistd.h>

class PlayerController {
private:
    AudioPlayerModel* model;
    SongCollection* currentCollection;
    
    std::thread progressThread;
    std::atomic<bool> running;

public:
    PlayerController(AudioPlayerModel* model);
    ~PlayerController();
    
    void setCollection(SongCollection* collection);
    
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