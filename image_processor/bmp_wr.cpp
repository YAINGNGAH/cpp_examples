#include "bmp_wr.h"
#include <iostream>
#include <fstream>

Image ReadFile(const char *file) {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    unsigned char *iz_rgb = nullptr;
    std::ifstream stream{file, std::ios_base::binary};
    if (!stream) {
        stream.close();
        throw std::runtime_error("Wrong path!");
    }
    stream.read(reinterpret_cast<char *>(&file_header), sizeof(file_header));
    if (file_header.bfType != BMPTYPE1) {
        stream.close();
        throw std::runtime_error("Wrong file format! Not a bmp!");
    }
    if (file_header.bfReserved1 != 0 || file_header.bfReserved2 != 0) {
        stream.close();
        ;
        throw std::runtime_error("Wrong file format! Not a bmp!");
    }
    stream.read(reinterpret_cast<char *>(&info_header), sizeof(info_header));
    if (info_header.biSize != INFOSIZE) {
        stream.close();
        throw std::runtime_error("Wrong file format! Buggy size of InfoHeader!");
    }
    if (info_header.biHeight < 0 || info_header.biWidth < 0) {
        stream.close();
        throw std::runtime_error("Wrong file format! Minus height or width!");
    }
    if (info_header.biPlanes != 1) {
        stream.close();
        throw std::runtime_error("Wrong file format! Not a bmp!");
    }
    if (info_header.biCompression != 0) {
        stream.close();
        throw std::runtime_error("Wrong file format! Has a compression!");
    }
    if (info_header.biBitCount != BITCOUNT) {
        stream.close();
        throw std::runtime_error("Wrong file format!Not 24bit bmp!");
    }
    if (info_header.biClrUsed != 0) {
        stream.close();
        throw std::runtime_error("Wrong file format! Crl used!");
    }
    int64_t rw = static_cast<int64_t>(info_header.biWidth * 3 + (4 - info_header.biWidth * 3 % 4) % 4);
    iz_rgb = new unsigned char[info_header.biHeight * static_cast<LONG>(rw)];
    stream.seekg(file_header.bfOffBits, stream.beg);
    stream.read(reinterpret_cast<char *>(iz_rgb), info_header.biHeight * static_cast<LONG>(rw));
    stream.close();
    return Image(file_header, info_header, iz_rgb);
}

void SaveFile(const Image &im, const char *file) {
    std::ofstream out(file, std::ios_base::binary);
    if (!out) {
        out.close();
        throw std::runtime_error("Wrong path!");
    }
    out.write(reinterpret_cast<const char *>(&im.file_header_), sizeof(BITMAPFILEHEADER));
    out.write(reinterpret_cast<const char *>(&im.info_header_), sizeof(BITMAPINFOHEADER));
    unsigned char *iz_rgb = nullptr;
    iz_rgb = new unsigned char[im.info_header_.biHeight * static_cast<LONG>(im.rw_)];
    for (size_t i = 0; i < static_cast<size_t>(im.info_header_.biHeight * static_cast<LONG>(im.rw_)); ++i) {
        iz_rgb[i] = 0;
    }
    for (size_t i = 0; i < static_cast<size_t>(im.info_header_.biHeight); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(im.info_header_.biWidth); ++j) {
            iz_rgb[i * im.rw_ + 3 * j + 0] = im.im_[static_cast<size_t>(im.info_header_.biHeight) - 1 - i][j].b;
            iz_rgb[i * im.rw_ + 3 * j + 1] = im.im_[static_cast<size_t>(im.info_header_.biHeight) - 1 - i][j].g;
            iz_rgb[i * im.rw_ + 3 * j + 2] = im.im_[static_cast<size_t>(im.info_header_.biHeight) - 1 - i][j].r;
        }
    }
    out.write(reinterpret_cast<const char *>(iz_rgb), im.info_header_.biHeight * static_cast<LONG>(im.rw_));
    delete[] iz_rgb;
    out.close();
}