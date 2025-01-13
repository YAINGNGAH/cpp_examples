#pragma once

#include "sw_fwd.h"  // Forward declaratio

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
private:
    T* x_;
    ControlBase* control_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
        x_ = nullptr;
        control_ = nullptr;
    }

    WeakPtr(const WeakPtr& other) {
        x_ = other.x_;
        control_ = other.control_;
        if (control_) {
            control_->WeakIncrement();
        }
    }
    template <typename U>
    WeakPtr(const WeakPtr<U>& other) {
        x_ = reinterpret_cast<T*>(other.Get());
        control_ = other.GetControl();
        if (control_) {
            control_->WeakIncrement();
        }
    }
    WeakPtr(WeakPtr&& other) {
        x_ = other.x_;
        control_ = other.control_;
        other.x_ = nullptr;
        other.control_ = nullptr;
    }
    template <typename U>
    WeakPtr(WeakPtr<U>&& other) {
        x_ = reinterpret_cast<T*>(other.Get());
        control_ = other.GetControl();
        other.MoveReset();
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        x_ = other.Get();
        control_ = other.GetControl();
        if (control_) {
            control_->WeakIncrement();
        }
    }
    template <typename U>
    WeakPtr(const SharedPtr<U>& other) {
        x_ = reinterpret_cast<T*>(other.Get());
        control_ = other.GetControl();
        if (control_) {
            control_->WeakIncrement();
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        x_ = other.x_;
        if (control_ != other.GetControl()) {
            if (control_) {
                control_->WeakDecrement();
            }
            control_ = other.control_;
            if (control_) {
                control_->WeakIncrement();
            }
        }
        return *this;
    }
    template <typename U>
    WeakPtr& operator=(const WeakPtr& other) {
        x_ = reinterpret_cast<T*>(other.Get());
        if (control_ != other.GetControl()) {
            if (control_) {
                control_->WeakDecrement();
            }
            control_ = other.GetControl();
            if (control_) {
                control_->WeakIncrement();
            }
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (other.Get() == Get()) {
            return *this;
        }
        x_ = other.x_;
        if (control_) {
            control_->WeakDecrement();
        }
        control_ = other.control_;
        other.x_ = nullptr;
        other.control_ = nullptr;
        return *this;
    }
    template <typename U>
    WeakPtr& operator=(WeakPtr<U>&& other) {
        if (other.Get() == Get()) {
            return *this;
        }
        x_ = reinterpret_cast<T*>(other.Get());
        if (control_) {
            control_->WeakDecrement();
        }
        control_ = other.GetControl();
        other.MoveReset();
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor
    ~WeakPtr() {
        x_ = nullptr;
        if (control_) {
            control_->WeakDecrement();
        }
        control_ = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void MoveReset() {
        x_ = nullptr;
        control_ = nullptr;
    }

    void Reset() {
        if (control_) {
            control_->WeakDecrement();
        }
        x_ = nullptr;
        control_ = nullptr;
    }
    void Swap(WeakPtr& other) {
        if (Get() == other.Get()) {
            return;
        }
        std::swap(x_, other.x_);
        std::swap(control_, other.control_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers
    T* Get() const {
        return x_;
    }
    ControlBase* GetControl() const {
        return control_;
    }
    size_t UseCount() const {
        if (control_) {
            return control_->Count();
        }
        return 0;
    }
    bool Expired() const {
        if (control_) {
            if (control_->Count() != 0) {
                return false;
            }
        }
        return true;
    }
    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(*this);
    }
};
