#include "MetadataView.h"

MetadataView::MetadataView(int h, int w, int y, int x) 
    : BaseView(h, w, y, x),
        currentEditField(EditField::NONE),
        isEditing(false),                  
        editBuffer("") {}   
void MetadataView::drawField(int line, const char* label, const std::string& value, 
                              bool isEditable, bool isSelected) {
    if (isSelected && isEditing) {
        wattron(window, A_REVERSE);
        mvwprintw(window, line, 4, "%-12s [%s_]", label, editBuffer.c_str());
        wattroff(window, A_REVERSE);
    } else if (isEditable) {
        if (isSelected) {
            wattron(window, A_BOLD | A_UNDERLINE);
        }
        mvwprintw(window, line, 4, "%-12s %s", label, value.c_str());
        if (isSelected) {
            wattroff(window, A_BOLD | A_UNDERLINE);
        }
        mvwprintw(window, line, width - 10, "[EDIT]");
    } else {
        mvwprintw(window, line, 4, "%-12s %s", label, value.c_str());
    }
}
void MetadataView::draw(const MetadataDisplayData& data) {
    werase(window);
    box(window, 0, 0);
    mvwprintw(window, 1, 2, "Metadata for: %s", filename.c_str());

    int line = 3;
    fieldPositions.clear();

    if (!data.isAudio && !data.isVideo) {
        mvwprintw(window, line++, 4, "No metadata available");
        mvwprintw(window, height - 2, 2, "Press ESC to return.");
        wrefresh(window);
        return;
    }

    // Title
    fieldPositions.push_back({EditField::TITLE, line});
    drawField(line++, "Title:", data.title, true, currentEditField == EditField::TITLE);

    if (data.isAudio) {
        fieldPositions.push_back({EditField::ARTIST, line});
        drawField(line++, "Artist:", data.artist, true, currentEditField == EditField::ARTIST);

        fieldPositions.push_back({EditField::ALBUM, line});
        drawField(line++, "Album:", data.album, true, currentEditField == EditField::ALBUM);

        fieldPositions.push_back({EditField::GENRE, line});
        drawField(line++, "Genre:", data.genre, true, currentEditField == EditField::GENRE);

        fieldPositions.push_back({EditField::YEAR, line});
        drawField(line++, "Year:", data.year, true, currentEditField == EditField::YEAR);

        mvwprintw(window, line++, 4, "Length:      %d sec", data.duration);
        mvwprintw(window, line++, 4, "Bitrate:     %d kbps", data.bitrate);
        mvwprintw(window, line++, 4, "Sample rate: %d Hz", data.sampleRate);
    } 
    else if (data.isVideo) {
        fieldPositions.push_back({EditField::CODEC, line});
        drawField(line++, "Codec:", data.codec, true, currentEditField == EditField::CODEC);

        mvwprintw(window, line++, 4, "Length:      %d sec", data.duration);
        mvwprintw(window, line++, 4, "Bitrate:     %d kbps", data.bitrate);
        mvwprintw(window, line++, 4, "Size:        %ld bytes", data.size);
    }

    if (isEditing) {
        mvwprintw(window, height - 3, 2, "Editing: Type to modify, ENTER to save, ESC to cancel");
    } else {
        mvwprintw(window, height - 3, 2, "Click on highlighted fields to edit, or press TAB to cycle");
    }

    mvwprintw(window, height - 2, 2, "Press ESC to return | Changes are saved automatically");
    wrefresh(window);
}
void MetadataView::draw() {
    // Nếu bạn muốn, có thể để trống hoặc hiển thị message mặc định
    werase(window);
    box(window, 0, 0);
    mvwprintw(window, 1, 2, "No metadata loaded yet.");
    wrefresh(window);
}
