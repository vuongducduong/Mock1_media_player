#include "MainController.h"

MainController::MainController() 
    : pcMediaFiles("./music"),
      usbMediaFiles("./usb"),
      usbManager(std::make_shared<UsbManager>()),  
      currentScreen(ScreenType::MAIN_CONSOLE),
      shouldExit(false),
      lastPcFileCount(0),
      lastUsbFileCount(0),
      lastUsbConnectedState(false) {
    
    lastPcRefreshCheck = std::chrono::steady_clock::now();
    lastUsbRefreshCheck = std::chrono::steady_clock::now();
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
    
    usbStatusBarCtrl = std::make_unique<UsbStatusBarController>(usbManager, termWidth);
    usbStatusBarCtrl->init();
    usbStatusBarCtrl->setOnEject([this]() {
        onUsbEject();
    });
    
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

    playerCtrl = std::make_unique<PlayerController>(&mediaPlayer);

    mainConsoleCtrl = std::make_unique<MainConsoleController>(
        termHeight - 8, termWidth, 4, 0);

    boardCtrl = std::make_unique<BoardController>(
        termHeight - 8, termWidth, 4, 0);

    // Load dữ liệu
    pcMediaFiles.load();
    usbMediaFiles.load();
    playlists.load();
    
    // Initialize file counts
    lastPcFileCount = pcMediaFiles.getAllMediaFiles().size();
    lastUsbFileCount = usbMediaFiles.getAllMediaFiles().size();
    lastUsbConnectedState = usbManager->isUsbConnected();
    
    // Vẽ màn hình ban đầu
    updateViews();
    
    return true;
}

void MainController::run() {
    while (!shouldExit) {
        handleInput();
        
        // Check for file changes periodically
        checkAndRefreshPC();
        checkAndRefreshUSB();
        
        updateViews();
    }
}

void MainController::checkAndRefreshPC() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastPcRefreshCheck).count();
    
    if (elapsed >= REFRESH_CHECK_INTERVAL_MS) {
        lastPcRefreshCheck = now;
        
        // Temporarily load to check file count
        size_t oldCount = lastPcFileCount;
        pcMediaFiles.load();
        size_t newCount = pcMediaFiles.getAllMediaFiles().size();
        
        if (newCount != oldCount) {
            lastPcFileCount = newCount;
            
            // If we're on THIS_PC screen, refresh the view
            if (currentScreen == ScreenType::THIS_PC) {
                refreshCurrentScreen();
            }
        }
    }
}

void MainController::checkAndRefreshUSB() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastUsbRefreshCheck).count();
    
    if (elapsed >= REFRESH_CHECK_INTERVAL_MS) {
        lastUsbRefreshCheck = now;
        
        // Check USB connection state
        bool currentUsbState = usbManager->isUsbConnected();
        
        // Detect USB disconnection
        if (lastUsbConnectedState && !currentUsbState) {
            lastUsbConnectedState = currentUsbState;
            
            // Auto-refresh USB screen if we're viewing it
            if (currentScreen == ScreenType::USB) {
                usbMediaFiles.load();
                lastUsbFileCount = usbMediaFiles.getAllMediaFiles().size();
                refreshCurrentScreen();
            }
        }
        
        // Detect USB connection or file changes
        if (currentUsbState) {
            size_t oldCount = lastUsbFileCount;
            usbMediaFiles.load();
            size_t newCount = usbMediaFiles.getAllMediaFiles().size();
            
            if (newCount != oldCount || !lastUsbConnectedState) {
                lastUsbFileCount = newCount;
                lastUsbConnectedState = currentUsbState;
                
                // If we're on USB screen, refresh the view
                if (currentScreen == ScreenType::USB) {
                    refreshCurrentScreen();
                }
            }
        }
        
        lastUsbConnectedState = currentUsbState;
    }
}

