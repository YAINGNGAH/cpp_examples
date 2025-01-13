#include "gauss.h"
#include "doublecolor.h"
#include <vector>
#include <cmath>
#include <numbers>
#include <cstdlib>

DoubleColor Dublefy(const Color& c) {
    DoubleColor dc;
    dc.r = static_cast<double>(c.r) / static_cast<double>(MAX_RGB);
    dc.g = static_cast<double>(c.g) / static_cast<double>(MAX_RGB);
    dc.b = static_cast<double>(c.b) / static_cast<double>(MAX_RGB);
    return dc;
}

Color Intify(const DoubleColor& dc) {
    Color c;
    c.r = static_cast<int>(round(dc.r * static_cast<double>(MAX_RGB)));
    c.g = static_cast<int>(round(dc.g * static_cast<double>(MAX_RGB)));
    c.b = static_cast<int>(round(dc.b * static_cast<double>(MAX_RGB)));
    return c;
}
Blur::Blur(double sigma) {
    sigma_ = std::abs(sigma);
}

void Blur::Use(Image& im) {
    std::vector<std::vector<DoubleColor>> tmp;
    tmp.resize(im.GetHeight(), std::vector<DoubleColor>(im.GetWidth()));
    std::vector<double> gquef;
    gquef.resize(static_cast<size_t>(ceil(KERLSIZE * sigma_) + 1));
    for (size_t i = 0; i < gquef.size(); ++i) {
        gquef[i] = std::exp(-static_cast<double>(i * i) / (sigma_ * sigma_) * HALF) /
                   sqrt(2 * std::numbers::pi * (sigma_ * sigma_));
    }
    for (int64_t y = 0; y < static_cast<int64_t>(im.GetHeight()); ++y) {
        for (int64_t x = 0; x < static_cast<int64_t>(im.GetWidth()); ++x) {
            for (int64_t dx = -static_cast<int64_t>(gquef.size()) + 1; dx < static_cast<int64_t>(gquef.size()); ++dx) {
                if (x + dx < 0) {
                    tmp[y][x] += Dublefy(im[y][0]) * gquef[std::abs(dx)];
                    continue;
                } else if (x + dx >= static_cast<int64_t>(im.GetWidth())) {
                    tmp[y][x] += Dublefy(im[y][im.GetWidth() - 1]) * gquef[std::abs(dx)];
                    continue;
                }
                tmp[y][x] += Dublefy(im[y][x + dx]) * gquef[std::abs(dx)];
            }
        }
    }
    std::vector<std::vector<DoubleColor>> tmpy;
    tmpy.resize(im.GetHeight(), std::vector<DoubleColor>(im.GetWidth()));
    for (int64_t y = 0; y < static_cast<int64_t>(im.GetHeight()); ++y) {
        for (int64_t x = 0; x < static_cast<int64_t>(im.GetWidth()); ++x) {
            for (int64_t dy = -static_cast<int64_t>(gquef.size()) + 1; dy < static_cast<int64_t>(gquef.size()); ++dy) {
                if (y + dy < 0) {
                    tmpy[y][x] += tmp[0][x] * gquef[std::abs(dy)];
                    continue;
                } else if (y + dy >= static_cast<int64_t>(im.GetHeight())) {
                    tmpy[y][x] += tmp[im.GetHeight() - 1][x] * gquef[std::abs(dy)];
                    continue;
                }
                tmpy[y][x] += tmp[y + dy][x] * gquef[std::abs(dy)];
            }
            tmpy[y][x].Normilize();
        }
    }
    tmp.clear();
    for (size_t i = 0; i < im.GetHeight(); ++i) {
        for (size_t j = 0; j < im.GetWidth(); ++j) {
            im[i][j] = Intify(tmpy[i][j]);
        }
    }
}
