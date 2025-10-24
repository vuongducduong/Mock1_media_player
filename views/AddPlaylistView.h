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
struct SelectedMediaFileInfo {
    std::string name;
    bool isFromPC;  // true = PC, false = USB
};

class AddPlaylistView : public BaseView {
private:
    std::string playlistName;
    std::vector<std::string> availableMediaFilesPC;
    std::vector<std::string> availableMediaFilesUSB;
    std::map<std::string, bool> selectedMediaFiles;  // tránh trùng
    
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
    void setAvailableMediaFilesPC(const std::vector<std::string>& songs);
    void setAvailableMediaFilesUSB(const std::vector<std::string>& songs);
    void reset();
    
    // Getters
    const std::string& getPlaylistName() const { return playlistName; }
    std::vector<std::string> getSelectedMediaFiles() const;
    int getSelectedCount() const { return selectedMediaFiles.size(); }
    
    // Navigation
    void scrollUp();
    void scrollDown();
    void switchToPC() { showingPC = true; scrollOffset = 0; }
    void switchToUSB() { showingPC = false; scrollOffset = 0; }
    
    // Song selection
    void toggleSong(int index);
    bool isMediaFileSelected(const std::string& song) const;
    
    // Click detection
    bool isNameInputClicked(int x, int localY) const;
    bool isPCTabClicked(int x, int localY) const;
    bool isUSBTabClicked(int x, int localY) const;
    bool isSaveButtonClicked(int x, int localY) const;
    bool isCancelButtonClicked(int x, int localY) const;
    int getSongAtY(int localY) const;
    
    // Input handling
    void editPlaylistName();
    std::vector<SelectedMediaFileInfo> getSelectedMediaFilesWithSource() const;
    void setSelectedMediaFiles(const std::vector<std::string>& songs);

    void setMode(AddPlaylistMode m) { mode = m; }
    AddPlaylistMode getMode() const { return mode; }
    void setEditingPlaylistIndex(int idx) { editingPlaylistIndex = idx; }
    int getEditingPlaylistIndex() const { return editingPlaylistIndex; }
    void setSelectedMediaFilesWithSource(const std::vector<SelectedMediaFileInfo>& songs);
 
    
private:
    void calculateMaxVisibleLines();
    const std::vector<std::string>& getCurrentMediaFileList() const;
};

#endif
