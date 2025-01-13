#include "grayscale.h"
#include <cmath>
void Grayscale::Use(Image &im) {
    double const red = 0.299;
    double const blue = 0.114;
    double const green = 0.587;
    for (size_t i = 0; i < im.GetHeight(); ++i) {
        for (size_t j = 0; j < im.GetWidth(); ++j) {
            im[i][j].b = im[i][j].g = im[i][j].r = static_cast<int>(
                std::round(red * static_cast<double>(im[i][j].r) + std::round(green * static_cast<double>(im[i][j].g)) +
                           std::round(blue * static_cast<double>(im[i][j].b))));
            im[i][j].Normilize();
        }
    }
}