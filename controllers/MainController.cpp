#include "MainController.h"


MainController::MainController() 
    : currentScreen(ScreenType::MAIN_CONSOLE),
      pcMediaFiles("./music"),
      usbMediaFiles("./usb"),
      shouldExit(false){      
      int termWidth = getmaxx(stdscr);
      usbStatusBar = std::make_unique<UsbStatusBar>(termWidth);
    }

MainController::~MainController() {
    endwin();
}

bool MainController::init() {
    // Khởi tạo ncurses
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON3_CLICKED | 
              BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
    
    // Lấy kích thước terminal
    getmaxyx(stdscr, termHeight, termWidth);
    
    // Khởi tạo audio
    if (!mediaPlayer.init()) {
        endwin();
        std::cerr << "Failed to initialize audio\n";
        return false;
    }
    
    // Tạo controllers
    topBarCtrl = std::make_unique<TopBarController>(termWidth);
    topBarCtrl->init();
    topBarCtrl->setOnButtonClick([this](int btnIndex) {
        onTopBarButtonClick(btnIndex);
    });
    
    mediaFileListCtrl = std::make_unique<MediaFileListController>(
        termHeight - 8, termWidth, 4, 0);
    mediaFileListCtrl->setOnMediaFileClick([this](int index) {
        onMediaFileClick(index);
    });
    mediaFileListCtrl->setOnMediaFileRightClick([this](int index) {
        onMediaFileRightClick(index);
    });
    
    bottomBarCtrl = std::make_unique<BottomBarController>(termWidth, termHeight - 4);
    bottomBarCtrl->setOnPrevious([this]() {
        playerCtrl->playPrevious();
    });
    bottomBarCtrl->setOnPlayPause([this]() {
        playerCtrl->togglePlayPause();
    });
    bottomBarCtrl->setOnNext([this]() {
        playerCtrl->playNext();
    });
    bottomBarCtrl->setOnVolumeDown([this]() {
        playerCtrl->decreaseVolume();
        bottomBarCtrl->setVolume(mediaPlayer.getVolume());
    });
    bottomBarCtrl->setOnVolumeUp([this]() {
        playerCtrl->increaseVolume();
        bottomBarCtrl->setVolume(mediaPlayer.getVolume());
    });
    
    playlistCtrl = std::make_unique<PlaylistController>(
        termHeight - 8, termWidth, 4, 0);
    playlistCtrl->setPlaylistManager(&playlists);
    playlistCtrl->setOnPlaylistSelect([this](int index) {
        onPlaylistSelect(index);
    });
    playlistCtrl->setOnPlaylistDoubleClick([this](int index) {
        onPlaylistDoubleClick(index);
    });
    playlistCtrl->setOnAddClick([this]() {
        addPlaylistCtrl->setupForCreate(&pcMediaFiles, &usbMediaFiles);
        switchScreen(ScreenType::ADD_PLAYLIST);
    });
    playlistCtrl->setOnRemoveClick([this]() {
        int index = playlists.getSelectedIndex();
        if (index >= 0) {
            playlists.deletePlaylist(index);
            switchScreen(currentScreen);
        }
    });
    playlistCtrl->setOnEditClick([this]() {
        int index = playlists.getSelectedIndex();
        if (index >= 0) {
            auto playlist = playlists.getPlaylist(index);
            if (playlist) {
                addPlaylistCtrl->setupForEdit(index, playlist.get(), 
                                             &pcMediaFiles, &usbMediaFiles);
                switchScreen(ScreenType::ADD_PLAYLIST);
            }
        }
    });
    
    addPlaylistCtrl = std::make_unique<AddPlaylistController>(
        termHeight - 8, termWidth, 4, 0);
    addPlaylistCtrl->setOnSave([this]() {
        onAddPlaylistSave();
    });
    addPlaylistCtrl->setOnCancel([this]() {
        onAddPlaylistCancel();
    });
    
    metadataCtrl = std::make_unique<MetadataController>(
        termHeight - 8, termWidth, 4, 0);
    
    // Tạo player controller
    playerCtrl = std::make_unique<PlayerController>(&mediaPlayer);
    
    // Tạo MainConsoleController
    mainConsoleCtrl = std::make_unique<MainConsoleController>(
        termHeight - 8, termWidth, 4, 0);
    // Tạo BoardController
    boardCtrl = std::make_unique<BoardController>(
        termHeight - 8, termWidth, 4, 0);
    // Load dữ liệu
    pcMediaFiles.load();
    usbMediaFiles.load();
    playlists.load();
    
    // Vẽ màn hình ban đầu
    updateViews();
    
    return true;
}

