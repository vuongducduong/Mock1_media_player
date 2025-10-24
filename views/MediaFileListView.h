#ifndef MEDIAFILELISTVIEW_H
#define MEDIAFILELISTVIEW_H
#include "BaseView.h"

class MediaFileListView : public BaseView {
private:
    std::vector<std::string> mediafile;
    std::string title;
    int currentPage;
    int scrollOffsetInPage;
    int mediafilePerPage;
    int maxVisibleLines;

    bool reset=false;
    int currentPlayingIndex = -1;

public:
    MediaFileListView(int h, int w, int y, int x);
    
    void draw() override;
    void setMediaFile(const std::vector<std::string>& s,bool reset);
    void setTitle(const std::string& t) { title = t; }
    
    // Pagination
    void nextPage();
    void prevPage();
    void scrollUp();
    void scrollDown();
    
    // Getters
    int getSongAtY(int y) const;
    int getMaxVisiblemediafile() const;
    int getCurrentPage() const { return currentPage; }
    int getTotalPages() const;
    
    // Check button clicks
    bool isPrevButtonClicked(int x, int y) const;
    bool isNextButtonClicked(int x, int y) const;

    void setCurrentPlayingIndex(int index) { currentPlayingIndex = index; }
    
private:
    void calculateMaxVisibleLines();
};

#endif