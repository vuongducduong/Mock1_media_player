#ifndef AUDIOPLAYERMODEL_H
#define AUDIOPLAYERMODEL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

enum class PlayerState {
    STOPPED,
    PLAYING,
    PAUSED
};

class AudioPlayerModel {
private:
    Mix_Music* currentMusic;
    PlayerState state;
    int volume;
    float progress;
    int currentTime;
    int duration;

public:
    AudioPlayerModel();
    ~AudioPlayerModel();
    
    bool init();
    void close();
    
    void play(const std::string& path);
    void pause();
    void resume();
    void stop();
    
    void setVolume(int vol);
    void increaseVolume(int delta = 1);
    void decreaseVolume(int delta = 1);
    
    void updateProgress();
    void setProgress(float p) { progress = p; }
    void setCurrentTime(int t) { currentTime = t; }
    void setDuration(int d) { duration = d; }
    
    PlayerState getState() const { return state; }
    int getVolume() const { return volume; }
    float getProgress() const { return progress; }
    int getCurrentTime() const { return currentTime; }
    int getDuration() const { return duration; }
    
    bool isPlaying() const;
};

#endif