#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "../models/MediaManager.h"
#include "../models/PlaylistManager.h"
#include "../models/MediaPlayer.h"
#include "../views/TopBarView.h"
#include "../views/MediaFileListView.h"
#include "../views/BottomBarView.h"
#include "../views/PlaylistView.h"
#include "../views/AddPlaylistView.h"
#include "../views/MetadataView.h"
#include "PlayerController.h"
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
    
    // Views
    std::unique_ptr<TopBarView> topBar;
    std::unique_ptr<MediaFileListView> mediafileList;
    std::unique_ptr<BottomBarView> bottomBar;
    std::unique_ptr<PlaylistView> playlistView;
    std::unique_ptr<MetadataView> metadataView;
    std::unique_ptr<AddPlaylistView> addPlaylistView;
    
    // Controllers
    std::unique_ptr<PlayerController> playerCtrl;
    
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
    
    void onTopBarClick(int x);
    void onMediaFileListClick(int y, bool rightClick);
    void onPlaylistClick(int x, int y, int button);
    void onControlClick(int x);
    void onAddPlaylistClick(int x, int y);

};

#endif