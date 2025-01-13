#pragma once

#include <type_traits>
#include <utility>

template <typename F, typename S, bool = !std::is_empty_v<F> || std::is_final_v<F>,
          bool = !std::is_empty_v<S> || std::is_final_v<S>,
          bool = std::is_base_of_v<F, S> || std::is_base_of_v<S, F>>
class CompressedPair {
public:
    CompressedPair() : first_(F()), second_(S()) {
    }

    CompressedPair(const CompressedPair& rhs) : first_(rhs.first_), second_(rhs.second_) {
    }

    CompressedPair(CompressedPair&& rhs)
        : first_(std::move(rhs.first_)), second_(std::move(rhs.second_)) {
    }

    CompressedPair& operator=(const CompressedPair& rhs) {
        first_ = rhs.first_;
        second_ = rhs.second_;
        return *this;
    }
    CompressedPair& operator=(CompressedPair&& rhs) {
        first_ = std::move(rhs.first_);
        second_ = std::move(rhs.second_);
        return *this;
    }

    template <typename U, typename T>
    CompressedPair(U&& first, T&& second)
        : first_(std::forward<U>(first)), second_(std::forward<T>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_;
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, true, false> : private F {
public:
    CompressedPair() : F(F()), second_(S()) {
    }

    CompressedPair(const CompressedPair& rhs) : F(*dynamic_cast<F*>(&rhs)), second_(rhs.second_) {
    }

    CompressedPair(CompressedPair&& rhs)
        : F(*dynamic_cast<F*>(&rhs)), second_(std::move(rhs.second_)) {
    }

    CompressedPair& operator=(const CompressedPair& rhs) {
        second_ = rhs.second_;
        return *this;
    }
    CompressedPair& operator=(CompressedPair&& rhs) {
        second_ = std::move(rhs.second_);
        return *this;
    }

    template <typename U, typename T>
    CompressedPair(U&& first, T&& second)
        : F(std::forward<U>(first)), second_(std::forward<T>(second)) {
    }

    F& GetFirst() {
        return static_cast<F&>(*this);
    }

    const F& GetFirst() const {
        return static_cast<F const&>(*this);
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, true, true> : private F {
public:
    CompressedPair() : F(F()), second_(S()) {
    }

    CompressedPair(const CompressedPair& rhs) : F(*dynamic_cast<F*>(&rhs)), second_(rhs.second_) {
    }

    CompressedPair(CompressedPair&& rhs)
        : F(*dynamic_cast<F*>(&rhs)), second_(std::move(rhs.second_)) {
    }

    CompressedPair& operator=(const CompressedPair& rhs) {
        second_ = rhs.second_;
        return *this;
    }
    CompressedPair& operator=(CompressedPair&& rhs) {
        second_ = std::move(rhs.second_);
        return *this;
    }

    template <typename U, typename T>
    CompressedPair(U&& first, T&& second)
        : F(std::forward<U>(first)), second_(std::forward<T>(second)) {
    }

    F& GetFirst() {
        return static_cast<F&>(*this);
    }

    const F& GetFirst() const {
        return static_cast<F const&>(*this);
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <typename F, typename S>
class CompressedPair<F, S, true, false, false> : private S {
public:
    CompressedPair() : S(S()), first_(F()) {
    }

    CompressedPair(const CompressedPair& rhs) : first_(rhs.first_), S(*dynamic_cast<S*>(&rhs)) {
    }

    CompressedPair(CompressedPair&& rhs)
        : first_(std::move(rhs.first_)), S(*dynamic_cast<S*>(&rhs)) {
    }

    CompressedPair& operator=(const CompressedPair& rhs) {
        first_ = rhs.first_;
        return *this;
    }
    CompressedPair& operator=(CompressedPair&& rhs) {
        first_ = std::move(rhs.first_);
        return *this;
    }

    template <typename U, typename T>
    CompressedPair(U&& first, T&& second)
        : first_(std::forward<U>(first)), S(std::forward<T>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return static_cast<S&>(*this);
    };

    const S& GetSecond() const {
        return static_cast<S const&>(*this);
    };

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair<F, S, true, false, true> : private S {
public:
    CompressedPair() : S(S()), first_(F()) {
    }

    CompressedPair(const CompressedPair& rhs) : first_(rhs.first_), S(*dynamic_cast<S*>(&rhs)) {
    }

    CompressedPair(CompressedPair&& rhs)
        : first_(std::move(rhs.first_)), S(*dynamic_cast<S*>(&rhs)) {
    }

    CompressedPair& operator=(const CompressedPair& rhs) {
        first_ = rhs.first_;
        return *this;
    }
    CompressedPair& operator=(CompressedPair&& rhs) {
        first_ = std::move(rhs.first_);
        return *this;
    }

    template <typename U, typename T>
    CompressedPair(U&& first, T&& second)
        : first_(std::forward<U>(first)), S(std::forward<T>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return static_cast<S&>(*this);
    };

    const S& GetSecond() const {
        return static_cast<S const&>(*this);
    };

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair<F, S, false, false, false> : private F, private S {
public:
    CompressedPair() : F(F()), S(S()) {
    }

    CompressedPair(const CompressedPair& rhs)
        : F(*dynamic_cast<F*>(&rhs)), S(*dynamic_cast<S*>(&rhs)) {
    }

    CompressedPair(CompressedPair&& rhs) : F(*dynamic_cast<F*>(&rhs)), S(*dynamic_cast<S*>(&rhs)) {
    }

    CompressedPair& operator=(const CompressedPair& rhs) {
        return *this;
    }
    CompressedPair& operator=(CompressedPair&& rhs) {
        return *this;
    }

    template <typename U, typename T>
    CompressedPair(U&& first, T&& second) : F(std::forward<U>(first)), S(std::forward<T>(second)) {
    }

    F& GetFirst() {
        return static_cast<F&>(*this);
    }

    const F& GetFirst() const {
        return static_cast<F const&>(*this);
    }

    S& GetSecond() {
        return static_cast<S&>(*this);
    };

    const S& GetSecond() const {
        return static_cast<S const&>(*this);
    };
};