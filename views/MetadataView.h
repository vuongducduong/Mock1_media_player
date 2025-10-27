#ifndef METADATAVIEW_H
#define METADATAVIEW_H

#include "BaseView.h"
#include "../models/MediaFile.h"

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

class MetadataView : public BaseView {
private:
    MediaFile* sourceMediaFile; 
    std::unique_ptr<MediaMetadata> metadata;
    std::string filename;
    EditField currentEditField;
    std::vector<std::pair<EditField, int>> fieldPositions; // Lưu vị trí y của mỗi field
    bool isEditing;
    std::string editBuffer;

    void drawField(int line, const char* label, const std::string& value, bool isEditable, bool isSelected);
    void startEditing(EditField field);
    void finishEditing(bool save);
    std::string getFieldValue(EditField field);
    void setFieldValue(EditField field, const std::string& value);

public:
    MetadataView(int h, int w, int y, int x);
    
    void draw() override;
    void setMetadata(MediaMetadata* meta);
    void setFilename(const std::string& name) { filename = name; }

    bool handleClick(int x, int y);
    bool handleKey(int ch);
    
    EditField getFieldAtY(int y);
    
    MediaMetadata* getModifiedMetadata() { return metadata.get(); }
    bool hasUnsavedChanges() const { return isEditing; }
        void setSourceMediaFile(MediaFile* file) { sourceMediaFile = file; }  // THÊM
    bool saveToFile();  // THÊM
};

#endif