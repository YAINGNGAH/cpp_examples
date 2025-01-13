#pragma once

constexpr int pow(int a, int b) {
    return b == 0 ? (1) : (b % 2 == 0 ? pow(a, b / 2) * pow(a, b / 2) : a * pow(a, b - 1));
}
