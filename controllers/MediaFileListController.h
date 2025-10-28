#ifndef MEDIAFILELISTCONTROLLER_H
#define MEDIAFILELISTCONTROLLER_H

#include "../views/MediaFileListView.h"
#include "../models/MediaManager.h"
#include <memory>
#include <functional>

class MediaFileListController {
private:
    std::unique_ptr<MediaFileListView> view;
    MediaManager* currentCollection;
    
    std::function<void(int)> onMediaFileClick;
    std::function<void(int)> onMediaFileRightClick;
    
public:
    MediaFileListController(int height, int width, int startY, int startX);
    ~MediaFileListController() = default;
    
    // Set collection hiện tại
    void setCollection(MediaManager* collection);
    
    // Xử lý click chuột
    void handleClick(int x, int y, int button);
    
    // Xử lý keyboard
    void handleKeyUp();
    void handleKeyDown();
    void handlePageUp();
    void handlePageDown();
    void handleKeyLeft();
    void handleKeyRight();
    
    // Xử lý scroll chuột
    void handleScrollUp();
    void handleScrollDown();
    
    // Set callbacks
    void setOnMediaFileClick(std::function<void(int)> callback);
    void setOnMediaFileRightClick(std::function<void(int)> callback);
    
    // Set title và update view
    void setTitle(const std::string& title);
    void updateView();
    void draw();
    
    // Resize
    void resize(int height, int width, int startY, int startX);
};

#endif