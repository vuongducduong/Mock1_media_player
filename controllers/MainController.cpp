#include "MainController.h"

MainController::MainController() 
    : currentScreen(ScreenType::MAIN_CONSOLE),
      pcMediaFiles("./music"),
      usbMediaFiles("./usb") {
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
    mousemask(BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON3_CLICKED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
    
    // Lấy kích thước terminal
    getmaxyx(stdscr, termHeight, termWidth);
    
    // Khởi tạo audio
    if (!mediaPlayer.init()) {
        endwin();
        std::cerr << "Failed to initialize audio\n";
        return false;
    }
    
    // Tạo views
    topBar = std::make_unique<TopBarView>(termWidth);
    topBar->setButtons({"Main console", "This PC", "From USB", "Playlist", "Board", "Exit"});
    
    mediafileList = std::make_unique<MediaFileListView>(
        termHeight - 7, termWidth, 3, 0);
    
    bottomBar = std::make_unique<BottomBarView>(termWidth, termHeight - 4);
    
    playlistView = std::make_unique<PlaylistView>(
        termHeight - 7, termWidth, 3, 0);
    
    metadataView = std::make_unique<MetadataView>(
        termHeight - 7, termWidth, 3, 0);

    addPlaylistView = std::make_unique<AddPlaylistView>(
    termHeight - 7, termWidth, 3, 0);
    // Tạo player controller
    playerCtrl = std::make_unique<PlayerController>(&mediaPlayer);
    
    // Load dữ liệu
    pcMediaFiles.load();
    usbMediaFiles.load();
    playlists.load();
    
    // Vẽ màn hình ban đầu
    updateViews();
    
    return true;
}

void MainController::run() {
    bool shouldExit = false;
    
    while (!shouldExit) {
        handleInput();
        updateViews();
        
        // Kiểm tra điều kiện thoát
        if (currentScreen == ScreenType::MAIN_CONSOLE) {
            // Có thể thêm logic thoát ở đây
        }
    }
}

//Hàm xử lý input chung cả chuột và bàn phím
void MainController::handleInput() {
    timeout(100); // Set timeout cho getch() để không block vô hạn
    int ch = getch();
    
    if (ch == ERR) {
        // Không có input, return
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
                // Scroll up với chuột
                if (currentScreen == ScreenType::THIS_PC || 
                    currentScreen == ScreenType::USB || 
                    currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                    mediafileList->scrollUp();
                }
                 else if (currentScreen == ScreenType::ADD_PLAYLIST) {
                    addPlaylistView->scrollUp();
                }
                 updateViews();
            } else if (event.bstate & BUTTON5_PRESSED) {
                // Scroll down với chuột
                if (currentScreen == ScreenType::THIS_PC || 
                    currentScreen == ScreenType::USB || 
                    currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                    mediafileList->scrollDown();
                }
                else if (currentScreen == ScreenType::ADD_PLAYLIST) {
                    addPlaylistView->scrollDown();
                }
                updateViews();
            }
            else if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                handleMouse(event.x, event.y, 2); 
            }
        }
    } else {
        handleKeyboard(ch);
    }
}

void MainController::handleKeyboard(int ch) {
    switch (ch) {
        case KEY_LEFT:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediafileList->prevPage();
            } else {
                topBar->setPointedButton(
                    (topBar->getButtonAtX(0) - 1 + 6) % 6);
            }
            break;
            
        case KEY_RIGHT:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediafileList->nextPage();
            } else {
                topBar->setPointedButton(
                    (topBar->getButtonAtX(0) + 1) % 6);
            }
            break;
            
        case KEY_UP:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediafileList->scrollUp();
            }
            break;
            
        case KEY_DOWN:
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediafileList->scrollDown();
            }
            break;
            
        case KEY_NPAGE: // Page Down
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediafileList->nextPage();
            }
            break;
            
        case KEY_PPAGE: // Page Up
            if (currentScreen == ScreenType::THIS_PC || 
                currentScreen == ScreenType::USB || 
                currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
                mediafileList->prevPage();
            }
            break;
            
        case '\n': // Enter key
            // Có thể thêm chức năng chọn bài hát bằng Enter
            break;
            
        case 27: // ESC
            if (currentScreen == ScreenType::METADATA) {
                switchScreen(ScreenType::THIS_PC);
            }
            if (currentScreen == ScreenType::ADD_PLAYLIST) {
                switchScreen(ScreenType::PLAYLIST_LIST);
            }
            if (currentScreen == ScreenType::PLAYLIST_MEDIAFILES) {
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
            bottomBar->setVolume(mediaPlayer.getVolume());
            break;
            
        case '-':
        case '_':
            playerCtrl->decreaseVolume();
            bottomBar->setVolume(mediaPlayer.getVolume());
            break;
    }
}

