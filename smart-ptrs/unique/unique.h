#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <type_traits>
template <typename T>
struct Slug {
    Slug() noexcept = default;
    template <typename U>
    Slug(Slug<U>&& rhs) {
    }
    template <typename U>
    Slug& operator=(Slug<U>&& rhs) {
        return *this;
    }
    void operator()(T* ptr) {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void_v<T>);
        if (ptr == nullptr) {
            return;
        }
        delete ptr;
    }
};
template <typename T>
struct Slug<T[]> {
    Slug() noexcept = default;
    template <typename U>
    Slug(Slug<U>&& rhs) {
    }
    template <typename U>
    Slug& operator=(Slug<U>&& rhs) {
        return *this;
    }
    void operator()(T* ptr) {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void_v<T>);
        if (ptr == nullptr) {
            return;
        }
        delete[] ptr;
    }
};
// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
private:
    CompressedPair<T*, Deleter> p_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : p_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : p_(ptr, std::forward<Deleter>(deleter)) {
    }

    template <typename F, typename U>
    UniquePtr(F* ptr, U&& deleter) : p_(dynamic_cast<T*>(ptr), std::forward<U>(deleter)) {
    }

    UniquePtr(UniquePtr<T, Deleter>&& other) noexcept {
        p_.GetFirst() = other.Release();
        p_.GetSecond() = std::move(other.GetDeleter());
    }
    template <typename F, typename U>
    UniquePtr(UniquePtr<F, U>&& other) noexcept {
        p_.GetFirst() = dynamic_cast<T*>(other.Release());
        if constexpr (std::is_same_v<U, Slug<F>>) {
            p_.GetSecond() = Slug<T>();
        } else {
            p_.GetSecond() = std::move(*dynamic_cast<Deleter*>(&other.GetDeleter()));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr<T, Deleter>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        GetDeleter()(p_.GetFirst());
        p_.GetFirst() = other.Release();
        p_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }
    template <typename U, typename F>
    UniquePtr& operator=(UniquePtr<U, F>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        GetDeleter()(p_.GetFirst());
        p_.GetFirst() = dynamic_cast<T*>(other.Release());
        if constexpr (std::is_same_v<F, Slug<U>>) {
            p_.GetSecond() = Slug<T>();
        } else {
            p_.GetSecond() = std::move(*dynamic_cast<Deleter*>(&other.GetDeleter()));
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        if (p_.GetFirst() != nullptr) {
            GetDeleter()(p_.GetFirst());
            p_.GetFirst() = nullptr;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        GetDeleter()(p_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* ptr = p_.GetFirst();
        p_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) {
        T* old_ptr = p_.GetFirst();
        p_.GetFirst() = ptr;
        if (old_ptr) {
            GetDeleter()(old_ptr);
        }
    }
    void Swap(UniquePtr& other) {
        if (this == &other) {
            return;
        }
        std::swap(p_, other.p_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return p_.GetFirst();
    }
    Deleter& GetDeleter() {
        return p_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return p_.GetSecond();
    }
    explicit operator bool() const {
        if (p_.GetFirst()) {
            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators
    T& operator*() const {
        return *p_.GetFirst();
    }
    T* operator->() const {
        return p_.GetFirst();
    }
};

// void specialization
template <typename Deleter>
class UniquePtr<void, Deleter> {
private:
    CompressedPair<void*, Deleter> p_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(void* ptr = nullptr) : p_(ptr, Deleter()) {
    }

    UniquePtr(void* ptr, Deleter deleter) : p_(ptr, std::forward<Deleter>(deleter)) {
    }

    template <typename F, typename U>
    UniquePtr(F* ptr, U&& deleter) : p_(dynamic_cast<void*>(ptr), std::forward<U>(deleter)) {
    }

    UniquePtr(UniquePtr<void, Deleter>&& other) noexcept {
        p_.GetFirst() = other.Release();
        p_.GetSecond() = std::move(other.GetDeleter());
    }
    template <typename F, typename U>
    UniquePtr(UniquePtr<F, U>&& other) noexcept {
        p_.GetFirst() = dynamic_cast<void*>(other.Release());
        if constexpr (std::is_same_v<U, Slug<F>>) {
            p_.GetSecond() = Slug<void>();
        } else {
            p_.GetSecond() = std::move(*dynamic_cast<Deleter*>(&other.GetDeleter()));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr<void, Deleter>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        GetDeleter()(p_.GetFirst());
        p_.GetFirst() = other.Release();
        p_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }
    template <typename U, typename F>
    UniquePtr& operator=(UniquePtr<U, F>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        GetDeleter()(p_.GetFirst());
        p_.GetFirst() = dynamic_cast<void*>(other.Release());
        if constexpr (std::is_same_v<F, Slug<U>>) {
            p_.GetSecond() = Slug<void>();
        } else {
            p_.GetSecond() = std::move(*dynamic_cast<Deleter*>(&other.GetDeleter()));
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        if (p_.GetFirst() != nullptr) {
            GetDeleter()(p_.GetFirst());
            p_.GetFirst() = nullptr;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        GetDeleter()(p_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void* Release() {
        void* ptr = p_.GetFirst();
        p_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(void* ptr = nullptr) {
        if (p_.GetFirst()) {
            GetDeleter()(p_.GetFirst());
        }
        p_.GetFirst() = ptr;
    }
    void Swap(UniquePtr& other) {
        if (this == &other) {
            return;
        }
        std::swap(p_, other.p_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    void* Get() const {
        return p_.GetFirst();
    }
    Deleter& GetDeleter() {
        return p_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return p_.GetSecond();
    }
    explicit operator bool() const {
        if (p_.GetFirst()) {
            return true;
        }
        return false;
    }
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
private:
    CompressedPair<T*, Deleter> p_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : p_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : p_(ptr, std::forward<Deleter>(deleter)) {
    }

    template <typename F, typename U>
    UniquePtr(F* ptr, U&& deleter) : p_(dynamic_cast<T*>(ptr), std::forward<U>(deleter)) {
    }

    UniquePtr(UniquePtr<T, Deleter>&& other) noexcept {
        p_.GetFirst() = other.Release();
        p_.GetSecond() = std::move(other.GetDeleter());
    }
    template <typename F, typename U>
    UniquePtr(UniquePtr<F, U>&& other) noexcept {
        p_.GetFirst() = dynamic_cast<T*>(other.Release());
        if constexpr (std::is_same_v<U, Slug<F>>) {
            p_.GetSecond() = Slug<T>();
        } else {
            p_.GetSecond() = std::move(*dynamic_cast<Deleter*>(&other.GetDeleter()));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr<T, Deleter>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        GetDeleter()(p_.GetFirst());
        p_.GetFirst() = other.Release();
        p_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }
    template <typename U, typename F>
    UniquePtr& operator=(UniquePtr<U, F>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        GetDeleter()(p_.GetFirst());
        p_.GetFirst() = dynamic_cast<T*>(other.Release());
        if constexpr (std::is_same_v<F, Slug<U>>) {
            p_.GetSecond() = Slug<T>();
        } else {
            p_.GetSecond() = std::move(*dynamic_cast<Deleter*>(&other.GetDeleter()));
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        if (p_.GetFirst() != nullptr) {
            GetDeleter()(p_.GetFirst());
            p_.GetFirst() = nullptr;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        GetDeleter()(p_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* ptr = p_.GetFirst();
        p_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) {
        if (p_.GetFirst()) {
            GetDeleter()(p_.GetFirst());
        }
        p_.GetFirst() = ptr;
    }
    void Swap(UniquePtr& other) {
        if (this == &other) {
            return;
        }
        std::swap(p_, other.p_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return p_.GetFirst();
    }
    Deleter& GetDeleter() {
        return p_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return p_.GetSecond();
    }
    explicit operator bool() const {
        if (p_.GetFirst()) {
            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators
    T& operator*() const {
        return *p_.GetFirst();
    }
    T* operator->() const {
        return p_.GetFirst();
    }
    T& operator[](size_t i) {
        return p_.GetFirst()[i];
    }
};
