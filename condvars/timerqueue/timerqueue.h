#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

private:
    std::mutex m_;
    std::multimap<TimePoint, T> q_;
    std::condition_variable time_passed_;

public:
    void Add(const T& item, TimePoint at) {
        std::unique_lock<std::mutex> guard(m_);
        q_.insert({at, item});
        time_passed_.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> guard(m_);
        if (q_.empty()) {
            time_passed_.wait(guard, [&]() -> bool { return !q_.empty(); });
        }
        time_passed_.wait_until(guard, q_.begin()->first,
                                [&]() -> bool { return Clock::now() >= q_.begin()->first; });
        auto v = q_.begin()->second;
        q_.erase(q_.begin());
        return v;
    }
};