//Hàm xử lý chuột chung
void MainController::handleMouse(int x, int y, int button) {
    // Click vào top bar
    if (y < 3) {
        onTopBarClick(x);
        return;
    }
    
    // Click vào bottom bar
    if (y >= termHeight - 4) {
        int localY = y - (termHeight - 4);
        if (localY == 2) {
            onControlClick(x);
        }
        return;
    }
    
    // Click vào main area
    int localY = y - 3;
    int localX = x;
    
    switch (currentScreen) {
        case ScreenType::THIS_PC:
             // Kiểm tra click vào nút pagination
            if (mediafileList->isPrevButtonClicked(localX, localY)) {
                mediafileList->prevPage();
                return;
            }
            if (mediafileList->isNextButtonClicked(localX, localY)) {
                mediafileList->nextPage();
                return;
            }
            // Click vào bài hát
            onMediaFileListClick(localY, button == 3);
            break;
        case ScreenType::USB:
        case ScreenType::PLAYLIST_MEDIAFILES:
            // Kiểm tra click vào nút pagination
            if (mediafileList->isPrevButtonClicked(localX, localY)) {
                mediafileList->prevPage();
                return;
            }
            if (mediafileList->isNextButtonClicked(localX, localY)) {
                mediafileList->nextPage();
                return;
            }
            // Click vào bài hát
            onMediaFileListClick(localY, button == 3);
            break;
            
        case ScreenType::PLAYLIST_LIST:
            onPlaylistClick(localX, localY, button);
            break;

        case ScreenType::ADD_PLAYLIST:
            onAddPlaylistClick(x, y);
            break;
            
        default:
            break;
    }
}
// hàm xử lý chuột trên thanh topbar
void MainController::onTopBarClick(int x) {
    int btnIndex = topBar->getButtonAtX(x);
    topBar->setSelectedButton(btnIndex);
    
    switch (btnIndex) {
        case 0: switchScreen(ScreenType::MAIN_CONSOLE); break;
        case 1: switchScreen(ScreenType::THIS_PC); break;
        case 2: switchScreen(ScreenType::USB); break;
        case 3: switchScreen(ScreenType::PLAYLIST_LIST); break;
        case 4: /* Board */ break;
        case 5: endwin(); exit(0); break;
    }
}

// hàm xử lý chuột ấn vào bài hát để chạy hoặc hiện metadata
void MainController::onMediaFileListClick(int y, bool rightClick) {
    int mediafileIndex = mediafileList->getSongAtY(y);
    if (mediafileIndex < 0) return;
    
    if (rightClick) {
        // Hiển thị metadata
        MediaManager* collection = nullptr;
        if (currentScreen == ScreenType::THIS_PC) collection = &pcMediaFiles;
        else if (currentScreen == ScreenType::USB) collection = &usbMediaFiles;
        else if (currentScreen == ScreenType::PLAYLIST_MEDIAFILES) collection = &playlistMediaFiles;
        
        if (collection) {
            auto mediafile= collection->getMediaFile(mediafileIndex);
            if (mediafile) {
                metadataView->setFilename(mediafile->getFilename());
                metadataView->setMetadata(mediafile->getMediaMetadata());
                currentScreen = ScreenType::METADATA;
            }
        }
    } else {
        // Phát nhạc
        playerCtrl->play(mediafileIndex);
    }
}

