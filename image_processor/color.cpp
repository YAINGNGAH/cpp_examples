#include "color.h"
#include <algorithm>
#include <cmath>

Color& Color::operator=(const Color& other) {
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
}

Color::Color() {
    r = 0;
    g = 0;
    b = 0;
}

Color Color::operator+(const Color& other) const {
    Color sum;
    sum.r = other.r + r;
    sum.g = other.g + g;
    sum.b = other.b + b;
    return sum;
}
Color Color::operator-(const Color& other) const {
    Color sum;
    sum.r = r - other.r;
    sum.g = g - other.g;
    sum.b = b - other.g;
    return sum;
}
void Color::Normilize() {
    r = std::max(std::min(MAX_RGB, r), 0);
    g = std::max(std::min(MAX_RGB, g), 0);
    b = std::max(std::min(MAX_RGB, b), 0);
}

Color Color::operator*(int other) const {
    Color m;
    m.r = r * other;
    m.g = g * other;
    m.b = b * other;
    return m;
}

Color& Color::operator+=(const Color& other) {
    r += other.r;
    g += other.g;
    b += other.b;
    return *this;
}

bool Color::operator==(const Color& other) const {
    return (r == other.r) && (g == other.g) && (b == other.b);
}