void MainController::run() {
    while (!shouldExit) {
        handleInput();
        updateViews();
    }
}

void MainController::handleInput() {
    timeout(100);
    int ch = getch();
    
    if (ch == ERR) {
        return;
    }
    
    if (ch == KEY_MOUSE) {
        MEVENT event;
        if (getmouse(&event) == OK) {
            if (event.bstate & BUTTON1_CLICKED) {
                handleMouse(event.x, event.y, 1);
            } else if (event.bstate & BUTTON3_CLICKED) {
                handleMouse(event.x, event.y, 3);
            } else if (event.bstate & BUTTON4_PRESSED) {
                // Scroll up
                if (currentScreen == ScreenType::THIS_PC || 
                    currentScreen == ScreenType::USB || 
                    currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                    mediaFileListCtrl->handleScrollUp();
                } else if (currentScreen == ScreenType::ADD_PLAYLIST) {
                    addPlaylistCtrl->handleScrollUp();
                }
                updateViews();
            } else if (event.bstate & BUTTON5_PRESSED) {
                // Scroll down
                if (currentScreen == ScreenType::THIS_PC || 
                    currentScreen == ScreenType::USB || 
                    currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                    mediaFileListCtrl->handleScrollDown();
                } else if (currentScreen == ScreenType::ADD_PLAYLIST) {
                    addPlaylistCtrl->handleScrollDown();
                }
                updateViews();
            } else if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                handleMouse(event.x, event.y, 2);
            }
        }
    } else {
        handleKeyboard(ch);
    }
}

void MainController::handleKeyboard(int ch) {
    // Metadata screen có priority cao nhất
    if (currentScreen == ScreenType::METADATA) {
        if (metadataCtrl->handleKey(ch)) {
            updateViews();
            return;
        }
    }
    
    switch (ch) {
        case KEY_RESIZE:
            handleResize();
            break;
            
        case KEY_LEFT:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediaFileListCtrl->handleKeyLeft();
            } else {
                topBarCtrl->handleLeft();
            }
            break;
            
        case KEY_RIGHT:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediaFileListCtrl->handleKeyRight();
            } else {
                topBarCtrl->handleRight();
            }
            break;
            
        case KEY_UP:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediaFileListCtrl->handleKeyUp();
            }
            break;
            
        case KEY_DOWN:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediaFileListCtrl->handleKeyDown();
            }
            break;
            
        case KEY_NPAGE:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediaFileListCtrl->handlePageDown();
            }
            break;
            
        case KEY_PPAGE:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediaFileListCtrl->handlePageUp();
            }
            break;
            
        case 27: // ESC
            if (currentScreen == ScreenType::METADATA) {
                switchScreen(ScreenType::THIS_PC);
            } else if (currentScreen == ScreenType::ADD_PLAYLIST) {
                switchScreen(ScreenType::PLAYLIST_LIST);
            } else if (currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                switchScreen(ScreenType::PLAYLIST_LIST);
            }
            break;
            
        case 'p':
        case 'P':
            playerCtrl->togglePlayPause();
            break;
            
        case 'n':
        case 'N':
            playerCtrl->playNext();
            break;
            
        case 'b':
        case 'B':
            playerCtrl->playPrevious();
            break;
            
        case '+':
        case '=':
            playerCtrl->increaseVolume();
            bottomBarCtrl->setVolume(mediaPlayer.getVolume());
            break;
            
        case '-':
        case '_':
            playerCtrl->decreaseVolume();
            bottomBarCtrl->setVolume(mediaPlayer.getVolume());
            break;
        case 'e':  // nhấn phím e để chọn Eject
            if (usbStatusBar)
                usbStatusBar->toggleEjectSelected();

            if (usbManager.isUsbConnected()) {
                usbManager.ejectAll();
            }
            break;
    }
}

