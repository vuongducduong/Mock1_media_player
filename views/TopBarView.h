#ifndef TOPBARVIEW_H
#define TOPBARVIEW_H

#include "BaseView.h"

class TopBarView : public BaseView {
private:
    std::vector<std::string> buttons;
    int pointedButton;
    int selectedButton;

public:
    TopBarView(int w);
    
    void draw() override;
    void setButtons(const std::vector<std::string>& btns) { buttons = btns; }
    void setPointedButton(int index) { pointedButton = index; }
    void setSelectedButton(int index) { selectedButton = index; }
    
    int getButtonAtX(int x) const;
};

#endif