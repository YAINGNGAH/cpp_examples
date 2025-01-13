#pragma once

#include <iterator>

template <class Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
    }

    Iterator begin() const {  // NOLINT
        return begin_;
    }

    Iterator end() const {  // NOLINT
        return end_;
    }

private:
    Iterator begin_, end_;
};

template <typename It1, typename It2>
class ZipIterator {
private:
    It1 p1_;
    It2 p2_;

public:
    explicit ZipIterator(It1 p1, It2 p2) : p1_(p1), p2_(p2) {
    }
    bool operator==(const ZipIterator& it) const {
        if (p1_ == it.p1_ || p2_ == it.p2_) {
            return true;
        }
        return false;
    }
    bool operator!=(const ZipIterator& it) const {
        if (p1_ != it.p1_ && p2_ != it.p2_) {
            return true;
        }
        return false;
    }
    auto operator*() const {
        return std::make_pair(*p1_, *p2_);
    }
    const ZipIterator& operator++() {
        ++p1_;
        ++p2_;
        return *this;
    }
};

template <typename Sequence1, typename Sequence2>
auto Zip(const Sequence1& sequence1, const Sequence2& sequence2) {
    ZipIterator<decltype(std::begin(sequence1)), decltype(std::begin(sequence2))> begin(
        std::begin(sequence1), std::begin(sequence2));
    ZipIterator<decltype(std::begin(sequence1)), decltype(std::begin(sequence2))> end(
        std::end(sequence1), std::end(sequence2));
    return IteratorRange(begin, end);
}

class RangeIterator {
private:
    long long v_;
    long long k_;

public:
    explicit RangeIterator(long long v, long long k) : v_(v), k_(k) {
    }
    bool operator==(const RangeIterator& it) const {
        if (v_ == it.v_ && k_ == it.k_) {
            return true;
        }
        return false;
    }
    bool operator!=(const RangeIterator& it) const {
        if (v_ != it.v_ || k_ != it.k_) {
            return true;
        }
        return false;
    }
    auto operator*() const {
        return v_;
    }
    const RangeIterator& operator++() {
        v_ += k_;
        return *this;
    }
};

auto Range(long long f, long long s, long long k) {
    RangeIterator begin(f, k);
    RangeIterator end((s - f) % k == 0 ? s : s + k - (s - f) % k, k);
    return IteratorRange(begin, end);
}

auto Range(long long f, long long s) {
    RangeIterator begin(f, 1);
    RangeIterator end(s, 1);
    return IteratorRange(begin, end);
}

auto Range(long long s) {
    return Range(0, s);
}

template <typename It>
class InGroupIterator {
private:
    It p_;

public:
    explicit InGroupIterator(It p) : p_(p) {
    }
    bool operator==(const InGroupIterator& it) const {
        if (p_ == it.p_) {
            return true;
        }
        return false;
    }
    bool operator!=(const InGroupIterator& it) const {
        if (p_ != it.p_) {
            return true;
        }
        return false;
    }
    auto operator*() const {
        return *p_;
    }
    const InGroupIterator& operator++() {
        ++p_;
        return *this;
    }
};

template <typename It>
class Groupiterator {
private:
    It p_;
    It end_;

public:
    explicit Groupiterator(It p, It end) : p_(p), end_(end) {
    }
    bool operator==(const Groupiterator& it) const {
        if (p_ == it.p_) {
            return true;
        }
        return false;
    }
    bool operator!=(const Groupiterator& it) const {
        if (p_ != it.p_) {
            return true;
        }
        return false;
    }
    auto operator*() const {
        It p = p_;
        while (p != end_ && *p == *p_) {
            ++p;
        }
        return IteratorRange(InGroupIterator(p_), InGroupIterator(p));
    }
    const Groupiterator& operator++() {
        It p = p_;
        while (p != end_ && *p == *p_) {
            ++p;
        }
        p_ = p;
        return *this;
    }
};
template <typename Sequence>
auto Group(const Sequence& s) {
    Groupiterator<decltype(std::begin(s))> begin(std::begin(s), std::end(s));
    Groupiterator<decltype(std::begin(s))> end(std::end(s), std::end(s));
    return IteratorRange(begin, end);
}
