#pragma once

constexpr int hash(const char *s, int p, int mod) {
    return ((*s) == '\0' ? 0 : ((*s) + static_cast<long long>(p) * hash(s + 1, p, mod) % mod)) %
           mod;
}
