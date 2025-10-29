#include "MainConsoleController.h"

MainConsoleController::MainConsoleController(int height, int width, int startY, int startX) {
    view = std::make_unique<MainConsoleView>(height, width, startY, startX);
}


// void MainConsoleController::updateView() {
//     if (playlistManager) {
//         view->setPlaylists(playlistManager->getPlaylistNames());
//         view->setSelectedIndex(playlistManager->getSelectedIndex());
//     }
// }

void MainConsoleController::draw() {
    view->draw();
}

void MainConsoleController::resize(int height, int width, int startY, int startX) {
    view = std::make_unique<MainConsoleView>(height, width, startY, startX);
}   