#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include "BaseView.h"

class BoardView : public BaseView {
private:

public:
    BoardView(int h, int w, int y, int x);
    
    void draw() override;
};

#endif