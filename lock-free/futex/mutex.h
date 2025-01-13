#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>

// Взял решение из статьи, указанной в условии,
// https://dept-info.labri.u-bordeaux.fr/~denis/Enseignement/2008-IR/Articles/01-futex.pdf, с
// использованием атомиков.

// Atomically do the following:
//    if (*value == expected_value) {
//        sleep_on_address(value)
//    }
void FutexWait(int* value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expected_value, nullptr, nullptr, 0);
}

// Wakeup 'count' threads sleeping on address of value(-1 wakes all)
void FutexWake(int* value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

int Cmpxchg(std::atomic<int>* ptr, int exp, int val) {
    int* e = &exp;
    ptr->compare_exchange_strong(*e, val);
    return *e;
}
class Mutex {
public:
    Mutex() : state_(0) {
    }
    void Lock() {
        int c;
        if ((c = Cmpxchg(&state_, 0, 1)) != 0) {
            do {
                if (c == 2 || Cmpxchg(&state_, 1, 2) != 0) {
                    FutexWait(reinterpret_cast<int*>(&state_), 2);
                }
            } while ((c = Cmpxchg(&state_, 0, 2)) != 0);
        }
    }

    void Unlock() {
        if (state_.fetch_sub(1) != 1) {
            state_.store(0);
            FutexWake(reinterpret_cast<int*>(&state_), 1);
        }
    }

private:
    std::atomic<int> state_;
};
