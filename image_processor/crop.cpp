#include "crop.h"

Crop::Crop(size_t w, size_t h) {
    w_ = w;
    h_ = h;
}

void Crop::Use(Image &im) {
    if (w_ >= im.GetWidth() and h_ >= im.GetHeight()) {
        return;
    }
    if (w_ >= im.GetWidth()) {
        w_ = im.GetWidth();
    }
    if (h_ >= im.GetHeight()) {
        h_ = im.GetHeight();
    }
    im.Resize(w_, h_);
}
