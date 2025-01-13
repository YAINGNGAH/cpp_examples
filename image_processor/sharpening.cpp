#include "sharpening.h"

Sharp::Sharp() : MatrixFilter() {
}

Color Sharp::Pixel(Image &im, size_t x, size_t y) const {
    Color tmp(0, 0, 0);
    for (auto h : Sharp::Y) {
        for (auto w : Sharp::X) {
            int addh = h;
            int addw = w;
            if (x == 0 and w == -1) {
                addw = 0;
            }
            if (y == 0 and h == -1) {
                addh = 0;
            }
            if (x == im.GetWidth() - 1 and w == 1) {
                addw = 0;
            }
            if (y == im.GetHeight() - 1 and h == 1) {
                addh = 0;
            }
            tmp = tmp + im[y + addh][x + addw] * Sharp::Matrix[h + 1][w + 1];
        }
    }
    tmp.Normilize();
    return tmp;
}

void Sharp::Use(Image &im) {
    MatrixFilter::Use(im);
}