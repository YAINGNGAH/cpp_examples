#pragma once

constexpr bool is_prime(int x) {
    if (x <= 1) {
        return false;
    }
    for (int i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
}

constexpr int next_prime(int x) {
    if (x <= 1) {
        return 2;
    }
    while (!is_prime(x)) {
        x++;
    }
    return x;
}
