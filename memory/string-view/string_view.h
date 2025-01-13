#pragma once

#include <string>
#include <cstring>

class StringView {
private:
    const char* p_;
    size_t size_ = std::string::npos;

public:
    explicit StringView(const std::string& s, const size_t i = 0,
                        const size_t size = std::string::npos) {
        if (size > s.size() - i) {
            size_ = s.size() - i;
        } else {
            size_ = size;
        }
        p_ = &s[i];
    }

    explicit StringView(const char* p) {
        p_ = p;
        size_ = std::strlen(p);
    }

    explicit StringView(const char* p, const size_t size) {
        p_ = p;
        size_ = size;
    }

    const char& operator[](size_t i) const {
        return *(p_ + i);
    }

    size_t Size() const {
        return size_;
    }
};
