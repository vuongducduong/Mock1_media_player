#include "BoardView.h"

BoardView::BoardView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x) {}

void BoardView::draw() {
    werase(window);
    box(window, 0, 0);

    mvwprintw(window, 2, 2, "Board!");

    wrefresh(window);
}

