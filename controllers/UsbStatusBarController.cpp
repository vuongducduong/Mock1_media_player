#include "UsbStatusBarController.h"
#include <cstring>

UsbStatusBarController::UsbStatusBarController(std::shared_ptr<UsbManager> manager,int width)
    : usbManager(manager),
    termWidth(width),
      onEjectCallback(nullptr) {
    usbStatusBar = std::make_unique<UsbStatusBar>(width);
}

void UsbStatusBarController::init() {
    if (usbManager) {
        usbManager->updateStatus();
    }
}

void UsbStatusBarController::update() {
    if (!usbManager) return;
    // Cập nhật trạng thái USB từ hệ thống
    usbManager->updateStatus();
    
    if (usbStatusBar) {
        usbStatusBar->render(*usbManager);
    }
}

void UsbStatusBarController::draw() {
    if (usbStatusBar) {
        usbStatusBar->draw();
    }
}

void UsbStatusBarController::resize(int width) {
    termWidth = width;
    usbStatusBar = std::make_unique<UsbStatusBar>(width);
    update();
}

bool UsbStatusBarController::handleClick(int x, int y) {
    if (!usbManager) return false;
    
    // Kiểm tra click vào nút Eject
    if (isClickOnEjectButton(x, y)) {
        if (usbManager->isUsbConnected()) {
            performEject();
            return true;
        }
    }
    return false;
}

bool UsbStatusBarController::handleKey(int ch) {
    if (!usbManager) return false;
    
    // Xử lý phím tắt 'e' hoặc 'E' để eject
    if (ch == 'e' || ch == 'E') {
        if (usbManager->isUsbConnected()) {
            performEject();
            return true;
        }
    }
    return false;
}

void UsbStatusBarController::performEject() {
    if (!usbManager || !usbStatusBar) return;
    
    // Toggle trạng thái selected (cho hiệu ứng highlight)
    usbStatusBar->toggleEjectSelected();
    
    // Thực hiện eject
    usbManager->ejectAll();
    
    // Callback nếu có
    if (onEjectCallback) {
        onEjectCallback();
    }
    
    // Reset selected sau khi eject
    usbStatusBar->toggleEjectSelected();
    
    // Cập nhật lại view
    update();
}

bool UsbStatusBarController::isClickOnEjectButton(int x, int y) {
    if (!usbManager) return false;
    
    // Kiểm tra y có nằm trong vùng USB status bar không (y == 0)
    if (y != 0) {
        return false;
    }
    
    // Chỉ active khi có USB connected
    if (!usbManager->isUsbConnected()) {
        return false;
    }
    
    // Tính vị trí của nút Eject
    const char* ejectLabel = "[Eject]";
    int ejectLength = strlen(ejectLabel);
    int ejectX = termWidth - ejectLength - 2;
    
    // Kiểm tra x có nằm trong vùng nút Eject không
    return (x >= ejectX && x < ejectX + ejectLength);
}