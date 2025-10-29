#ifndef MAINCONSOLECONTROLLER_H
#define MAINCONSOLECONTROLLER_H

#include "../views/MainConsoleView.h"
#include <memory>

class MainConsoleController {
private:
    std::unique_ptr<MainConsoleView> view;
    
public:
    MainConsoleController(int height, int width, int startY, int startX);
    ~MainConsoleController() = default;

    // void updateView();
    void draw();
    
    // Resize
    void resize(int height, int width, int startY, int startX);
};

#endif
