#include "MainConsoleView.h"

MainConsoleView::MainConsoleView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x) {}

void MainConsoleView::draw() {
    werase(window);
    box(window, 0, 0);

    mvwprintw(window, 2, 2, "Welcome to the Media Player!");

    wrefresh(window);
}

