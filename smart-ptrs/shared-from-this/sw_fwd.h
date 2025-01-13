#pragma once

#include <exception>

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class ESFDbase {
public:
    ESFDbase() = default;
    ~ESFDbase() = default;
};

template <typename T>
class EnableSharedFromThis;

class ControlBase {
protected:
    size_t csh_;
    size_t cwk_;

public:
    ControlBase() {
        csh_ = 1;
        cwk_ = 1;
    }
    void Increment() {
        csh_++;
        cwk_++;
    }
    virtual void Decrement() {
        csh_--;
        cwk_--;
    }
    void WeakIncrement() {
        cwk_++;
    }
    virtual void WeakDecrement() {
        cwk_--;
    }
    size_t Count() const {
        return csh_;
    }
    size_t AllCount() const {
        return cwk_;
    }

    virtual ~ControlBase() = default;
};
template <typename T>
class Control : public ControlBase {
private:
    T* ptr_;

public:
    Control(T* ptr) {
        ControlBase();
        ptr_ = ptr;
    }
    template <typename U>
    void DeleteWeakThis(EnableSharedFromThis<U>* e) {
        e->weak_this_.MoveReset();
    }
    void Decrement() override {
        ControlBase::Decrement();
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            if (cwk_ == 1) {
                DeleteWeakThis(ptr_);
                delete this;
            } else if (csh_ == 0) {
                delete ptr_;
                ptr_ = nullptr;
            }
        } else {
            if (cwk_ == 0) {
                delete this;
            } else if (csh_ == 0) {
                delete ptr_;
                ptr_ = nullptr;
            }
        }
    }
    void WeakDecrement() override {
        ControlBase::WeakDecrement();
        if (cwk_ == 0) {
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
    bool deleted_;

public:
    template <typename... Args>
    ControlObject(Args&&... args) {
        ::new (&object_) T(std::forward<Args>(args)...);
        deleted_ = false;
    }
    template <typename U>
    void DeleteWeakThis(EnableSharedFromThis<U>* e) {
        e->weak_this_.MoveReset();
    }
    void Decrement() override {
        ControlBase::Decrement();
        if constexpr (std::is_convertible_v<T*, ESFDbase*>) {
            if (cwk_ == 1) {
                DeleteWeakThis(reinterpret_cast<T*>(&object_));
                delete this;
            } else if (csh_ == 0) {
                reinterpret_cast<T*>(&object_)->~T();
                deleted_ = true;
            }
        } else {
            if (cwk_ == 0) {
                delete this;
            } else if (csh_ == 0) {
                reinterpret_cast<T*>(&object_)->~T();
                deleted_ = true;
            }
        }
    }
    void WeakDecrement() override {
        ControlBase::WeakDecrement();
        if (cwk_ == 0) {
            delete this;
        }
    }
    T* Get() {
        return reinterpret_cast<T*>(&object_);
    }
    ~ControlObject() {
        if (!deleted_) {
            reinterpret_cast<T*>(&object_)->~T();
        }
    }
};
