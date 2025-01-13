#include "median.h"
#include <algorithm>

Median::Median(int r) : MatrixFilter() {
    r_ = r;
}

Color Median::Pixel(Image &im, size_t x, size_t y) const {
    std::vector<int> r;
    std::vector<int> g;
    std::vector<int> b;
    for (int h = -r_ + 1; h < r_; ++h) {
        for (int w = -r_ + 1; w < r_; ++w) {
            int addh = h;
            int addw = w;
            if (w < 0 and static_cast<int>(x) + w < 0) {
                addw = -static_cast<int>(x);
            }
            if (h < 0 and static_cast<int>(y) + h < 0) {
                addh = -static_cast<int>(y);
            }
            if (w > 0 and x + w > im.GetWidth() - 1) {
                addw = static_cast<int>(im.GetWidth()) - 1 - static_cast<int>(x);
            }
            if (h > 0 and y + h > im.GetHeight() - 1) {
                addh = static_cast<int>(im.GetHeight()) - 1 - static_cast<int>(y);
            }
            r.push_back(im[y + addh][x + addw].r);
            g.push_back(im[y + addh][x + addw].g);
            b.push_back(im[y + addh][x + addw].b);
        }
    }
    std::nth_element(r.begin(), r.begin() + static_cast<int64_t>(r.size() / 2), r.end());
    std::nth_element(g.begin(), g.begin() + static_cast<int64_t>(g.size() / 2), g.end());
    std::nth_element(b.begin(), b.begin() + static_cast<int64_t>(b.size() / 2), b.end());
    Color tmp;
    tmp.r = r[r.size() / 2];
    tmp.g = g[g.size() / 2];
    tmp.b = b[b.size() / 2];
    return tmp;
}

void Median::Use(Image &im) {
    MatrixFilter::Use(im);
}
