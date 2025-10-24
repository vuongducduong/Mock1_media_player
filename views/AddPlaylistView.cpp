#include "AddPlaylistView.h"
#include <algorithm>
#include <cstring>

AddPlaylistView::AddPlaylistView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x), playlistName("New Playlist"), 
      scrollOffset(0), showingPC(true), currentSection(AddPlaylistSection::NAME_INPUT),
      mode(AddPlaylistMode::CREATE), editingPlaylistIndex(-1) {
    calculateMaxVisibleLines();
}

void AddPlaylistView::calculateMaxVisibleLines() {
    // height - (borders + title + name input + tabs + selected count + buttons)
    maxVisibleLines = height - 9;
    if (maxVisibleLines < 5) maxVisibleLines = 5;
}

void AddPlaylistView::setAvailableMediaFilesPC(const std::vector<std::string>& songs) {
    availableMediaFilesPC = songs;
}

void AddPlaylistView::setAvailableMediaFilesUSB(const std::vector<std::string>& songs) {
    availableMediaFilesUSB = songs;
}

void AddPlaylistView::reset() {
    playlistName = "New Playlist";
    selectedMediaFiles.clear();
    scrollOffset = 0;
    showingPC = true;
    currentSection = AddPlaylistSection::NAME_INPUT;
    mode = AddPlaylistMode::CREATE;        // THÊM DÒNG NÀY
    editingPlaylistIndex = -1;             // THÊM DÒNG NÀY
}

std::vector<std::string> AddPlaylistView::getSelectedMediaFiles() const {
    std::vector<std::string> result;
    for (const auto& pair : selectedMediaFiles) {
        result.push_back(pair.first);
    }
    return result;
}

const std::vector<std::string>& AddPlaylistView::getCurrentMediaFileList() const {
    return showingPC ? availableMediaFilesPC : availableMediaFilesUSB;
}

void AddPlaylistView::scrollUp() {
    if (scrollOffset > 0) {
        scrollOffset--;
    }
}

void AddPlaylistView::scrollDown() {
    const auto& songs = getCurrentMediaFileList();
    int maxScroll = std::max(0, (int)songs.size() - maxVisibleLines);
    if (scrollOffset < maxScroll) {
        scrollOffset++;
    }
}

bool AddPlaylistView::isMediaFileSelected(const std::string& song) const {
    return selectedMediaFiles.count(song) > 0;
}

