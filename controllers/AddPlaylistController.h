#ifndef ADDPLAYLISTCONTROLLER_H
#define ADDPLAYLISTCONTROLLER_H

#include "../views/AddPlaylistView.h"
#include "../models/MediaManager.h"
#include "../models/PlaylistManager.h"
#include <memory>
#include <functional>

class AddPlaylistController {
private:
    std::unique_ptr<AddPlaylistView> view;
    
    std::function<void()> onSave;
    std::function<void()> onCancel;
    
public:
    AddPlaylistController(int height, int width, int startY, int startX);
    ~AddPlaylistController() = default;
    
    // Xử lý click chuột
    void handleClick(int x, int y);
    
    // Xử lý scroll
    void handleScrollUp();
    void handleScrollDown();
    
    // Set callbacks
    void setOnSave(std::function<void()> callback);
    void setOnCancel(std::function<void()> callback);
    
    // Các hàm setup cho create mode
    void setupForCreate(MediaManager* pcMedia, MediaManager* usbMedia);
    
    // Các hàm setup cho edit mode
    void setupForEdit(int playlistIndex, Playlist* playlist, 
                     MediaManager* pcMedia, MediaManager* usbMedia);
    
    // Lấy dữ liệu
    std::string getPlaylistName() const;
    std::vector<SelectedMediaFileInfo> getSelectedMediaFiles() const;
    int getSelectedCount() const;
    AddPlaylistMode getMode() const;
    int getEditingPlaylistIndex() const;
    
    // Reset và vẽ
    void reset();
    void draw();
    
    // Resize
    void resize(int height, int width, int startY, int startX);
};

#endif