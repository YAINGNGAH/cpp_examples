#pragma once

template <unsigned a, unsigned b>
struct Pow {
    static const unsigned value =
        (b % 2 == 0 ? (Pow<a, b / 2>::value * Pow<a, b / 2>::value) : a * Pow<a, b - 1>::value);
};

template <unsigned a>
struct Pow<a, 0> {
    static const unsigned value = 1;
};
