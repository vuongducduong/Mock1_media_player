#include "BottomBarView.h"
BottomBarView::BottomBarView(int w, int y) 
    : BaseView(4, w, y, 0), progress(0), currentTime(0), totalTime(0), isPaused(false) {
    controls = {"<<", "||", ">>", "Vol-", "Vol+"};
}

void BottomBarView::draw() {
    werase(window);
    box(window, 0, 0);
    
    // Hiển thị tên bài hát đang phát
    if (!mediaFileName.empty()) {
        mvwprintw(window, 0, 2, "Playing: %s", mediaFileName.c_str());
    }
    
    // Vẽ thanh progress
    const std::string label = "Progress: ";
    int available_for_bar = width - 4 - label.size() - 20; // trừ padding và thời gian
    if (available_for_bar < 8) available_for_bar = 8;
    
    int filled = (int)(available_for_bar * progress);
    if (filled > available_for_bar) filled = available_for_bar;
    
    std::string bar = "[";
    bar += std::string(filled, '=');
    bar += std::string(available_for_bar - filled, '-');
    bar += "]";
    
    int y_progress = 1;
    mvwprintw(window, y_progress, 2, "%s%s", label.c_str(), bar.c_str());
    
    // Hiển thị thời gian
    int minutes = currentTime / 60;
    int seconds = currentTime % 60;
    int totalMinutes = totalTime / 60;
    int totalSeconds = totalTime % 60;
    mvwprintw(window, y_progress, width - 15, "%02d:%02d / %02d:%02d", 
              minutes, seconds, totalMinutes, totalSeconds);
    
    // Vẽ các nút điều khiển
    int y_controls = 2;
    int spacing = 4;
    int total_width = 0;
    if(getPaused()){
        controls[1]={"|>"};
    }
    else{
        controls[1]={"||"};
    }
    for (const auto& c : controls) {
        total_width += 2 + c.size();
    }
    total_width += spacing * (controls.size() - 1);
    
    int x_start = (width - total_width) / 2;
    wattron(window, A_DIM);
    int xx = x_start;
    for (size_t i = 0; i < controls.size(); ++i) {
        if ((int)i == activeControlIndex) {
            wattron(window, A_REVERSE | A_BOLD); // hiệu ứng sáng nổi bật
            mvwprintw(window, y_controls, xx, "[%s]", controls[i].c_str());
            wattroff(window, A_REVERSE | A_BOLD);
            xx += 2 + controls[i].size();
            if (i + 1 < controls.size()) xx += spacing;
        } else {
            mvwprintw(window, y_controls, xx, "[%s]", controls[i].c_str());
            xx += 2 + controls[i].size();
            if (i + 1 < controls.size()) xx += spacing;
        }
    }
        //     mvwprintw(window, y_controls, xx, "[%s]", controls[i].c_str());
        // xx += 2 + controls[i].size();
        // if (i + 1 < controls.size()) xx += spacing;
    wattroff(window, A_DIM);

    std::string volText = "Volume: " + std::to_string(volumePercent) + "%";
    mvwprintw(window, 2, width - volText.size() - 3, "%s", volText.c_str());

    wrefresh(window);
}

int BottomBarView::getControlAtX(int x) const {
    int spacing = 4;
    int total_width = 0;
    for (const auto& c : controls) {
        total_width += 2 + c.size();
    }
    total_width += spacing * (controls.size() - 1);
    
    int x_start = (width - total_width) / 2;
    int xx = x_start;
    
    for (size_t i = 0; i < controls.size(); ++i) {
        int ctrl_width = 2 + controls[i].size();
        if (x >= xx && x <= xx + ctrl_width) {
            return i;
        }
        xx += ctrl_width + spacing;
    }
    return -1;
}

bool BottomBarView:: getPaused(){
    return isPaused;
}

void BottomBarView::setVolume(int vol) {
    volumePercent = std::max(0, std::min(vol, 100)); // Giới hạn 0-100%
}

void BottomBarView::setActiveControl(int index) {
    activeControlIndex = index;
    draw(); // vẽ lại để cập nhật highlight
}