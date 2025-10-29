#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "../models/MediaManager.h"
#include "../models/PlaylistManager.h"
#include "../models/MediaPlayer.h"
#include "../models/UsbManager.h"
#include "../views/UsbStatusBar.h"
#include "PlayerController.h"
#include "MetadataController.h"
#include "TopBarController.h"
#include "MediaFileListController.h"
#include "PlaylistController.h"
#include "AddPlaylistController.h"
#include "BottomBarController.h"
#include "MainConsoleController.h"
#include "BoardController.h"

#include <memory>


enum class ScreenType {
    MAIN_CONSOLE,
    THIS_PC,
    USB,
    PLAYLIST_LIST,
    PLAYLIST_MEDIAFILES,
    ADD_PLAYLIST,
    METADATA,
    BOARD,
    EXIT
};

class MainController {
private:
    // Models
    MediaPlayer mediaPlayer;
    MediaManager pcMediaFiles;
    MediaManager usbMediaFiles;
    MediaManager playlistMediaFiles;
    PlaylistManager playlists;
    UsbManager usbManager;
    
    // Controllers
    std::unique_ptr<TopBarController> topBarCtrl;
    std::unique_ptr<MediaFileListController> mediaFileListCtrl;
    std::unique_ptr<BottomBarController> bottomBarCtrl;
    std::unique_ptr<PlaylistController> playlistCtrl;
    std::unique_ptr<AddPlaylistController> addPlaylistCtrl;
    std::unique_ptr<PlayerController> playerCtrl;
    std::unique_ptr<MetadataController> metadataCtrl;
    std::unique_ptr<MainConsoleController> mainConsoleCtrl;
    std::unique_ptr<BoardController> boardCtrl;

    std::unique_ptr<UsbStatusBar> usbStatusBar;
    
    // State
    ScreenType currentScreen;
    int termHeight, termWidth;
    bool shouldExit;

public:
    MainController();
    ~MainController();
    
    bool init();
    void run();
    
private:
    void handleInput();
    void handleKeyboard(int ch);
    void handleMouse(int x, int y, int button);
    
    void switchScreen(ScreenType screen);
    void updateViews();
    void handleResize();
    
    // Callback handlers
    void onTopBarButtonClick(int btnIndex);
    void onMediaFileClick(int index);
    void onMediaFileRightClick(int index);
    void onPlaylistSelect(int index);
    void onPlaylistDoubleClick(int index);
    void onAddPlaylistSave();
    void onAddPlaylistCancel();
};

#endif