//hàm xử lý chuột trong màn hình playlist
void MainController::onPlaylistClick(int x,int y, int button) {

    if (playlistView->isAddButtonClicked(x, y)) {
        //  Chuyển sang màn hình add playlist
        currentScreen = ScreenType::ADD_PLAYLIST;
        addPlaylistView->reset();
        addPlaylistView->setAvailableMediaFilesPC(pcMediaFiles.getMediaFileNames());
        addPlaylistView->setAvailableMediaFilesUSB(usbMediaFiles.getMediaFileNames());
        switchScreen(currentScreen);
        return;
    }
    
    if (playlistView->isRemoveButtonClicked(x, y)) {
        int index = playlists.getSelectedIndex();
        if (index >= 0) {
            playlists.deletePlaylist(index);
        }
        switchScreen(currentScreen);
        return;
    }
if (playlistView->isEditButtonClicked(x, y)) {
    int index = playlists.getSelectedIndex();
    if (index >= 0) {
        auto playlist = playlists.getPlaylist(index);
        if (playlist) {
            // Load playlist trước
            playlist->load();
            
            currentScreen = ScreenType::ADD_PLAYLIST;
            addPlaylistView->setMode(AddPlaylistMode::EDIT);
            addPlaylistView->setEditingPlaylistIndex(index);
            addPlaylistView->setPlaylistName(playlist->getPlayListName());
            
            // Load danh sách available songs
            addPlaylistView->setAvailableMediaFilesPC(pcMediaFiles.getMediaFileNames());
            addPlaylistView->setAvailableMediaFilesUSB(usbMediaFiles.getMediaFileNames());

            std::vector<SelectedMediaFileInfo> setAvailableMediaFiles;
            auto& playlistMediaFiles = playlist->getMediaFiles();   
            
            // Lấy danh sách tên file từ PC và USB để so sánh
            auto pcList = pcMediaFiles.getMediaFileNames();
            auto usbList = usbMediaFiles.getMediaFileNames();
            
            for (const auto& mediafile: playlistMediaFiles.getAllMediaFiles()) {
                SelectedMediaFileInfo info;
                info.name = mediafile->getFilename();
                
                // Tìm trong danh sách PC trước
                bool foundInPC = false;
                for (const auto& pcMediaFile: pcList) {
                    if (pcMediaFile== info.name) {
                        info.isFromPC = true;
                        foundInPC = true;
                        break;
                    }
                }
                
                // Nếu không có trong PC, tìm trong USB
                if (!foundInPC) {
                    for (const auto& usbMediaFile: usbList) {
                        if (usbMediaFile== info.name) {
                            info.isFromPC = false;
                            break;
                        }
                    }
                }
                
                setAvailableMediaFiles.push_back(info);
            }
            
            addPlaylistView->setSelectedMediaFilesWithSource(setAvailableMediaFiles);
            switchScreen(currentScreen);
        }
    }
    return;
}
    int playlistIndex = playlistView->getPlaylistAtY(y);
    if (playlistIndex >= 0) {
        if (button == 1) {
            playlists.setSelectedIndex(playlistIndex);
            playlistView->setSelectedIndex(playlistIndex);
            playlistView->draw();
            return;
        }

        if (button == 2) {
            playlists.setSelectedIndex(playlistIndex);
            auto playlist = playlists.getPlaylist(playlistIndex);
            if (playlist) {
                playlist->load();
                playlistMediaFiles = playlist->getMediaFiles();
                playerCtrl->setCollection(&playlistMediaFiles);
                switchScreen(ScreenType::PLAYLIST_MEDIAFILES);
            }
        }
    }
}
// Thêm hàm mới xử lý click trong màn hình Add Playlist:
void MainController::onAddPlaylistClick(int x, int y) {
    int localY = y - 3;
    
    // Click vào ô nhập tên
    if (addPlaylistView->isNameInputClicked(x, localY)) {
        addPlaylistView->editPlaylistName();
        return;
    }
    
    // Click vào tab PC
    if (addPlaylistView->isPCTabClicked(x, localY)) {
        addPlaylistView->switchToPC();
        return;
    }
    
    // Click vào tab USB
    if (addPlaylistView->isUSBTabClicked(x, localY)) {
        addPlaylistView->switchToUSB();
        return;
    }
    
    // Click vào bài hát
    int mediafileIndex = addPlaylistView->getSongAtY(localY);
    if (mediafileIndex >= 0) {
        addPlaylistView->toggleSong(mediafileIndex);
        return;
    }
    
    // Click nút Save
    if (addPlaylistView->isSaveButtonClicked(x, localY)) {
        if (addPlaylistView->getSelectedCount() > 0 && 
            !addPlaylistView->getPlaylistName().empty()) {
            
            // THAY ĐỔI: Kiểm tra mode
            if (addPlaylistView->getMode() == AddPlaylistMode::CREATE) {
                // Tạo playlist mới
                playlists.createPlaylist(
                    addPlaylistView->getPlaylistName(),
                    addPlaylistView->getSelectedMediaFilesWithSource()
                );
            } else {
                // Update playlist hiện có
                playlists.updatePlaylist(
                    addPlaylistView->getEditingPlaylistIndex(),
                    addPlaylistView->getPlaylistName(),
                    addPlaylistView->getSelectedMediaFilesWithSource()
                );
            }
            clear();
            refresh();
            // Quay lại màn hình playlist
            switchScreen(ScreenType::PLAYLIST_LIST);
            playlists.load();
        }
        return;
    }
    
    // Click nút Cancel
    if (addPlaylistView->isCancelButtonClicked(x, localY)) {
        switchScreen(ScreenType::PLAYLIST_LIST);
        return;
    }
}

