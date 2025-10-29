#pragma once
#include "BaseView.h"
#include "../models/UsbManager.h"
#include <string>
#include <ncurses.h>
#include <mutex>
#include <cstring>
class UsbStatusBar : public BaseView {
public:
    explicit UsbStatusBar(int w);

    void render(const UsbManager& manager);   // cập nhật trạng thái USB
    void draw() override;                     // vẽ UI

    void setUsbStatus(bool connected, const std::string& name, const std::string& status);
    void toggleEjectSelected();
    bool isEjectSelected() const;

private:
    int width;
    bool isConnected;
    bool ejectSelected;
    bool lastConnected;
    std::string usbName;
    std::string lastName;
    std::string statusText;
    std::string lastStatus;

    mutable std::mutex mtx;
};
