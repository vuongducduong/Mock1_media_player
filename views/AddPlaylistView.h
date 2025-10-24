#ifndef ADDPLAYLISTVIEW_H
#define ADDPLAYLISTVIEW_H

#include "BaseView.h"
#include <vector>
#include <string>
#include <map>

enum class AddPlaylistSection {
    NAME_INPUT,      // Đang chọn ô nhập tên
    SOURCE_TABS,     // Đang chọn tab PC/USB
    SONG_LIST,       // Đang xem danh sách bài hát
    BUTTONS          // Đang ở nút Save/Cancel
};
enum class AddPlaylistMode {
    CREATE,
    EDIT
};
struct SelectedSongInfo {
    std::string name;
    bool isFromPC;  // true = PC, false = USB
};

class AddPlaylistView : public BaseView {
private:
    std::string playlistName;
    std::vector<std::string> availableSongsPC;
    std::vector<std::string> availableSongsUSB;
    std::map<std::string, bool> selectedSongs;  // tránh trùng
    
    int scrollOffset;
    int maxVisibleLines;
    
    bool showingPC;  // true = PC, false = USB
    AddPlaylistSection currentSection;

    AddPlaylistMode mode;
    int editingPlaylistIndex;

public:
    AddPlaylistView(int h, int w, int y, int x);
    
    void draw() override;
    
    // Setters
    void setPlaylistName(const std::string& name) { playlistName = name; }
    void setAvailableSongsPC(const std::vector<std::string>& songs);
    void setAvailableSongsUSB(const std::vector<std::string>& songs);
    void reset();
    
    // Getters
    const std::string& getPlaylistName() const { return playlistName; }
    std::vector<std::string> getSelectedSongs() const;
    int getSelectedCount() const { return selectedSongs.size(); }
    
    // Navigation
    void scrollUp();
    void scrollDown();
    void switchToPC() { showingPC = true; scrollOffset = 0; }
    void switchToUSB() { showingPC = false; scrollOffset = 0; }
    
    // Song selection
    void toggleSong(int index);
    bool isSongSelected(const std::string& song) const;
    
    // Click detection
    bool isNameInputClicked(int x, int localY) const;
    bool isPCTabClicked(int x, int localY) const;
    bool isUSBTabClicked(int x, int localY) const;
    bool isSaveButtonClicked(int x, int localY) const;
    bool isCancelButtonClicked(int x, int localY) const;
    int getSongAtY(int localY) const;
    
    // Input handling
    void editPlaylistName();
    std::vector<SelectedSongInfo> getSelectedSongsWithSource() const;
    void setSelectedSongs(const std::vector<std::string>& songs);

        void setMode(AddPlaylistMode m) { mode = m; }
    AddPlaylistMode getMode() const { return mode; }
    void setEditingPlaylistIndex(int idx) { editingPlaylistIndex = idx; }
    int getEditingPlaylistIndex() const { return editingPlaylistIndex; }
    void setSelectedSongsWithSource(const std::vector<SelectedSongInfo>& songs);
 
    
private:
    void calculateMaxVisibleLines();
    const std::vector<std::string>& getCurrentSongList() const;
};

#endif
