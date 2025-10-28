#ifndef METADATAVIEW_H
#define METADATAVIEW_H

#include "BaseView.h"
// #include "../models/MediaFile.h"

enum class EditField {
    NONE = -1,
    TITLE = 0,
    ARTIST,
    ALBUM,
    YEAR,
    GENRE,
    CODEC,
    // Thêm các field khác nếu cần
};
struct MetadataDisplayData {
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    std::string year;
    std::string codec;
    int duration = 0;
    int bitrate = 0;
    int sampleRate = 0;
    long size = 0;
    bool isAudio = false;
    bool isVideo = false;
};
class MetadataView : public BaseView {
private:
    std::string filename;
    EditField currentEditField;
    std::vector<std::pair<EditField, int>> fieldPositions; // Lưu vị trí y của mỗi field
    bool isEditing;
    std::string editBuffer;

    void drawField(int line, const char* label, const std::string& value, bool isEditable, bool isSelected);

public:
    MetadataView(int h, int w, int y, int x);
    void setFilename(const std::string& name) { filename = name; }
    void setCurrentEditField(EditField field) { 
        currentEditField = field; 
    }
    void setFieldPositions (const std::vector<std::pair<EditField, int>>& positions) {
        fieldPositions = positions;
    }
    void setIsEditing(bool editing) { isEditing = editing; }
    void setEditBuffer(const std::string& buffer) { editBuffer = buffer; }

    //getter 
    std::string getFilename() const { return filename; }
    EditField getCurrentEditField() const { return currentEditField; }
    std::vector<std::pair<EditField, int>> getFieldPositions() const { return fieldPositions; }
    bool getIsEditing() const { return isEditing; }
    std::string getEditBuffer() const { return editBuffer; }

    void draw() override;
    void draw(const MetadataDisplayData& data);

};

#endif