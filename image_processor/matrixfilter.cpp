#include "matrixfilter.h"

void MatrixFilter::Use(Image &im) {
    std::vector<std::vector<Color>> tmp;
    tmp.resize(im.GetHeight(), std::vector<Color>(im.GetWidth()));
    for (size_t i = 0; i < im.GetHeight(); ++i) {
        for (size_t j = 0; j < im.GetWidth(); ++j) {
            tmp[i][j] = Pixel(im, j, i);
        }
    }
    for (size_t i = 0; i < im.GetHeight(); ++i) {
        for (size_t j = 0; j < im.GetWidth(); ++j) {
            im[i][j] = tmp[i][j];
        }
    }
}