#pragma once
#include <type_traits>
#include <vector>
struct Sum {
    template <typename T>
    T operator()(T a, T b) {
        if constexpr (!std::is_integral_v<T>) {
            throw "Wrong type";
        }
        return a + b;
    }
};

struct Prod {
    template <typename T>
    T operator()(T a, T b) {
        if constexpr (!std::is_integral_v<T>) {
            throw "Wrong type";
        }
        return a * b;
    }
};

struct Concat {
    template <typename T>
    std::vector<T> operator()(const std::vector<T>& a, const std::vector<T>& b) {
        std::vector<T> c;
        c.reserve(a.size() + b.size());
        for (auto i : a) {
            c.push_back(i);
        }
        for (auto i : b) {
            c.push_back(i);
        }
        return c;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    T value = init;
    Iterator it = first;
    while (it != last) {
        value = func(value, *it);
        ++it;
    }
    return value;
}

class Length {
private:
    int* len_;

public:
    Length(int* len) : len_(len) {
    }
    template <class T>
    T operator()(T a, T b) {
        (*len_)++;
        return T();
    }
};