void MainController::handleMouse(int x, int y, int button) {
    // Metadata screen có priority
    if (currentScreen == ScreenType::METADATA) {
       int localY = y-4;
        if (metadataCtrl->handleClick(x, localY)) {
            updateViews();
            return;
        }
    }
    
    // Click vào top bar
    if (y>0 && y < 3) {
        topBarCtrl->handleClick(x); 
        return;
    }
    
    // Click vào bottom bar
    if (y >= termHeight - 4) {
        int localY = y - (termHeight - 4);
        bottomBarCtrl->handleClick(x, localY);
        return;
    }
    
    // Click vào main area
   int localY = y-4;
    int localX = x;
    
    switch (currentScreen) {
        case ScreenType::THIS_PC:
        case ScreenType::USB:
        case ScreenType::PLAYLIST_MEDIAFILES:
            mediaFileListCtrl->handleClick(localX, localY, button);
            break;
            
        case ScreenType::PLAYLIST_LIST:
            playlistCtrl->handleClick(localX, localY, button);
            break;
            
        case ScreenType::ADD_PLAYLIST:
            addPlaylistCtrl->handleClick(localX, localY);
            break;
            
        default:
            break;
    }
}

void MainController::handleResize() {
    getmaxyx(stdscr, termHeight, termWidth);
    clear();
    refresh();
    
    // Resize tất cả controllers
    topBarCtrl->resize(termWidth);
    mediaFileListCtrl->resize(termHeight - 8, termWidth, 4, 0);
    bottomBarCtrl->resize(termWidth, termHeight - 4);
    playlistCtrl->resize(termHeight - 8, termWidth, 4, 0);
    metadataCtrl = std::make_unique<MetadataController>(
        termHeight - 8, termWidth, 4, 0);
    addPlaylistCtrl->resize(termHeight - 8, termWidth, 4, 0);
    mainConsoleCtrl->resize(termHeight - 8, termWidth, 4, 0);
    boardCtrl->resize(termHeight - 8, termWidth, 4, 0);
    
    updateViews();
}

// Callback handlers
void MainController::onTopBarButtonClick(int btnIndex) {
    topBarCtrl->setSelectedButton(btnIndex);
    
    switch (btnIndex) {
        case 0: switchScreen(ScreenType::MAIN_CONSOLE); break;
        case 1: switchScreen(ScreenType::THIS_PC); break;
        case 2: switchScreen(ScreenType::USB); break;
        case 3: switchScreen(ScreenType::PLAYLIST_LIST); break;
        case 4: switchScreen(ScreenType::BOARD); break;
        case 5: 
            shouldExit = true;
            endwin();
            exit(0);
            break;
    }
}

void MainController::onMediaFileClick(int index) {
    playerCtrl->play(index);
}

void MainController::onMediaFileRightClick(int index) {
    MediaManager* collection = nullptr;
    if (currentScreen == ScreenType::THIS_PC) collection = &pcMediaFiles;
    else if (currentScreen == ScreenType::USB) collection = &usbMediaFiles;
    else if (currentScreen == ScreenType::PLAYLIST_MEDIAFILES) collection = &playlistMediaFiles;
    
    if (collection) {
        auto mediafile = collection->getMediaFile(index);
        if (mediafile) {
            MediaMetadata* meta = mediafile->getMediaMetadata();
            if (!meta) return;
            
            metadataCtrl->setFilename(mediafile->getFilename());
            metadataCtrl->setMetadata(meta);
            metadataCtrl->setSourceMediaFile(mediafile.get());
            currentScreen = ScreenType::METADATA;
        }
    }
}

void MainController::onPlaylistSelect(int index) {
    // Playlist được select, có thể highlight
}

void MainController::onPlaylistDoubleClick(int index) {
    auto playlist = playlists.getPlaylist(index);
    if (playlist) {
        playlist->load();
        playlistMediaFiles = playlist->getMediaFiles();
        playerCtrl->setCollection(&playlistMediaFiles);
        switchScreen(ScreenType::PLAYLIST_MEDIAFILES);
    }
}

