#pragma once

#include <memory>
#include <atomic>

template <class T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(size_t size) {
        max_size_ = 1;
        while (max_size_ < size){
            max_size_*=2;
        }
        array_ = std::unique_ptr<Element>(new Element[max_size_]);
    }

    bool Enqueue(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() == max_size_) {
            return false;
        }
        queue_.push(value);
        return true;
    }

    bool Dequeue(T& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        data = std::move(queue_.front());
        queue_.pop();
        return true;
    }

private:
    struct Element{
        T value_;
        std::atomic<size_t> generation_;
    };
    size_t max_size_ = 0;
    std::unique_ptr<Element> array_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;


};