void MainController::refreshCurrentScreen() {
    switch (currentScreen) {
        case ScreenType::THIS_PC:
            if (mediaFileListCtrl) {
                mediaFileListCtrl->setCollection(&pcMediaFiles);
            }
            break;
            
        case ScreenType::USB:
            if (mediaFileListCtrl) {
                mediaFileListCtrl->setCollection(&usbMediaFiles);
            }
            break;
            
        default:
            break;
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
    // USB Status Bar có priority cao (xử lý phím 'e' để eject)
    if (usbStatusBarCtrl && usbStatusBarCtrl->handleKey(ch)) {
        updateViews();
        return;
    }
    
    // Metadata screen có priority cao
    if (currentScreen == ScreenType::METADATA) {
        if (metadataCtrl && metadataCtrl->handleKey(ch)) {
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
                if (mediaFileListCtrl) mediaFileListCtrl->handleKeyLeft();
            } else {
                if (topBarCtrl) topBarCtrl->handleLeft();
            }
            break;
            
        case KEY_RIGHT:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                if (mediaFileListCtrl) mediaFileListCtrl->handleKeyRight();
            } else {
                if (topBarCtrl) topBarCtrl->handleRight();
            }
            break;
            
        case KEY_UP:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                if (mediaFileListCtrl) mediaFileListCtrl->handleKeyUp();
            }
            break;
            
        case KEY_DOWN:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                if (mediaFileListCtrl) mediaFileListCtrl->handleKeyDown();
            }
            break;
            
        case KEY_NPAGE:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                if (mediaFileListCtrl) mediaFileListCtrl->handlePageDown();
            }
            break;
            
        case KEY_PPAGE:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                if (mediaFileListCtrl) mediaFileListCtrl->handlePageUp();
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
            if (playerCtrl) playerCtrl->togglePlayPause();
            break;
            
        case 'n':
        case 'N':
            if (playerCtrl) playerCtrl->playNext();
            break;
            
        case 'b':
        case 'B':
            if (playerCtrl) playerCtrl->playPrevious();
            break;
            
        case '+':
        case '=':
            if (playerCtrl) {
                playerCtrl->increaseVolume();
                if (bottomBarCtrl) bottomBarCtrl->setVolume(mediaPlayer.getVolume());
            }
            break;
            
        case '-':
        case '_':
            if (playerCtrl) {
                playerCtrl->decreaseVolume();
                if (bottomBarCtrl) bottomBarCtrl->setVolume(mediaPlayer.getVolume());
            }
            break;
    }
}

void MainController::handleMouse(int x, int y, int button) {
    // USB Status Bar có priority cao nhất (xử lý click vào nút Eject)
    if (usbStatusBarCtrl && usbStatusBarCtrl->handleClick(x, y)) {
        updateViews();
        return;
    }
    
    // Metadata screen có priority
    if (currentScreen == ScreenType::METADATA) {
        int localY = y - 4;
        if (metadataCtrl && metadataCtrl->handleClick(x, localY)) {
            updateViews();
            return;
        }
    }
    
    // Click vào top bar
    if (y > 0 && y < 3) {
        if (topBarCtrl) topBarCtrl->handleClick(x); 
        return;
    }
    
    // Click vào bottom bar
    if (y >= termHeight - 4) {
        int localY = y - (termHeight - 4);
        if (bottomBarCtrl) bottomBarCtrl->handleClick(x, localY);
        return;
    }
    
    // Click vào main area
    int localY = y - 4;
    int localX = x;
    
    switch (currentScreen) {
        case ScreenType::THIS_PC:
        case ScreenType::USB:
        case ScreenType::PLAYLIST_MEDIAFILES:
            if (mediaFileListCtrl) mediaFileListCtrl->handleClick(localX, localY, button);
            break;
            
        case ScreenType::PLAYLIST_LIST:
            if (playlistCtrl) playlistCtrl->handleClick(localX, localY, button);
            break;
            
        case ScreenType::ADD_PLAYLIST:
            if (addPlaylistCtrl) addPlaylistCtrl->handleClick(localX, localY);
            break;
            
        default:
            break;
    }
}

void MainController::handleResize() {
    getmaxyx(stdscr, termHeight, termWidth);
    clear();
    refresh();
    
    // Resize USB Status Bar Controller
    if (usbStatusBarCtrl) {
        usbStatusBarCtrl->resize(termWidth);
    }
    
    // Resize tất cả controllers
    if (topBarCtrl) topBarCtrl->resize(termWidth);
    if (mediaFileListCtrl) mediaFileListCtrl->resize(termHeight - 8, termWidth, 4, 0);
    if (bottomBarCtrl) bottomBarCtrl->resize(termWidth, termHeight - 4);
    if (playlistCtrl) playlistCtrl->resize(termHeight - 8, termWidth, 4, 0);
    
    metadataCtrl = std::make_unique<MetadataController>(
        termHeight - 8, termWidth, 4, 0);
    
    if (addPlaylistCtrl) addPlaylistCtrl->resize(termHeight - 8, termWidth, 4, 0);
    if (mainConsoleCtrl) mainConsoleCtrl->resize(termHeight - 8, termWidth, 4, 0);
    if (boardCtrl) boardCtrl->resize(termHeight - 8, termWidth, 4, 0);
    
    updateViews();
}

// Callback handlers
void MainController::onTopBarButtonClick(int btnIndex) {
    if (topBarCtrl) topBarCtrl->setSelectedButton(btnIndex);
    
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
    if (playerCtrl) playerCtrl->play(index);
}

