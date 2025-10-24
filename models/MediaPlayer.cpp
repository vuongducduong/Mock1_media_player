#include "MediaPlayer.h"

MediaPlayer::MediaPlayer() 
    : currentMusic(nullptr), state(PlayerState::STOPPED), 
      volume(50), progress(0), currentTime(0), duration(0) {}

MediaPlayer::~MediaPlayer() {
    close();
}

bool MediaPlayer::init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;
    Mix_VolumeMusic((int)(volume * 128.0 / 100));
    return true;
}

void MediaPlayer::play(const std::string& path) {
    if (currentMusic != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    
    currentMusic = Mix_LoadMUS(path.c_str());
    if (!currentMusic) return;
    
    Mix_PlayMusic(currentMusic, 1);
    state = PlayerState::PLAYING;
    currentTime = 0;
    progress = 0;
}

void MediaPlayer::pause() {
    if (state == PlayerState::PLAYING) {
        Mix_PauseMusic();
        state = PlayerState::PAUSED;
    }
}

void MediaPlayer::resume() {
    if (state == PlayerState::PAUSED) {
        Mix_ResumeMusic();
        state = PlayerState::PLAYING;
    }
}

void MediaPlayer::stop() {
    if (currentMusic) {
        Mix_HaltMusic();
        state = PlayerState::STOPPED;
        currentTime = 0;
        progress = 0;
    }
}

void MediaPlayer::setVolume(int vol) {
    volume = std::max(0, std::min(100, vol));
    Mix_VolumeMusic((int)(volume * 128.0 / 100));
}

void MediaPlayer::increaseVolume(int delta) {
    setVolume(volume + delta);
}

void MediaPlayer::decreaseVolume(int delta) {
    setVolume(volume - delta);
}

void MediaPlayer::updateProgress() {
    if (state == PlayerState::PLAYING && currentTime < duration) {
        currentTime++;
        progress = (float)currentTime / (float)duration;
    }
}

bool MediaPlayer::isPlaying() const {
    return Mix_PlayingMusic() != 0;
}

void MediaPlayer::close() {
    if (currentMusic != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
    }
    Mix_CloseAudio();
    SDL_Quit();
}