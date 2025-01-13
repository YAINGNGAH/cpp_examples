#pragma once
#include "matrixfilter.h"
#include "image.h"

class Median : MatrixFilter {
private:
    int r_;
    Color Pixel(Image& im, size_t x, size_t y) const override;

public:
    explicit Median(int r);
    void Use(Image& im) override;
};