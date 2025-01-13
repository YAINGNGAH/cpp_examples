#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <type_traits>

template <typename T>
class EnableSharedFromThis : public ESFDbase {
public:
    WeakPtr<T> weak_this_;
    EnableSharedFromThis() : weak_this_() {
    }
    bool Enable() const {
        return !weak_this_.Expired();
    }
    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(weak_this_);
    }
    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(weak_this_);
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(weak_this_);
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(weak_this_);
    }
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
private:
    T* x_;
    ControlBase* control_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() : x_(nullptr) {
        control_ = nullptr;
    }
    SharedPtr(std::nullptr_t) : x_(nullptr) {
        control_ = nullptr;
    }
    explicit SharedPtr(T* ptr) : x_(ptr) {
        control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            InitWeakThis(x_);
        }
    }
    template <typename U>
    explicit SharedPtr(U* ptr) : x_(reinterpret_cast<T*>(ptr)) {
        control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            InitWeakThis(x_);
        }
    }

    SharedPtr(const SharedPtr& other) {
        x_ = other.Get();
        control_ = other.GetControl();
        if (control_) {
            control_->Increment();
        }
    }
    template <typename U>
    SharedPtr(const SharedPtr<U>& other) {
        x_ = reinterpret_cast<T*>(other.Get());
        control_ = other.GetControl();
        if (control_) {
            control_->Increment();
        }
    }
    SharedPtr(SharedPtr&& other) noexcept {
        x_ = other.Get();
        control_ = other.GetControl();
        other.MoveReset();
    }
    template <typename U>
    SharedPtr(SharedPtr<U>&& other) noexcept {
        x_ = reinterpret_cast<T*>(other.Get());
        control_ = other.GetControl();
        other.MoveReset();
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        x_ = ptr;
        control_ = other.GetControl();
        if (control_) {
            control_->Increment();
        }
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            InitWeakThis(x_);
        }
    }
    // Кал для мейкшейрда о боже убейте меня пожалуйста
    SharedPtr(ControlObject<T>* control) {
        control_ = reinterpret_cast<ControlBase*>(control);
        x_ = control->Get();
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            InitWeakThis(x_);
        }
    }
    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        control_ = other.GetControl();
        x_ = other.Get();
        if (control_) {
            control_->Increment();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        x_ = other.Get();
        if (control_ != other.GetControl()) {
            if (control_) {
                control_->Decrement();
            }
            control_ = other.GetControl();
            if (control_) {
                control_->Increment();
            }
        }
        return *this;
    }
    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& other) {
        x_ = reinterpret_cast<T*>(other.Get());
        if (control_ != other.GetControl()) {
            if (control_) {
                control_->Decrement();
            }
            control_ = other.GetControl();
            if (control_) {
                control_->Increment();
            }
        }
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        x_ = other.Get();
        other.x_ = nullptr;
        if (control_ != other.GetControl()) {
            if (control_) {
                control_->Decrement();
            }
            control_ = other.GetControl();
        }
        other.control_ = nullptr;
        return *this;
    }
    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) noexcept {
        if (Get() == other.Get()) {
            return *this;
        }
        x_ = reinterpret_cast<T*>(other.Get());
        if (control_ != other.GetControl()) {
            if (control_) {
                control_->Decrement();
            }
            control_ = other.GetControl();
        }
        other.MoveReset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        x_ = nullptr;
        if (control_) {
            control_->Decrement();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        x_ = nullptr;
        if (control_) {
            control_->Decrement();
        }
        control_ = nullptr;
    }
    void MoveReset() {
        x_ = nullptr;
        control_ = nullptr;
    }

    void Reset(T* ptr) {
        if (x_ != ptr) {
            x_ = ptr;
        }
        if (control_) {
            control_->Decrement();
            if (ptr) {
                control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
            } else {
                control_ = nullptr;
            }
        } else {
            if (ptr) {
                control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
            }
        }
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            InitWeakThis(x_);
        }
    }
    template <typename U>
    void Reset(U* ptr) {
        if (x_ != reinterpret_cast<T*>(ptr)) {
            x_ = reinterpret_cast<T*>(ptr);
        }
        if (control_) {
            control_->Decrement();
            if (ptr) {
                control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
            } else {
                control_ = nullptr;
            }
        } else {
            if (ptr) {
                control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
            }
        }
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            InitWeakThis(x_);
        }
    }
    void Swap(SharedPtr& other) {
        if (Get() == other.Get()) {
            return;
        }
        std::swap(x_, other.x_);
        std::swap(control_, other.control_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    ControlBase* GetControl() const {
        return control_;
    }

    T* Get() const {
        return x_;
    }
    T& operator*() const {
        return *x_;
    }
    T* operator->() const {
        return x_;
    }
    size_t UseCount() const {
        if (control_) {
            return control_->Count();
        }
        return 0;
    }
    explicit operator bool() const {
        if (x_) {
            return true;
        }
        return false;
    }
    // help
    template <typename Y>
    void InitWeakThis(EnableSharedFromThis<Y>* e) {
        if (!e->Enable()) {
            e->weak_this_ = *this;
        }
    }
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    if (left.Get() == right.Get()) {
        return true;
    }
    return false;
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new ControlObject<T>(std::forward<Args>(args)...));
}
