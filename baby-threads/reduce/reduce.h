#pragma once
#include <thread>
#include <iterator>
#include <vector>

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    unsigned int numthreads = std::min(std::thread::hardware_concurrency(), 128u);
    std::vector<std::thread> workers;
    T res[128];
    size_t size = std::distance(first, last);
    size_t r = (size + (numthreads - size % numthreads)) / numthreads;
    size_t correct = 0;
    for (size_t i = 0; i < size; i += r) {
        workers.emplace_back([&, i, correct]() {
            res[correct] = *(first + i);
            for (size_t j = i + 1; j < std::min(i + r, size); ++j) {
                res[correct] = func(res[correct], *(first + j));
            }
        });
        correct++;
    }
    for (auto& t : workers) {
        t.join();
    }
    auto cur_value(initial_value);
    for (size_t i = 0; i < correct; i++) {
        cur_value = func(cur_value, res[i]);
    }
    return cur_value;
}
