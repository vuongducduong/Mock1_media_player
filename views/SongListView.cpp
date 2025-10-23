#include "SongListView.h"

SongListView::SongListView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x), currentPage(0), scrollOffsetInPage(0), songsPerPage(25) {
    calculateMaxVisibleLines();
}

void SongListView::calculateMaxVisibleLines() {
    // height - 3 (border + title line + bottom border)
    maxVisibleLines = height - 3;
    if(maxVisibleLines>=25){
        maxVisibleLines=25;
    }
}

void SongListView::setSongs(const std::vector<std::string>& s,bool reset) {
    if (reset || songs != s) {
        songs = s;
        currentPage = 0;
        scrollOffsetInPage = 0;
    } else {
        // Cùng danh sách, giữ nguyên trang & offset (chính là điều cần thiết)
        songs = s;
    }
}

int SongListView::getTotalPages() const {
    if (songs.empty()) return 1;
    return (songs.size() + songsPerPage - 1) / songsPerPage;
}

void SongListView::nextPage() {
    int totalPages = getTotalPages();
    if (currentPage < totalPages - 1) {
        currentPage++;
        scrollOffsetInPage = 0;
    }
}

void SongListView::prevPage() {
    if (currentPage > 0) {
        currentPage--;
        scrollOffsetInPage = 0;
    }
}

void SongListView::scrollUp() {
    if (scrollOffsetInPage > 0) {
        scrollOffsetInPage--;
    }
}

void SongListView::scrollDown() {
    int startIndex = currentPage * songsPerPage;
    int endIndex = std::min((int)songs.size(), startIndex + songsPerPage);
    int songsInPage = endIndex - startIndex;
    int maxScroll = std::max(0, songsInPage - maxVisibleLines);

    if (scrollOffsetInPage < maxScroll) {
        scrollOffsetInPage++;
    }
}


void SongListView::draw() {
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
    int pageStart = currentPage * songsPerPage;
    int pageEnd = std::min((int)songs.size(), pageStart + songsPerPage);
    int startIndex = pageStart + scrollOffsetInPage;
    int endIndex = std::min(pageEnd, startIndex + maxVisibleLines);

    int y = 2;
    for (int i = startIndex; i < endIndex; ++i) {
        std::string displayName = songs[i];
        int maxNameWidth = width - 10;
        if ((int)displayName.length() > maxNameWidth)
            displayName = displayName.substr(0, maxNameWidth - 3) + "...";
        if (i == currentPlayingIndex) wattron(window, A_UNDERLINE | A_BOLD);
        mvwprintw(window, y++, 4, "%2d. %s", i + 1, displayName.c_str());
        if (i == currentPlayingIndex) wattroff(window, A_UNDERLINE | A_BOLD);
    }
    wnoutrefresh(window);
}

int SongListView::getSongAtY(int localY) const {
    if (localY < 2) return -1;
    
    int index = currentPage * songsPerPage + scrollOffsetInPage + (localY - 2);
    int endIndex = std::min((int)songs.size(), currentPage * songsPerPage + songsPerPage);
    
    if (index >= 0 && index < endIndex && index < (int)songs.size()) {
        return index;
    }
    return -1;
}

int SongListView::getMaxVisibleSongs() const {
    return maxVisibleLines;
}

bool SongListView::isPrevButtonClicked(int x, int localY) const {
    if (localY != 1) return false;
    
    std::string prevBtn = "[ < Prev ]";
    std::string nextBtn = "[ Next > ]";
    int prevX = width - nextBtn.size() - prevBtn.size() - 6;
    
    return (x >= prevX && x < prevX + (int)prevBtn.size());
}

bool SongListView::isNextButtonClicked(int x, int localY) const {
    if (localY != 1) return false;
    
    std::string nextBtn = "[ Next > ]";
    int nextX = width - nextBtn.size() - 2;
    
    return (x >= nextX && x < nextX + (int)nextBtn.size());
}