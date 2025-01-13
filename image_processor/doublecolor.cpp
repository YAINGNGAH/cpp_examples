#include "doublecolor.h"
#include <algorithm>
#include <cmath>

DoubleColor& DoubleColor::operator=(const DoubleColor& other) {
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
}

DoubleColor::DoubleColor() {
    r = 0;
    g = 0;
    b = 0;
}

DoubleColor DoubleColor::operator+(const DoubleColor& other) const {
    DoubleColor sum;
    sum.r = other.r + r;
    sum.g = other.g + g;
    sum.b = other.b + b;
    return sum;
}
DoubleColor DoubleColor::operator-(const DoubleColor& other) const {
    DoubleColor sum;
    sum.r = r - other.r;
    sum.g = g - other.g;
    sum.b = b - other.g;
    return sum;
}
void DoubleColor::Normilize() {
    r = std::max(std::min(MAXDOUBLECOLOR, r), 0.);
    g = std::max(std::min(MAXDOUBLECOLOR, g), 0.);
    b = std::max(std::min(MAXDOUBLECOLOR, b), 0.);
}

DoubleColor& DoubleColor::operator+=(const DoubleColor& other) {
    r += other.r;
    g += other.g;
    b += other.b;
    return *this;
}

DoubleColor::DoubleColor(unsigned char rc, unsigned char gc, unsigned char bc) {
    r = static_cast<double>(rc) / static_cast<double>(MAX_RGB);
    g = static_cast<double>(gc) / static_cast<double>(MAX_RGB);
    b = static_cast<double>(bc) / static_cast<double>(MAX_RGB);
}

DoubleColor DoubleColor::operator*(double other) const {
    DoubleColor m;
    m.r = r * other;
    m.g = g * other;
    m.b = b * other;
    return m;
}