#ifndef METADATACONTROLLER_H
#define METADATACONTROLLER_H

#include "../models/MediaFile.h"
#include "../views/MetadataView.h"

class MetadataController {
private:
    std::unique_ptr<MetadataView> metadataView;
    std::unique_ptr<MediaMetadata> metadata;
    MediaFile* sourceMediaFile;
    bool isEditing;
    std::string editBuffer;

public:
    MetadataController(int h, int w, int y, int x);

    void setSourceMediaFile(MediaFile* file) { sourceMediaFile = file; } 
    void setMetadata(MediaMetadata* meta);
    void setFilename(const std::string& name) {metadataView->setFilename(name); }
    void draw();

    bool handleClick(int x, int y);
    bool handleKey(int ch);
    EditField getFieldAtY(int y);
    bool saveToFile();

private:
    void startEditing(EditField field);
    void finishEditing(bool save);
    std::string getFieldValue(EditField field);
    void setFieldValue(EditField field, const std::string& value);
};

#endif
