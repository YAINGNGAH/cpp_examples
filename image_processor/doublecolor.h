#pragma once

#include "color.h"
#include <tuple>
#include <cstdint>
#include <valarray>

const double MAXDOUBLECOLOR = 1.l;

struct DoubleColor {
public:
    double r;
    double g;
    double b;
    explicit DoubleColor(unsigned char rc, unsigned char gc, unsigned char bc);
    DoubleColor();
    DoubleColor& operator=(const DoubleColor& other);
    DoubleColor& operator+=(const DoubleColor& other);
    DoubleColor operator+(const DoubleColor& other) const;
    DoubleColor operator-(const DoubleColor& other) const;
    DoubleColor operator*(double other) const;
    void Normilize();
};
