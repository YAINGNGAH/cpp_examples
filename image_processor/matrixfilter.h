#pragma once
#include "filter_template.h"

class MatrixFilter : Filter {
private:
    virtual Color Pixel(Image& im, size_t x, size_t y) const = 0;

public:
    MatrixFilter() = default;
    ~MatrixFilter() override = default;
    void Use(Image& im) override;
};