#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <ncursesw/ncurses.h>
#include <string>
#include <vector>

class BaseView {
protected:
    WINDOW* window;
    int height, width;
    int startY, startX;

public:
    BaseView(int h, int w, int y, int x);
    virtual ~BaseView();
    
    virtual void draw() = 0;
    virtual void clear();
    virtual void refresh();
    
    WINDOW* getWindow() const { return window; }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
};

#endif
