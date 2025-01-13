#pragma once

#include <memory>
#include <list>
#include <mutex>
#include <functional>
#include <stdexcept>
#include <atomic>
template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher) {
    }

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher) {
    }

    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash())
        : hasher_(hasher) {
        if (expected_threads_count < 8) {
            expected_threads_count = 8;
        }
        if (expected_threads_count < expected_size) {
            expected_size += expected_threads_count - expected_size % expected_threads_count;
        } else {
            expected_size = expected_threads_count;
        }
        reserve_ = expected_size;
        table_.reset(new std::list<std::pair<K, V>>[expected_size]);
        locks_.reset(new std::mutex[expected_threads_count]);
        size_ = 0;
        exthreads_ = expected_threads_count;
    }
    void Rehash() {
        if (static_cast<double>(size_) / reserve_ <= kThreshold) {
            return;
        }
        if (reserve_ > SIZE_MAX / kIncrease) {
            return;
        }
        for (size_t i = 0; i < exthreads_; ++i) {
            locks_[i].lock();
        }
        if (static_cast<double>(size_) / reserve_ <= kThreshold) {
            for (size_t i = exthreads_ - 1; i != SIZE_MAX; --i) {
                locks_[i].unlock();
            }
            return;
        }
        size_t newreserve = kIncrease * reserve_;
        std::unique_ptr<std::list<std::pair<K, V>>[]> newtable(
            new std::list<std::pair<K, V>>[newreserve]);
        for (size_t i = 0; i < reserve_; ++i) {
            for (auto j : table_[i]) {
                auto ind = hasher_(j.first) % (newreserve);
                newtable[ind].push_front(j);
            }
        }
        table_ = std::move(newtable);
        reserve_ = newreserve;
        for (size_t i = exthreads_ - 1; i != SIZE_MAX; --i) {
            locks_[i].unlock();
        }
    }
    bool Insert(const K& key, const V& value) {
        auto hash = hasher_(key);
        locks_[hash % exthreads_].lock();
        auto ind = hash % reserve_;
        for (auto i : table_[ind]) {
            if (i.first == key) {
                locks_[hash % exthreads_].unlock();
                return false;
            }
        }
        table_[ind].push_front(std::make_pair(key, value));
        size_.fetch_add(1);
        locks_[hash % exthreads_].unlock();
        if (static_cast<double>(size_) / reserve_ > kThreshold) {
            Rehash();
        }
        return true;
    }
    bool Erase(const K& key) {
        auto hash = hasher_(key);
        locks_[hash % exthreads_].lock();
        auto ind = hash % reserve_;
        for (auto i = table_[ind].begin(); i != table_[ind].end(); ++i) {
            if ((*i).first == key) {
                table_[ind].erase(i);
                size_.fetch_sub(1);
                locks_[hash % exthreads_].unlock();
                return true;
            }
        }
        locks_[hasher_(key) % exthreads_].unlock();
        return false;
    }

    void Clear() {
        for (size_t i = 0; i < exthreads_; ++i) {
            locks_[i].lock();
        }
        for (size_t i = 0; i < reserve_; ++i) {
            table_[i].clear();
        }
        size_ = 0;
        for (size_t i = exthreads_ - 1; i != SIZE_MAX; --i) {
            locks_[i].unlock();
        }
    }

    std::pair<bool, V> Find(const K& key) const {
        auto hash = hasher_(key);
        locks_[hash % exthreads_].lock();
        for (auto i : table_[hash % reserve_]) {
            if (i.first == key) {
                locks_[hash % exthreads_].unlock();
                return std::make_pair(true, i.second);
            }
        }
        locks_[hasher_(key) % exthreads_].unlock();
        return std::make_pair(false, V());
    }

    const V At(const K& key) const {
        auto hash = hasher_(key);
        locks_[hash % exthreads_].lock();
        for (auto i : table_[hash % reserve_]) {
            if (i.first == key) {
                locks_[hash % exthreads_].unlock();
                return i.second;
            }
        }
        throw std::out_of_range("No such key");
    }

    size_t Size() const {
        return size_;
    }
    static const int kIncrease;
    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;
    static const double kThreshold;

private:
    std::unique_ptr<std::list<std::pair<K, V>>[]> table_;
    mutable std::unique_ptr<std::mutex[]> locks_;
    Hash hasher_;
    std::atomic_size_t size_;
    std::atomic_size_t reserve_;
    std::atomic_size_t exthreads_;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = -1;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kIncrease = 9;

template <class K, class V, class Hash>
const double ConcurrentHashMap<K, V, Hash>::kThreshold = 11;