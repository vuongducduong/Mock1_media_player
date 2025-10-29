#include "UsbStatusBar.h"


UsbStatusBar::UsbStatusBar(int w)
    : BaseView(1, w, 0, 0),
      width(w),
      isConnected(false),
      ejectSelected(false),
      lastConnected(false),
      usbName("No Device"),
      lastName("No Device"),
      statusText("Ready"),
      lastStatus("Ready") {}

void UsbStatusBar::setUsbStatus(bool connected, const std::string& name, const std::string& status) {
    std::lock_guard<std::mutex> lock(mtx);
    isConnected = connected;
    usbName = name;
    statusText = status;
}

void UsbStatusBar::toggleEjectSelected() {
    std::lock_guard<std::mutex> lock(mtx);
    ejectSelected = !ejectSelected;
}

bool UsbStatusBar::isEjectSelected() const {
    std::lock_guard<std::mutex> lock(mtx);
    return ejectSelected;
}

void UsbStatusBar::render(const UsbManager& manager) {
    bool connected = manager.isUsbConnected();
    std::string name = manager.getUsbName();
    std::string status = manager.getStatusText();

    // Cập nhật khi có thay đổi
    if (connected != lastConnected || name != lastName || status != lastStatus) {
        setUsbStatus(connected, name, status);
        lastConnected = connected;
        lastName = name;
        lastStatus = status;
    }
}

void UsbStatusBar::draw() {
    std::lock_guard<std::mutex> lock(mtx);
    werase(window);

    wattron(window, A_BOLD);
    mvwprintw(window, 0, 1, "USB:");
    wattroff(window, A_BOLD);

    int x = 6;
    if (isConnected) {
        wattron(window, COLOR_PAIR(2));
        mvwprintw(window, 0, x, "%s", usbName.c_str());
        wattroff(window, COLOR_PAIR(2));
    } else {
        wattron(window, COLOR_PAIR(1));
        mvwprintw(window, 0, x, "No Device");
        wattroff(window, COLOR_PAIR(1));
    }

    // In status text (ở giữa)
    int statusX = width / 2 - (int)statusText.size() / 2;
    if (statusX < x + 10) statusX = x + 10;
    wattron(window, A_DIM);
    mvwprintw(window, 0, statusX, "[%s]", statusText.c_str());
    wattroff(window, A_DIM);

    // Nút Eject (chỉ khi có USB)
    if (isConnected) {
        const char* ejectLabel = "[Eject]";
        int ejectX = width - (int)strlen(ejectLabel) - 2;

        if (ejectSelected)
            wattron(window, A_REVERSE);

        mvwprintw(window, 0, ejectX, "%s", ejectLabel);

        if (ejectSelected)
            wattroff(window, A_REVERSE);
    } else {
        ejectSelected = false; // vô hiệu hóa eject khi không có USB
    }

    wnoutrefresh(window);
}

