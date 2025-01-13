#pragma once
#include "image.h"
#include "matrixfilter.h"

#include <array>
class Sharp : MatrixFilter {
    constexpr static std::array<std::array<int, 3>, 3> Matrix{
        std::array<int, 3>{0, -1, 0}, std::array<int, 3>{-1, 5, -1}, std::array<int, 3>{0, -1, 0}};
    constexpr static std::array<int, 3> X{-1, 0, 1};
    constexpr static std::array<int, 3> Y{-1, 0, 1};

private:
    Color Pixel(Image& im, size_t x, size_t y) const override;

public:
    Sharp();
    void Use(Image& im) override;
};