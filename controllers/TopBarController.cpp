#include "TopBarController.h"

TopBarController::TopBarController(int termWidth) {
    view = std::make_unique<TopBarView>(termWidth);
}

void TopBarController::init() {
    view->setButtons({"Main console", "This PC", "From USB", "Playlist", "Board", "Exit"});
}

void TopBarController::handleClick(int x) {
    int btnIndex = view->getButtonAtX(x);
    view->setSelectedButton(btnIndex);
    
    if (onButtonClick) {
        onButtonClick(btnIndex);
    }
}

void TopBarController::handleLeft() {
    int currentBtn = view->getButtonAtX(0);
    int newBtn = (currentBtn - 1 + 6) % 6;
    view->setPointedButton(newBtn);
}

void TopBarController::handleRight() {
    int currentBtn = view->getButtonAtX(0);
    int newBtn = (currentBtn + 1) % 6;
    view->setPointedButton(newBtn);
}

void TopBarController::setOnButtonClick(std::function<void(int)> callback) {
    onButtonClick = callback;
}

void TopBarController::setSelectedButton(int index) {
    view->setSelectedButton(index);
}

void TopBarController::draw() {
    view->draw();
}

void TopBarController::resize(int termWidth) {
    view = std::make_unique<TopBarView>(termWidth);
    init();
}