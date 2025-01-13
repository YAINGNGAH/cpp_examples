#include "image.h"

Image::Image(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, unsigned char* iz_rgb) {
    file_header_ = file_header;
    info_header_ = info_header;
    rw_ = info_header_.biWidth * 3 + (4 - info_header_.biWidth * 3 % 4) % 4;
    im_.resize(info_header_.biHeight, std::vector<Color>(info_header_.biWidth));
    for (size_t i = 0; i < static_cast<size_t>(info_header_.biHeight); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(info_header_.biWidth); ++j) {
            im_[static_cast<size_t>(info_header_.biHeight) - i - 1][j] =
                Color(iz_rgb[i * rw_ + 3 * j + 2], iz_rgb[i * rw_ + 3 * j + 1], iz_rgb[i * rw_ + 3 * j]);
        }
    }
    delete[] iz_rgb;
}

std::vector<Color>& Image::operator[](size_t i) {
    return im_[i];
}
const size_t Image::GetWidth() const {
    return info_header_.biWidth;
}
const size_t Image::GetHeight() const {
    return info_header_.biHeight;
}
void Image::Resize(size_t w, size_t h) {
    im_.resize(h);
    for (size_t i = 0; i < h; ++i) {
        im_[i].resize(w);
    }
    info_header_.biWidth = static_cast<LONG>(w);
    info_header_.biHeight = static_cast<LONG>(h);
    rw_ = info_header_.biWidth * 3 + (4 - info_header_.biWidth * 3 % 4) % 4;
    info_header_.biSizeImage = rw_ * info_header_.biHeight;
    file_header_.bfSize = info_header_.biSizeImage + HEADERSIZE;
}