void AddPlaylistView::draw() {
    werase(window);
    box(window, 0, 0);
    
    int currentY = 1;
    
    // ===== TITLE =====
    wattron(window, A_BOLD);
    if (mode == AddPlaylistMode::CREATE) {
        mvwprintw(window, currentY++, (width - 20) / 2, "=== CREATE PLAYLIST ===");
    } 
    else {
        mvwprintw(window, currentY++, (width - 20) / 2, "=== EDIT PLAYLIST ===");
    }
    wattroff(window, A_BOLD);
    
    // ===== NAME INPUT =====
    mvwprintw(window, currentY, 2, "Playlist Name:");
    wattron(window, A_UNDERLINE | A_BOLD);
    mvwprintw(window, currentY, 18, "[%s]", playlistName.c_str());
    wattroff(window, A_UNDERLINE | A_BOLD);
    mvwprintw(window, currentY, 18 + playlistName.size() + 3, "<-- Click to edit");
    currentY += 2;
    
    // ===== SOURCE TABS =====
    std::string pcTab = "[ PC Songs ]";
    std::string usbTab = "[ USB Songs ]";
    
    int pcTabX = 4;
    int usbTabX = pcTabX + pcTab.size() + 4;
    
    // PC Tab
    if (showingPC) {
        wattron(window, A_STANDOUT | A_BOLD);
    }
    mvwprintw(window, currentY, pcTabX, "%s", pcTab.c_str());
    if (showingPC) {
        wattroff(window, A_STANDOUT | A_BOLD);
    }
    
    // USB Tab
    if (!showingPC) {
        wattron(window, A_STANDOUT | A_BOLD);
    }
    mvwprintw(window, currentY, usbTabX, "%s", usbTab.c_str());
    if (!showingPC) {
        wattroff(window, A_STANDOUT | A_BOLD);
    }
    
    currentY += 2;
    
    // ===== SONG LIST =====
    const auto& songs = getCurrentMediaFileList();
    std::string sourceLabel = showingPC ? "PC" : "USB";
    mvwprintw(window, currentY++, 2, "Available Songs from %s (Click to add/remove):", 
              sourceLabel.c_str());
    
    int listStartY = currentY;
    int displayEnd = std::min((int)songs.size(), scrollOffset + maxVisibleLines);
    
    for (int i = scrollOffset; i < displayEnd; ++i) {
        bool isSelected = isMediaFileSelected(songs[i]);
        
        // Cắt tên nếu quá dài
        std::string displayName = songs[i];
        int maxNameWidth = width - 12;
        if ((int)displayName.length() > maxNameWidth) {
            displayName = displayName.substr(0, maxNameWidth - 3) + "...";
        }
        
        if (isSelected) {
            wattron(window, A_BOLD);
            mvwprintw(window, currentY++, 4, "[V] %s", displayName.c_str());
            wattroff(window, A_BOLD);
        } else {
            wattron(window, A_DIM);
            mvwprintw(window, currentY++, 4, "[ ] %s", displayName.c_str());
            wattroff(window, A_DIM);
        }
    }
    
    // Scroll indicator
    if ((int)songs.size() > maxVisibleLines) {
        int maxScroll = songs.size() - maxVisibleLines;
        std::string scrollInfo = std::to_string(scrollOffset) + 
                                "/" + std::to_string(maxScroll);
        mvwprintw(window, listStartY, width - scrollInfo.size() - 2, "%s", scrollInfo.c_str());
    }
    
    // ===== SELECTED COUNT =====
    currentY = height - 2;
    wattron(window, A_BOLD);
    mvwprintw(window, currentY, 2, "Selected: %d songs from PC + USB", 
              (int)selectedMediaFiles.size());
    wattroff(window, A_BOLD);
    
    // ===== BUTTONS =====
    std::string saveBtn = "[ SAVE ]";
    std::string cancelBtn = "[ CANCEL ]";
    
    int saveBtnX = width - saveBtn.size() - cancelBtn.size() - 6;
    int cancelBtnX = saveBtnX + saveBtn.size() + 4;
    
    wattron(window, A_BOLD | A_STANDOUT);
    mvwprintw(window, currentY, saveBtnX, "%s", saveBtn.c_str());
    wattroff(window, A_BOLD | A_STANDOUT);
    
    wattron(window, A_BOLD);
    mvwprintw(window, currentY, cancelBtnX, "%s", cancelBtn.c_str());
    wattroff(window, A_BOLD);
    
    wnoutrefresh(window);
}

void AddPlaylistView::editPlaylistName() {
    // Hiển thị dialog nhập tên
    int dialogH = 7;
    int dialogW = 50;
    int dialogY = (height - dialogH) / 2;
    int dialogX = (width - dialogW) / 2;
    
    WINDOW* dialog = newwin(dialogH, dialogW, startY + dialogY, startX + dialogX);
    box(dialog, 0, 0);
    
    wattron(dialog, A_BOLD);
    mvwprintw(dialog, 1, 2, "Enter Playlist Name:");
    wattroff(dialog, A_BOLD);
    
    mvwprintw(dialog, 3, 2, "> ");
    wrefresh(dialog);
    
    // Enable input
    echo();
    curs_set(1);
    
    char buffer[100] = {0};
    strncpy(buffer, playlistName.c_str(), 99);
    
    // Hiển thị tên hiện tại
    mvwprintw(dialog, 3, 4, "%-40s", buffer);
    wmove(dialog, 3, 4);
    wrefresh(dialog);
    
    // Nhập tên mới
    char input[100] = {0};
    wgetnstr(dialog, input, 99);
    
    if (strlen(input) > 0) {
        playlistName = input;
    }
    
    // Disable input
    noecho();
    curs_set(0);
    
    delwin(dialog);
    touchwin(window);
}

