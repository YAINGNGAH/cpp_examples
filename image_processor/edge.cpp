#include "edge.h"
#include "grayscale.h"
#include <cmath>

Edge::Edge(double threshold) : MatrixFilter() {
    threshold_ = static_cast<int>(round(threshold * static_cast<double>(MAX_RGB)));
}

Color Edge::Pixel(Image &im, size_t x, size_t y) const {
    Color tmp(0, 0, 0);
    for (auto h : Edge::Y) {
        for (auto w : Edge::X) {
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
            tmp = tmp + im[y + addh][x + addw] * Edge::Matrix[h + 1][w + 1];
        }
    }
    if (tmp.r > threshold_) {
        tmp = Color(MAX_RGB, MAX_RGB, MAX_RGB);
    } else {
        tmp = Color();
    }
    return tmp;
}

void Edge::Use(Image &im) {
    Grayscale gs{};
    gs.Use(im);
    MatrixFilter::Use(im);
}