void MainController::onMediaFileRightClick(int index) {
    MediaManager* collection = nullptr;
    if (currentScreen == ScreenType::THIS_PC) collection = &pcMediaFiles;
    else if (currentScreen == ScreenType::USB) collection = &usbMediaFiles;
    else if (currentScreen == ScreenType::PLAYLIST_MEDIAFILES) collection = &playlistMediaFiles;
    
    if (collection && metadataCtrl) {
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
    if (playlist && playerCtrl) {
        playlist->load();
        playlistMediaFiles = playlist->getMediaFiles();
        playerCtrl->setCollection(&playlistMediaFiles);
        switchScreen(ScreenType::PLAYLIST_MEDIAFILES);
    }
}

void MainController::onAddPlaylistSave() {
    if (!addPlaylistCtrl) return;
    
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

void MainController::onUsbEject() {
    // Force immediate refresh after eject
    usbMediaFiles.load();
    lastUsbFileCount = usbMediaFiles.getAllMediaFiles().size();
    lastUsbConnectedState = usbManager->isUsbConnected();
    
    // If we're on USB screen, refresh the view immediately
    if (currentScreen == ScreenType::USB && mediaFileListCtrl) {
        mediaFileListCtrl->setCollection(&usbMediaFiles);
        updateViews();
    }
}

void MainController::switchScreen(ScreenType screen) {
    currentScreen = screen;
    
    switch (screen) {
        case ScreenType::THIS_PC:
            pcMediaFiles.load();
            lastPcFileCount = pcMediaFiles.getAllMediaFiles().size();
            if (playerCtrl) playerCtrl->setCollection(&pcMediaFiles);
            if (mediaFileListCtrl) {
                mediaFileListCtrl->setCollection(&pcMediaFiles);
                mediaFileListCtrl->setTitle("This PC - Media");
            }
            break;
            
        case ScreenType::USB:
            usbMediaFiles.load();
            lastUsbFileCount = usbMediaFiles.getAllMediaFiles().size();
            if (playerCtrl) playerCtrl->setCollection(&usbMediaFiles);
            if (mediaFileListCtrl) {
                mediaFileListCtrl->setCollection(&usbMediaFiles);
                mediaFileListCtrl->setTitle("USB - Media");
            }
            break;
            
        case ScreenType::PLAYLIST_LIST:
            playlists.load();
            if (playlistCtrl) playlistCtrl->updateView();
            break;
            
        case ScreenType::PLAYLIST_MEDIAFILES:
            if (playerCtrl) playerCtrl->setCollection(&playlistMediaFiles);
            if (mediaFileListCtrl) {
                mediaFileListCtrl->setCollection(&playlistMediaFiles);
                mediaFileListCtrl->setTitle("Playlist Media " + playlistMediaFiles.getFolder());
            }
            break;
            
        case ScreenType::ADD_PLAYLIST:
            if (addPlaylistCtrl) addPlaylistCtrl->draw();
            break;
            
        default:
            break;
    }
}

void MainController::updateViews() {
    // QUAN TRỌNG: Cập nhật và vẽ USB Status Bar TRƯỚC TIÊN
    if (usbStatusBarCtrl) {
        usbStatusBarCtrl->update();
        usbStatusBarCtrl->draw();
    }

    // Vẽ top bar
    if (topBarCtrl) {
        topBarCtrl->draw();
    }
    
    // Vẽ main content theo screen
    switch (currentScreen) {
        case ScreenType::MAIN_CONSOLE:
            if (mainConsoleCtrl) mainConsoleCtrl->draw();
            break;
            
        case ScreenType::THIS_PC:
        case ScreenType::USB:
        case ScreenType::PLAYLIST_MEDIAFILES:
            if (mediaFileListCtrl) {
                mediaFileListCtrl->updateView();
                mediaFileListCtrl->draw();
            }
            break;
            
        case ScreenType::PLAYLIST_LIST:
            if (playlistCtrl) {
                playlistCtrl->updateView();
                playlistCtrl->draw();
            }
            break;
            
        case ScreenType::ADD_PLAYLIST:
            if (addPlaylistCtrl) addPlaylistCtrl->draw();
            break;
            
        case ScreenType::BOARD:
            if (boardCtrl) boardCtrl->draw();
            break;
            
        case ScreenType::METADATA:
            if (metadataCtrl) metadataCtrl->draw();
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
    
    if (bottomBarCtrl) {
        bottomBarCtrl->updateMediaInfo(
            currentMediaFileName,
            mediaPlayer.getProgress(),
            mediaPlayer.getCurrentTime(),
            mediaPlayer.getDuration(),
            mediaPlayer.getState() == PlayerState::PAUSED
        );
        bottomBarCtrl->draw();
    }
    
    doupdate();
}