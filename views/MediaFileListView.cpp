#include "MediaFileListView.h"

MediaFileListView::MediaFileListView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x), currentPage(0), scrollOffsetInPage(0), mediafilePerPage(25) {
    calculateMaxVisibleLines();
}

void MediaFileListView::calculateMaxVisibleLines() {
    // height - 3 (border + title line + bottom border)
    maxVisibleLines = height - 3;
    if(maxVisibleLines>=25){
        maxVisibleLines=25;
    }
}

void MediaFileListView::setMediaFile(const std::vector<std::string>& s,bool reset) {
    if (reset || mediafile != s) {
        mediafile = s;
        currentPage = 0;
        scrollOffsetInPage = 0;
    } else {
        // Cùng danh sách, giữ nguyên trang & offset (chính là điều cần thiết)
        mediafile = s;
    }
}

int MediaFileListView::getTotalPages() const {
    if (mediafile.empty()) return 1;
    return (mediafile.size() + mediafilePerPage - 1) / mediafilePerPage;
}

void MediaFileListView::nextPage() {
    int totalPages = getTotalPages();
    if (currentPage < totalPages - 1) {
        currentPage++;
        scrollOffsetInPage = 0;
    }
}

void MediaFileListView::prevPage() {
    if (currentPage > 0) {
        currentPage--;
        scrollOffsetInPage = 0;
    }
}

void MediaFileListView::scrollUp() {
    if (scrollOffsetInPage > 0) {
        scrollOffsetInPage--;
    }
}

void MediaFileListView::scrollDown() {
    int startIndex = currentPage * mediafilePerPage;
    int endIndex = std::min((int)mediafile.size(), startIndex + mediafilePerPage);
    int mediafileInPage = endIndex - startIndex;
    int maxScroll = std::max(0, mediafileInPage - maxVisibleLines);

    if (scrollOffsetInPage < maxScroll) {
        scrollOffsetInPage++;
    }
}


void MediaFileListView::draw() {
    werase(window);
    box(window, 0, 0);

    std::string pageInfo = " (Page " + std::to_string(currentPage + 1) + 
                          "/" + std::to_string(getTotalPages()) + ")";
    std::string fullTitle = title + pageInfo;
    mvwprintw(window, 1, 2, "%s", fullTitle.c_str());

    // Nút chuyển trang
    std::string prevBtn = "[ < Prev ]";
    std::string nextBtn = "[ Next > ]";
    int btnY = 1;
    int prevX = width - nextBtn.size() - prevBtn.size() - 6;
    int nextX = width - nextBtn.size() - 2;

    if (currentPage > 0) wattron(window, A_BOLD); else wattron(window, A_DIM);
    mvwprintw(window, btnY, prevX, "%s", prevBtn.c_str());
    wattroff(window, A_BOLD | A_DIM);

    if (currentPage < getTotalPages() - 1) wattron(window, A_BOLD); else wattron(window, A_DIM);
    mvwprintw(window, btnY, nextX, "%s", nextBtn.c_str());
    wattroff(window, A_BOLD | A_DIM);

    // Tính chỉ số hiển thị bài hát
    int pageStart = currentPage * mediafilePerPage;
    int pageEnd = std::min((int)mediafile.size(), pageStart + mediafilePerPage);
    int startIndex = pageStart + scrollOffsetInPage;
    int endIndex = std::min(pageEnd, startIndex + maxVisibleLines);

    int y = 2;
    for (int i = startIndex; i < endIndex; ++i) {
        std::string displayName = mediafile[i];
        int maxNameWidth = width - 10;
        if ((int)displayName.length() > maxNameWidth)
            displayName = displayName.substr(0, maxNameWidth - 3) + "...";
        if (i == currentPlayingIndex) wattron(window, A_UNDERLINE | A_BOLD);
        mvwprintw(window, y++, 4, "%2d. %s", i + 1, displayName.c_str());
        if (i == currentPlayingIndex) wattroff(window, A_UNDERLINE | A_BOLD);
    }
    wnoutrefresh(window);
}

int MediaFileListView::getSongAtY(int localY) const {
    if (localY < 2) return -1;
    
    int index = currentPage * mediafilePerPage + scrollOffsetInPage + (localY - 2);
    int endIndex = std::min((int)mediafile.size(), currentPage * mediafilePerPage + mediafilePerPage);
    
    if (index >= 0 && index < endIndex && index < (int)mediafile.size()) {
        return index;
    }
    return -1;
}

int MediaFileListView::getMaxVisiblemediafile() const {
    return maxVisibleLines;
}

bool MediaFileListView::isPrevButtonClicked(int x, int localY) const {
    if (localY != 1) return false;
    
    std::string prevBtn = "[ < Prev ]";
    std::string nextBtn = "[ Next > ]";
    int prevX = width - nextBtn.size() - prevBtn.size() - 6;
    
    return (x >= prevX && x < prevX + (int)prevBtn.size());
}

bool MediaFileListView::isNextButtonClicked(int x, int localY) const {
    if (localY != 1) return false;
    
    std::string nextBtn = "[ Next > ]";
    int nextX = width - nextBtn.size() - 2;
    
    return (x >= nextX && x < nextX + (int)nextBtn.size());
}