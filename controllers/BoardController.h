#ifndef BOARDCONTROLLER_H
#define BOARDCONTROLLER_H

#include "../views/BoardView.h"
#include <memory>

class BoardController {
private:
    std::unique_ptr<BoardView> view;
    
public:
    BoardController(int height, int width, int startY, int startX);
    ~BoardController() = default;
    
    // void updateView();
    void draw();
    
    // Resize
    void resize(int height, int width, int startY, int startX);
};

#endif
