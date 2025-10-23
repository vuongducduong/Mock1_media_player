#include "TopBarView.h"

TopBarView::TopBarView(int w) 
    : BaseView(3, w, 0, 0), pointedButton(0), selectedButton(0) {}

void TopBarView::draw() {
    werase(window);
    box(window, 0, 0);
    
    int spacing = width / buttons.size();
    
    for (size_t i = 0; i < buttons.size(); i++) {
        if ((int)i == selectedButton)
            wattron(window, A_REVERSE);
        
        mvwprintw(window, 1, i * spacing + 2, "%s", buttons[i].c_str());
        wattroff(window, A_REVERSE);
    }
    
    wrefresh(window);
}

int TopBarView::getButtonAtX(int x) const {
    int spacing = width / buttons.size();
    return x / spacing;
}