void MainController::onAddPlaylistSave() {
    if (addPlaylistCtrl->getMode() == AddPlaylistMode::CREATE) {
        playlists.createPlaylist(
            addPlaylistCtrl->getPlaylistName(),
            addPlaylistCtrl->getSelectedMediaFiles()
        );
    } else {
        playlists.updatePlaylist(
            addPlaylistCtrl->getEditingPlaylistIndex(),
            addPlaylistCtrl->getPlaylistName(),
            addPlaylistCtrl->getSelectedMediaFiles()
        );
    }
    clear();
    refresh();
    switchScreen(ScreenType::PLAYLIST_LIST);
    playlists.load();
}

void MainController::onAddPlaylistCancel() {
    switchScreen(ScreenType::PLAYLIST_LIST);
}

void MainController::switchScreen(ScreenType screen) {
    currentScreen = screen;
    
    switch (screen) {
        case ScreenType::THIS_PC:
            pcMediaFiles.load();
            playerCtrl->setCollection(&pcMediaFiles);
            mediaFileListCtrl->setCollection(&pcMediaFiles);
            mediaFileListCtrl->setTitle("This PC - Media");
            break;
            
        case ScreenType::USB:
            usbMediaFiles.load();
            playerCtrl->setCollection(&usbMediaFiles);
            mediaFileListCtrl->setCollection(&usbMediaFiles);
            mediaFileListCtrl->setTitle("USB - Media");
            break;
            
        case ScreenType::PLAYLIST_LIST:
            playlists.load();
            playlistCtrl->updateView();
            break;
            
        case ScreenType::PLAYLIST_MEDIAFILES:
            playerCtrl->setCollection(&playlistMediaFiles);
            mediaFileListCtrl->setCollection(&playlistMediaFiles);
            mediaFileListCtrl->setTitle("Playlist Media " + playlistMediaFiles.getFolder());
            break;
            
        case ScreenType::ADD_PLAYLIST:
            addPlaylistCtrl->draw();
            break;
            
        default:
            break;
    }
}

void MainController::updateViews() {
    getmaxyx(stdscr, termHeight, termWidth);
    // --- Cập nhật trạng thái USB ---
    usbManager.updateStatus();

    // Nếu terminal thay đổi kích thước → tạo lại thanh USB
    if (!usbStatusBar || usbStatusBar->getWidth() != termWidth) {
        usbStatusBar = std::make_unique<UsbStatusBar>(termWidth);
    }

    // Cập nhật dữ liệu hiển thị cho thanh USB
    usbStatusBar->render(usbManager);

    // Vẽ thanh USB
    usbStatusBar->draw();

    // Vẽ top bar luôn
    topBarCtrl->draw();
    
    // Vẽ main content theo screen
    switch (currentScreen) {
        case ScreenType::MAIN_CONSOLE:
            mainConsoleCtrl->draw();
            break;
            
        case ScreenType::THIS_PC:
        case ScreenType::USB:
        case ScreenType::PLAYLIST_MEDIAFILES:
            mediaFileListCtrl->updateView();
            mediaFileListCtrl->draw();
            break;
            
        case ScreenType::PLAYLIST_LIST:
            playlistCtrl->updateView();
            playlistCtrl->draw();
            break;
            
        case ScreenType::ADD_PLAYLIST:
            addPlaylistCtrl->draw();
            break;
        case ScreenType::BOARD:
            boardCtrl->draw();
            break;
        case ScreenType::METADATA:
            metadataCtrl->draw();
            break;
    }
    
    // Cập nhật bottom bar
    std::string currentMediaFileName;
    MediaManager* activeCollection = nullptr;
    
    if (currentScreen == ScreenType::THIS_PC) activeCollection = &pcMediaFiles;
    else if (currentScreen == ScreenType::USB) activeCollection = &usbMediaFiles;
    else if (currentScreen == ScreenType::PLAYLIST_MEDIAFILES) activeCollection = &playlistMediaFiles;
    
    if (activeCollection) {
        auto mediafile = activeCollection->getCurrentMediaFile();
        if (mediafile) {
            currentMediaFileName = mediafile->getFilename();
        }
    }
    
    bottomBarCtrl->updateMediaInfo(
        currentMediaFileName,
        mediaPlayer.getProgress(),
        mediaPlayer.getCurrentTime(),
        mediaPlayer.getDuration(),
        mediaPlayer.getState() == PlayerState::PAUSED
    );
    bottomBarCtrl->draw();
    
    doupdate();
}