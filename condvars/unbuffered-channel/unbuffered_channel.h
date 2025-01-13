#pragma once

#include <utility>
#include <optional>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <condition_variable>
#include <memory>
#include <iostream>

template <class T>
class UnbufferedChannel {
private:
    std::atomic<bool> closed_ = false;
    std::condition_variable empty_;
    std::condition_variable full_;
    std::condition_variable writing_;
    std::mutex global_;
    std::mutex read_;
    std::mutex write_;
    T v_;
    std::atomic<int> state_ = 0;

public:
    void Send(const T& value) {
        std::unique_lock<std::mutex> lock(global_);
        while ((state_ != 0) && !closed_) {
            empty_.wait(lock);
        }
        if (closed_) {
            throw std::runtime_error("Closed channel");
        }
        v_ = value;
        state_ = 1;
        full_.notify_one();
        while ((state_ != 0) && !closed_) {
            empty_.wait(lock);
        }
        empty_.notify_one();
    }

    std::optional<T> Recv() {
        std::unique_lock<std::mutex> lock(global_);
        while ((state_ != 1) && !closed_) {
            full_.wait(lock);
        }
        if (closed_ && state_ == 0) {
            return std::nullopt;
        }
        state_ = 0;
        empty_.notify_one();
        return v_;
    }

    void Close() {
        std::unique_lock<std::mutex> locks(global_);
        closed_ = true;
        empty_.notify_all();
        full_.notify_all();
        writing_.notify_all();
    }
};
