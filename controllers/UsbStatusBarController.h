#ifndef USBSTATUSBARCONTROLLER_H
#define USBSTATUSBARCONTROLLER_H

#include "../models/UsbManager.h"
#include "../views/UsbStatusBar.h"
#include <memory>
#include <functional>

class UsbStatusBarController {
private:
    std::shared_ptr<UsbManager> usbManager;  // ⚠️ THAY ĐỔI: dùng shared_ptr để share với main
    std::unique_ptr<UsbStatusBar> usbStatusBar;
    int termWidth;
    
    // Callback khi eject được thực hiện
    std::function<void()> onEjectCallback;

public:
    UsbStatusBarController(std::shared_ptr<UsbManager> manager, int width);
    ~UsbStatusBarController() = default;
    
    // Khởi tạo
    void init();
    
    // Cập nhật và vẽ
    void update();
    void draw();
    
    // Xử lý sự kiện
    bool handleClick(int x, int y);
    bool handleKey(int ch);
    
    // Resize
    void resize(int width);
    
    // Getter
    std::shared_ptr<UsbManager> getUsbManager() { return usbManager; }
    bool isUsbConnected() const { return usbManager && usbManager->isUsbConnected(); }
    
    // Setter callback
    void setOnEject(std::function<void()> callback) {
        onEjectCallback = callback;
    }

private:
    void performEject();
    bool isClickOnEjectButton(int x, int y);
};

#endif