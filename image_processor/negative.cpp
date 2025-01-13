#include "negative.h"

void Negative::Use(Image &im) {
    for (size_t i = 0; i < im.GetHeight(); ++i) {
        for (size_t j = 0; j < im.GetWidth(); ++j) {
            im[i][j] = Color(MAX_RGB, MAX_RGB, MAX_RGB) - im[i][j];
        }
    }
}
