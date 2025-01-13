#pragma once
#include <array>
#include <cstddef>
#include <exception>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
private:
    std::array<std::pair<K, V>, MaxSize>
        map_;  // Ибо в душе не знаю, как мне тут красно-черное дерево писать.
    size_t size_ = 0;

public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        for (size_t i = 0; i < size_; ++i) {
            if (key == map_[i].first) {
                return map_[i].second;
            }
        }
        if (size_ == MaxSize) {
            throw std::exception();
        }
        map_[size_].first = key;
        size_++;
        return map_[size_ - 1].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (key == map_[i].first) {
                return map_[i].second;
            }
        }
        throw std::exception();
    }

    constexpr bool Erase(const K& key) {
        for (size_t i = 0; i < size_; ++i) {
            if (key == map_[i].first) {
                for (int j = i + 1; j < size_; ++j) {
                    map_[j - 1] = map_[j];
                }
                size_--;
                return true;
            }
        }
        return false;
    }

    constexpr bool Find(const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (key == map_[i].first) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return size_;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return map_[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return map_[pos];
    }
};
