#pragma once
#include <memory>

class Any {
public:
    Any() {
        ptr_ = nullptr;
    }

    template <class T>
    Any(const T& value) : ptr_(new Storage<T>(value)) {
    }

    template <class T>
    Any& operator=(const T& value) {
        ptr_.reset();

        ptr_ = std::unique_ptr<Storagebase>(new Storage<T>(value));
        return *this;
    }

    Any(const Any& rhs) : ptr_(rhs.ptr_->Clone()) {
    }
    Any& operator=(const Any& rhs) {
        Any tmp(rhs);
        std::swap(ptr_, tmp.ptr_);
        return *this;
    }
    ~Any() {
    }

    bool Empty() const {
        if (ptr_.get()) {
            return false;
        }
        return true;
    }

    void Clear() {
        ptr_.reset();
    }
    void Swap(Any& rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

    template <class T>
    const T& GetValue() const {
        return *dynamic_cast<Storage<T>&>(*ptr_);
    }

private:
    struct Storagebase {
        virtual Storagebase* Clone() const {
            return nullptr;
        }
        virtual ~Storagebase() {
        }
    };
    template <typename T>
    struct Storage : Storagebase {
        T val_;

    public:
        explicit Storage(const T& rhs) : val_(rhs) {
        }
        virtual Storagebase* Clone() const override {
            return new Storage(val_);
        }
        T& operator*() {
            return val_;
        }
        const T& operator*() const {
            return val_;
        }
    };

    std::unique_ptr<Storagebase> ptr_;
};
