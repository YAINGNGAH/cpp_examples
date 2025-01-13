#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <type_traits>

class ControlBase {
protected:
    size_t c_;

public:
    ControlBase() {
        c_ = 1;
    }
    void Increment() {
        c_++;
    }
    virtual void Decrement() {
        c_--;
    }
    size_t Count() const {
        return c_;
    }

    virtual ~ControlBase() = default;
};

template<typename T>
class Control : public ControlBase {
private:
    T* ptr_;
public:
    Control(T* ptr) {
        ControlBase();
        ptr_ = ptr;
    }
    void Decrement() override {
        ControlBase::Decrement();
        if (c_ == 0) {
            delete this;
        }
    }
    T* Get() const {
        return ptr_;
    }
    ~Control() {
        if (ptr_) {
            delete ptr_;
        }
    }
};
template <typename T>
class ControlObject : public ControlBase {
private:
    std::aligned_storage_t<sizeof(T), alignof(T)> object_;

public:
    template <typename... Args>
    ControlObject(Args&&... args) {
        ::new (&object_) T(std::forward<Args>(args)...);
    }
    void Decrement() override {
        ControlBase::Decrement();
        if (c_ == 0) {
            delete this;
        }
    }
    T* Get() {
        return reinterpret_cast<T*>(&object_);
    }
    ~ControlObject() {
        reinterpret_cast<T*>(&object_)->~T();
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
    }
    template <typename U>
    explicit SharedPtr(U* ptr) : x_(reinterpret_cast<T*>(ptr)) {
        control_ = reinterpret_cast<ControlBase*>(new Control(ptr));
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
    }
    // Кал для мейкшейрда о боже убейте меня пожалуйста
    SharedPtr(ControlObject<T>* control) {
        control_ = reinterpret_cast<ControlBase*>(control);
        x_ = control->Get();
    }
    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other);  // not implemented for now

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
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    if constexpr (std::is_same_v<decltype(left.Get()), decltype(right.Get())>) {
        if (left.Get() == right.Get()) {
            return true;
        }
    }
    return false;
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new ControlObject<T>(std::forward<Args>(args)...));
}

// Look for usage examples in tests
// another time
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