// ===== CLICK DETECTION =====

bool AddPlaylistView::isNameInputClicked(int x, int localY) const {
    return (localY == 2 && x >= 18);
}

bool AddPlaylistView::isPCTabClicked(int x, int localY) const {
    int pcTabX = 4;
    std::string pcTab = "[ PC Songs ]";
    return (localY == 4 && x >= pcTabX && x < pcTabX + (int)pcTab.size());
}

bool AddPlaylistView::isUSBTabClicked(int x, int localY) const {
    std::string pcTab = "[ PC Songs ]";
    int usbTabX = 4 + pcTab.size() + 4;
    std::string usbTab = "[ USB Songs ]";
    return (localY == 4 && x >= usbTabX && x < usbTabX + (int)usbTab.size());
}

bool AddPlaylistView::isSaveButtonClicked(int x, int localY) const {
    if (localY != height - 2) return false;
    
    std::string saveBtn = "[ SAVE ]";
    std::string cancelBtn = "[ CANCEL ]";
    int saveBtnX = width - saveBtn.size() - cancelBtn.size() - 6;
    
    return (x >= saveBtnX && x < saveBtnX + (int)saveBtn.size());
}

bool AddPlaylistView::isCancelButtonClicked(int x, int localY) const {
    if (localY != height - 2) return false;
    
    std::string saveBtn = "[ SAVE ]";
    std::string cancelBtn = "[ CANCEL ]";
    int saveBtnX = width - saveBtn.size() - cancelBtn.size() - 6;
    int cancelBtnX = saveBtnX + saveBtn.size() + 4;
    
    return (x >= cancelBtnX && x < cancelBtnX + (int)cancelBtn.size());
}

int AddPlaylistView::getSongAtY(int localY) const {
    int listStartY = 7;
    if (localY < listStartY || localY > (height-3)) return -1;
    
    int index = scrollOffset + (localY - listStartY);
    const auto& songs = getCurrentMediaFileList();
    
    if (index >= 0 && index < (int)songs.size()) {
        return index;
    }
    return -1;
}
void AddPlaylistView::toggleSong(int index) {
    const auto& songs = getCurrentMediaFileList();
    if (index < 0 || index >= (int)songs.size()) return;
    
    std::string songName = songs[index];
    
    if (selectedMediaFiles.count(songName)) {
        selectedMediaFiles.erase(songName);
    } else {
        selectedMediaFiles[songName] = showingPC; // Track source
    }
}

std::vector<SelectedMediaFileInfo> AddPlaylistView::getSelectedMediaFilesWithSource() const {
    std::vector<SelectedMediaFileInfo> result;
    for (const auto& pair : selectedMediaFiles) {
        SelectedMediaFileInfo info;
        info.name = pair.first;
        info.isFromPC = pair.second;
        result.push_back(info);
    }
    return result;
}

void AddPlaylistView::setSelectedMediaFiles(const std::vector<std::string>& songs) {
    selectedMediaFiles.clear();

    // Đánh dấu lại những bài hát có trong playlist cũ
    for (const auto& name : songs) {
        bool found = false;

        // Kiểm tra trong danh sách PC
        if (std::find(availableMediaFilesPC.begin(), availableMediaFilesPC.end(), name) != availableMediaFilesPC.end()) {
            selectedMediaFiles[name] = true;  // true = từ PC
            found = true;
        }

        // Kiểm tra trong danh sách USB
        if (!found && std::find(availableMediaFilesUSB.begin(), availableMediaFilesUSB.end(), name) != availableMediaFilesUSB.end()) {
            selectedMediaFiles[name] = false; // false = từ USB
        }
    }
}

void AddPlaylistView::setSelectedMediaFilesWithSource(const std::vector<SelectedMediaFileInfo>& songs) {
    selectedMediaFiles.clear();
    for (const auto& info : songs) {
        selectedMediaFiles[info.name] = info.isFromPC;
    }
}
