#pragma once

#include <atomic>

struct RWSpinLock {
private:
    std::atomic<int> state_;

public:
    RWSpinLock() : state_(0) {
    }
    void LockRead() {
        int prev = state_.load();
        do {
            prev = state_.load();
            while ((prev & 255) != 0) {
                prev = state_.load();
            }
        } while ((prev & 255) == 0 && !state_.compare_exchange_strong(prev, prev + (1 << 8)));
    }

    void UnlockRead() {
        state_.fetch_sub(1 << 8);
    }

    void LockWrite() {
        int prev = 0;
        do {
            prev = state_.load();
            while (prev != 0) {
                prev = state_.load();
            }
        } while (prev == 0 && !state_.compare_exchange_strong(prev, 1));
    }

    void UnlockWrite() {
        state_.store(0);
    }
};
