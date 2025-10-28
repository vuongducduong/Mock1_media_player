#ifndef TOPBARCONTROLLER_H
#define TOPBARCONTROLLER_H

#include "../views/TopBarView.h"
#include <memory>
#include <functional>

class TopBarController {
private:
    std::unique_ptr<TopBarView> view;
    std::function<void(int)> onButtonClick;
    
public:
    TopBarController(int termWidth);
    ~TopBarController() = default;
    
    // Khởi tạo các button
    void init();
    
    // Xử lý click chuột
    void handleClick(int x);
    
    // Xử lý phím mũi tên trái/phải để di chuyển giữa các button
    void handleLeft();
    void handleRight();
    
    // Set callback khi button được click
    void setOnButtonClick(std::function<void(int)> callback);
    
    // Cập nhật button được chọn
    void setSelectedButton(int index);
    
    // Vẽ view
    void draw();
    
    // Resize
    void resize(int termWidth);
};

#endif