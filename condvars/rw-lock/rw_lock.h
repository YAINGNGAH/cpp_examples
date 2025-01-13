#pragma once
#include <mutex>
#include <condition_variable>
class RWLock {
public:
    template <class Func>
    void Read(Func func) {
        global_.lock();
        ++blocked_readers_;
        global_.unlock();
        try {
            func();
        } catch (...) {
            EndRead();
            throw;
        }
        EndRead();
    }

    template <class Func>
    void Write(Func func) {
        std::unique_lock<std::mutex> l(write_);
        std::unique_lock<std::mutex> lock(global_);
        while (blocked_readers_ != 0) {
            no_readers_.wait(lock);
        }
        func();
    }

private:
    std::mutex write_;
    std::mutex global_;
    std::condition_variable no_readers_;
    int blocked_readers_ = 0;

    void EndRead() {
        global_.lock();
        --blocked_readers_;
        if (blocked_readers_ == 0) {
            no_readers_.notify_one();
        }
        global_.unlock();
    }
};
