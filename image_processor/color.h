#pragma once
#include <tuple>
#include <cstdint>

const int MAX_RGB = 255;

struct Color {
public:
    int r;
    int g;
    int b;
    explicit Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b){};
    Color();
    Color& operator=(const Color& other);
    Color& operator+=(const Color& other);
    Color operator+(const Color& other) const;
    Color operator-(const Color& other) const;
    Color operator*(int other) const;
    bool operator==(const Color& other) const;
    void Normilize();
};
