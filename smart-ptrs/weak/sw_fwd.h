#pragma once

#include <exception>

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

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

class Control : public ControlBase {
private:
    class PtrBase {
    public:
        virtual ~PtrBase() = default;
    };
    template <typename T>
    class Ptr : public PtrBase {
    private:
        T* ptr_;

    public:
        Ptr(T* ptr) {
            ptr_ = ptr;
        }
        T* Get() {
            return ptr_;
        }
        ~Ptr() {
            delete ptr_;
        }
    };
    PtrBase* ptr_;

public:
    template <typename T>
    Control(T* ptr) {
        ControlBase();
        ptr_ = reinterpret_cast<PtrBase*>(new Ptr<T>(ptr));
    }
    void Decrement() override {
        ControlBase::Decrement();
        if (cwk_ == 0) {
            delete this;
        } else if (csh_ == 0) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }
    void WeakDecrement() override {
        ControlBase::WeakDecrement();
        if (cwk_ == 0) {
            delete this;
        }
    }
    template <typename T>
    auto Get() const {
        return reinterpret_cast<Ptr<T>*>(ptr_)->Get();
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
    void Decrement() override {
        ControlBase::Decrement();
        if (cwk_ == 0) {
            delete this;
        } else if (csh_ == 0) {
            reinterpret_cast<T*>(&object_)->~T();
            deleted_ = true;
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