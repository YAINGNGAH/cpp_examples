#include "is_prime.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>

void CheckPrime(const uint64_t& x, uint64_t i, uint64_t j, std::atomic_bool& check) {
    for (auto k = i; k < j; ++k) {
        if (x % k == 0) {
            check = false;
        }
    }
}

bool IsPrime(uint64_t x) {
    unsigned int numthreads = std::thread::hardware_concurrency();
    std::atomic_bool check = true;
    std::vector<std::thread> workers;
    if (x <= 1) {
        return false;
    }
    uint64_t root = sqrt(x);
    unsigned long long bound = std::min(root + 6, x);
    unsigned long long r = (bound + (numthreads - bound % numthreads)) / numthreads;
    for (auto i = 2ull; i < bound; i += r) {
        workers.emplace_back(CheckPrime, std::cref(x), i, std::min(i + r, bound), std::ref(check));
    }
    for (auto& t : workers) {
        t.join();
    }
    return check;
}
