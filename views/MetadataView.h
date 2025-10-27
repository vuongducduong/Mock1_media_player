#ifndef METADATAVIEW_H
#define METADATAVIEW_H

#include "BaseView.h"
#include "../models/MediaFile.h"

class MetadataView : public BaseView {
private:
    std::unique_ptr<MediaMetadata> metadata;
    std::string filename;

public:
    MetadataView(int h, int w, int y, int x);
    
    void draw() override;
    void setMetadata(MediaMetadata* meta);
    void setFilename(const std::string& name) { filename = name; }
};

#endif