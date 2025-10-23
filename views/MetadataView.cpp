#include "MetadataView.h"

MetadataView::MetadataView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x) {}

void MetadataView::draw() {
    werase(window);
    box(window, 0, 0);
    
    mvwprintw(window, 1, 2, "Metadata for: %s", filename.c_str());
    
    int line = 3;
    mvwprintw(window, line++, 4, "Title:  %s", metadata.title.c_str());
    mvwprintw(window, line++, 4, "Artist: %s", metadata.artist.c_str());
    mvwprintw(window, line++, 4, "Album:  %s", metadata.album.c_str());
    mvwprintw(window, line++, 4, "Year:   %d", metadata.year);
    mvwprintw(window, line++, 4, "Length: %d sec", metadata.duration);
    mvwprintw(window, line++, 4, "Bitrate: %d kbps", metadata.bitrate);
    mvwprintw(window, line++, 4, "Sample rate: %d Hz", metadata.sampleRate);
    
    mvwprintw(window, height - 2, 2, "Press ESC to return.");
    
    wrefresh(window);
}
