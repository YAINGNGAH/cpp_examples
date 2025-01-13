#include "find_subsets.h"
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <iostream>
#include <ctime>

int64_t Pow(int64_t a, int64_t b) {
    if (b == 0) {
        return 1;
    }
    if (b % 2 == 0) {
        int64_t r = Pow(a, b / 2);
        return r * r;
    } else {
        return Pow(a, b - 1) * a;
    }
}

Subsets FindEqualSumSubsets(const std::vector<int64_t>& data) {
    std::unordered_map<int64_t, int64_t> sets;
    std::vector<int64_t> sums;
    int64_t sn = data.size() / 2.1;
    int64_t fn = data.size() - sn;

    if (sn != 0) {
        unsigned int numthreads = std::thread::hardware_concurrency();
        std::vector<std::thread> workers;
        std::mutex writelock;
        workers.emplace_back([&]() {
            auto end = Pow(3, sn);
            int64_t mask;
            int64_t sum = 0;
            int64_t key;
            sets.reserve(end);

            for (int64_t i = 0; i < end; ++i) {
                mask = i;
                sum = 0;
                for (int64_t j = 0; j < sn && mask != 0; ++j) {
                    key = mask % 3;
                    if (key == 1) {
                        sum += data[j];
                    } else if (key == 2) {
                        sum -= data[j];
                    }
                    mask /= 3;
                }
                sets[sum] = i;
            }
        });
        workers.emplace_back([&]() {
            auto end = Pow(3, fn);
            sums.reserve(end);
            int64_t mask;
            int64_t sum;
            int64_t key;
            for (int64_t i = 0; i < end; ++i) {
                mask = i;
                sum = 0;
                for (int64_t j = 0; j < fn && mask != 0; ++j) {
                    key = mask % 3;
                    if (key == 1) {
                        sum += data[sn + j];
                    } else if (key == 2) {
                        sum -= data[sn + j];
                    }
                    mask /= 3;
                }
                sums.push_back(sum);
            }
        });
        for (auto& t : workers) {
            t.join();
        }
        workers.clear();
        auto end = sums.size() + numthreads - sums.size() % numthreads;
        auto step = end / numthreads;
        std::atomic_bool check = false;
        int64_t first = -1;
        int64_t second = -1;
        for (size_t t = 0; t < end; t += step) {
            workers.emplace_back([&, t]() {
                for (size_t i = t; i < std::min(t + step, sums.size()); ++i) {
                    if (check) {
                        return;
                    }
                    if (sets.contains(-sums[i])) {
                        auto cur = sets[-sums[i]];
                        bool check1 = false;
                        bool check2 = false;
                        auto mask = cur;
                        for (int64_t j = 0; j < sn && mask != 0; ++j) {
                            auto k = mask % 3;
                            if (k == 1) {
                                check1 = true;
                            } else if (k == 2) {
                                check2 = true;
                            }
                            mask /= 3;
                        }
                        mask = i;
                        for (int64_t j = 0; j < fn && mask != 0; ++j) {
                            auto k = mask % 3;
                            if (k == 1) {
                                check1 = true;
                            } else if (k == 2) {
                                check2 = true;
                            }
                            mask /= 3;
                        }
                        if (check1 && check2) {
                            writelock.lock();
                            first = cur;
                            second = i;
                            check = true;
                            writelock.unlock();
                            return;
                        }
                    }
                }
            });
        }
        for (auto& t : workers) {
            t.join();
        }
        Subsets ans;
        if (first == -1 || second == -1) {
            ans.exists = false;
        } else {
            ans.exists = true;
            int64_t mask1 = first;
            for (int64_t j = 0; j < sn && mask1 != 0; ++j) {
                int64_t key = mask1 % 3;
                if (key == 1) {
                    ans.first_indices.push_back(j);
                } else if (key == 2) {
                    ans.second_indices.push_back(j);
                }
                mask1 /= 3;
            }
            int64_t mask2 = second;
            for (int64_t j = 0; j < fn && mask2 != 0; ++j) {
                int64_t key = mask2 % 3;
                if (key == 1) {
                    ans.first_indices.push_back(sn + j);
                } else if (key == 2) {
                    ans.second_indices.push_back(sn + j);
                }
                mask2 /= 3;
            }
        }
        return ans;
    }
    Subsets ans;
    ans.exists = false;
    return ans;
}
