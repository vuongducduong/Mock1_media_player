#ifndef METADATAVIEW_H
#define METADATAVIEW_H

#include "BaseView.h"
#include "../models/Song.h"

class MetadataView : public BaseView {
private:
    SongMetadata metadata;
    std::string filename;

public:
    MetadataView(int h, int w, int y, int x);
    
    void draw() override;
    void setMetadata(const SongMetadata& meta) { metadata = meta; }
    void setFilename(const std::string& name) { filename = name; }
};

#endif