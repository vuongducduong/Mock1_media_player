#include "BaseView.h"
BaseView::BaseView(int h, int w, int y, int x) 
    : height(h), width(w), startY(y), startX(x) {
    window = newwin(h, w, y, x);
}

BaseView::~BaseView() {
    delwin(window);
}

void BaseView::clear() {
    werase(window);
}

void BaseView::refresh() {
    wrefresh(window);
}
