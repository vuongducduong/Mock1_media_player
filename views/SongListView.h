#ifndef SONGLISTVIEW_H
#define SONGLISTVIEW_H

#include "BaseView.h"

class SongListView : public BaseView {
private:
    std::vector<std::string> songs;
    std::string title;
    int currentPage;
    int scrollOffsetInPage;
    int songsPerPage;
    int maxVisibleLines;

    bool reset=false;

public:
    SongListView(int h, int w, int y, int x);
    
    void draw() override;
    void setSongs(const std::vector<std::string>& s,bool reset);
    void setTitle(const std::string& t) { title = t; }
    
    // Pagination
    void nextPage();
    void prevPage();
    void scrollUp();
    void scrollDown();
    
    // Getters
    int getSongAtY(int y) const;
    int getMaxVisibleSongs() const;
    int getCurrentPage() const { return currentPage; }
    int getTotalPages() const;
    
    // Check button clicks
    bool isPrevButtonClicked(int x, int y) const;
    bool isNextButtonClicked(int x, int y) const;
    
private:
    void calculateMaxVisibleLines();
};

#endif