#ifndef BOTTOMBARCONTROLLER_H
#define BOTTOMBARCONTROLLER_H

#include "../views/BottomBarView.h"
#include "../models/MediaPlayer.h"
#include <memory>
#include <functional>

class BottomBarController {
private:
    std::unique_ptr<BottomBarView> view;
    
    std::function<void()> onPrevious;
    std::function<void()> onPlayPause;
    std::function<void()> onNext;
    std::function<void()> onVolumeDown;
    std::function<void()> onVolumeUp;
    
public:
    BottomBarController(int termWidth, int startY);
    ~BottomBarController() = default;
    
    // Xử lý click chuột
    void handleClick(int x, int y);
    
    // Set callbacks
    void setOnPrevious(std::function<void()> callback);
    void setOnPlayPause(std::function<void()> callback);
    void setOnNext(std::function<void()> callback);
    void setOnVolumeDown(std::function<void()> callback);
    void setOnVolumeUp(std::function<void()> callback);
    
    // Update thông tin
    void updateMediaInfo(const std::string& filename, float progress, 
                        int currentTime, int duration, bool paused);
    void setVolume(int volume);
    
    // Vẽ
    void draw();
    
    // Resize
    void resize(int termWidth, int startY);
};

#endif