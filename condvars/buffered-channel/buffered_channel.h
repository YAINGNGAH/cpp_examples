#pragma once

#include <utility>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>
#include <stdexcept>

template <class T>
class BufferedChannel {
private:
    size_t n_;
    std::queue<T> q_;

    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::mutex global_;
    std::mutex read_;
    std::atomic<bool> closed_;

public:
    explicit BufferedChannel(int size) : n_(size) {
        closed_ = false;
    }

    void Send(const T& value) {
        std::unique_lock<std::mutex> lock(global_);
        while (q_.size() == n_ && !closed_) {
            not_full_.wait(lock);
        }
        if (closed_) {
            throw std::runtime_error("Closed channel");
        }
        q_.push(value);
        not_empty_.notify_one();
    }

    std::optional<T> Recv() {
        std::unique_lock<std::mutex> lr(read_);
        std::unique_lock<std::mutex> lock(global_);
        while (q_.empty() && !closed_) {
            not_empty_.wait(lock);
        }
        if (closed_ && q_.empty()) {
            return std::nullopt;
        }
        auto v = q_.front();
        q_.pop();
        not_full_.notify_one();
        return v;
    }

    void Close() {
        std::unique_lock<std::mutex> locks(global_);
        closed_ = true;
        not_full_.notify_all();
        not_empty_.notify_all();
    }
};