// Hàm check chuột ấn ở bottombar
void MainController::onControlClick(int x) {
    int ctrlIndex = bottomBar->getControlAtX(x);
    bottomBar->setActiveControl(ctrlIndex);
    switch (ctrlIndex) {
        case 0: playerCtrl->playPrevious(); break;
        case 1: 
        playerCtrl->togglePlayPause(); break;
        case 2: playerCtrl->playNext(); break;
        case 3: 
            playerCtrl->decreaseVolume();
            bottomBar->setVolume(mediaPlayer.getVolume());
             break;
        case 4:
            playerCtrl->increaseVolume();
            bottomBar->setVolume(mediaPlayer.getVolume());
            break;
    }
}
//hàm thay đổi dữ liệu khi đổi màn hình
void MainController::switchScreen(ScreenType screen) {
    currentScreen = screen;
    
    switch (screen) {
        case ScreenType::THIS_PC:
            pcMediaFiles.load();
            playerCtrl->setCollection(&pcMediaFiles);
            mediafileList->setMediaFile(pcMediaFiles.getMediaFileNames(), true);
            break;
        case ScreenType::USB:
            usbMediaFiles.load();
            playerCtrl->setCollection(&usbMediaFiles);
            mediafileList->setMediaFile(usbMediaFiles.getMediaFileNames(), true);
            break;
        case ScreenType::PLAYLIST_LIST:
            playlists.load();
            break;
        case ScreenType::PLAYLIST_MEDIAFILES:
            playerCtrl->setCollection(&playlistMediaFiles);
            mediafileList->setMediaFile(playlistMediaFiles.getMediaFileNames(), true);
            break;
        case ScreenType::ADD_PLAYLIST:
            addPlaylistView->draw();
        default:
            break;
    }
}

void MainController::updateViews() {
    // clear();
    // refresh();
    
    topBar->draw();
    
    switch (currentScreen) {
        case ScreenType::MAIN_CONSOLE:
            // Vẽ màn hình chính
            break;
            
        case ScreenType::THIS_PC:
            mediafileList->setMediaFile(pcMediaFiles.getMediaFileNames(),false);
            mediafileList->setTitle("This PC - Media");
            mediafileList->setCurrentPlayingIndex(pcMediaFiles.getCurrentIndex());
            mediafileList->draw();
            break;
            
        case ScreenType::USB:
            mediafileList->setMediaFile(usbMediaFiles.getMediaFileNames(),false); // 
            mediafileList->setTitle("USB - Media");
            mediafileList->setCurrentPlayingIndex(usbMediaFiles.getCurrentIndex());
            mediafileList->draw();
            break;
            
        case ScreenType::PLAYLIST_LIST:
            playlistView->setPlaylists(playlists.getPlaylistNames());
            playlistView->setSelectedIndex(playlists.getSelectedIndex());
            playlistView->draw();
            break;
            
        case ScreenType::PLAYLIST_MEDIAFILES:
            mediafileList->setMediaFile(playlistMediaFiles.getMediaFileNames(),false);
            mediafileList->setTitle("Playlist Media "+playlistMediaFiles.getFolder());
            mediafileList->setCurrentPlayingIndex(playlistMediaFiles.getCurrentIndex());
            mediafileList->draw();
            break;
        case ScreenType::ADD_PLAYLIST:
            addPlaylistView->draw();
            break;
        case ScreenType::METADATA:
            metadataView->draw();
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
    
    bottomBar->setMediaFileName(currentMediaFileName);
    bottomBar->setProgress(mediaPlayer.getProgress());
    bottomBar->setTime(mediaPlayer.getCurrentTime(), mediaPlayer.getDuration());
    bottomBar->setPaused(mediaPlayer.getState() == PlayerState::PAUSED);
    bottomBar->draw();

    doupdate();
}