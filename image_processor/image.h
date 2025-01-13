#pragma once
#include "color.h"
#include <vector>
#include <cstdint>
#include <cstddef>

const int HEADERSIZE = 54;

using WORD = uint16_t;
using DWORD = uint32_t;
using LONG = int32_t;

struct __attribute__((packed)) BITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};

struct __attribute__((packed)) BITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};

class Image {
private:
    std::vector<std::vector<Color>> im_;
    BITMAPFILEHEADER file_header_;
    BITMAPINFOHEADER info_header_;
    size_t rw_;

public:
    explicit Image(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, unsigned char* iz_rgb);
    std::vector<Color>& operator[](size_t i);
    void Resize(size_t w, size_t h);
    const size_t GetWidth() const;
    const size_t GetHeight() const;
    friend void SaveFile(const Image& im, const char* file);
};