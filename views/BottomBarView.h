#ifndef BOTTOMBARVIEW_H
#define BOTTOMBARVIEW_H

#include "BaseView.h"

class BottomBarView : public BaseView {
private:
    std::string mediaFileName;
    float progress;
    int currentTime;
    int totalTime;
    std::vector<std::string> controls;
    bool isPaused;
    int volumePercent = 50;
    int activeControlIndex = -1;

public:
    BottomBarView(int w, int y);
    
    void draw() override;
    void setMediaFileName(const std::string& name) { mediaFileName = name; }
    void setProgress(float p) { progress = p; }
    void setTime(int current, int total) { currentTime = current; totalTime = total; }
    void setControls(const std::vector<std::string>& c) { controls = c; }
    void setPaused(bool p) { isPaused = p; }
    
    int getControlAtX(int x) const;
    bool getPaused();
    void setVolume(int vol);
    void setActiveControl(int index);
};

#endif