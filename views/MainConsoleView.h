#ifndef MAINCONSOLEVIEW_H
#define MAINCONSOLEVIEW_H

#include "BaseView.h"

class MainConsoleView : public BaseView {
private:

public:
    MainConsoleView(int h, int w, int y, int x);
    
    void draw() override;
};

#endif