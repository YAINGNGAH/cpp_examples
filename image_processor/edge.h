#pragma once
#include "image.h"
#include "matrixfilter.h"
#include <array>

class Edge : MatrixFilter {
    constexpr static std::array<std::array<int, 3>, 3> Matrix{
        std::array<int, 3>{0, -1, 0}, std::array<int, 3>{-1, 4, -1}, std::array<int, 3>{0, -1, 0}};
    constexpr static std::array<int, 3> X{-1, 0, 1};
    constexpr static std::array<int, 3> Y{-1, 0, 1};

private:
    int threshold_;
    Color Pixel(Image& im, size_t x, size_t y) const override;

public:
    explicit Edge(double threshold);
    void Use(Image& im) override;
};
