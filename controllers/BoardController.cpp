#include "BoardController.h"

BoardController::BoardController(int height, int width, int startY, int startX) {
    view = std::make_unique<BoardView>(height, width, startY, startX);
}


// void BoardController::updateView() {
//     if (playlistManager) {
//         view->setPlaylists(playlistManager->getPlaylistNames());
//         view->setSelectedIndex(playlistManager->getSelectedIndex());
//     }
// }

void BoardController::draw() {
    view->draw();
}

void BoardController::resize(int height, int width, int startY, int startX) {
    view = std::make_unique<BoardView>(height, width, startY, startX);
}   