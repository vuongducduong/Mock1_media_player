#ifndef PLAYLISTCONTROLLER_H
#define PLAYLISTCONTROLLER_H

#include "../views/PlaylistView.h"
#include "../models/PlaylistManager.h"
#include <memory>
#include <functional>

class PlaylistController {
private:
    std::unique_ptr<PlaylistView> view;
    PlaylistManager* playlistManager;
    
    std::function<void(int)> onPlaylistSelect;
    std::function<void(int)> onPlaylistDoubleClick;
    std::function<void()> onAddClick;
    std::function<void()> onRemoveClick;
    std::function<void()> onEditClick;
    
public:
    PlaylistController(int height, int width, int startY, int startX);
    ~PlaylistController() = default;
    
    // Set playlist manager
    void setPlaylistManager(PlaylistManager* manager);
    
    // Xử lý click chuột
    void handleClick(int x, int y, int button);
    
    // Set callbacks
    void setOnPlaylistSelect(std::function<void(int)> callback);
    void setOnPlaylistDoubleClick(std::function<void(int)> callback);
    void setOnAddClick(std::function<void()> callback);
    void setOnRemoveClick(std::function<void()> callback);
    void setOnEditClick(std::function<void()> callback);
    
    // Update và vẽ
    void updateView();
    void draw();
    
    // Resize
    void resize(int height, int width, int startY, int startX);
};

#endif
