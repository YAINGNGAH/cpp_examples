#pragma once
#include <algorithm>

template <typename Callback>
class CallbackStorage {
private:
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
    bool alive_;

public:
    explicit CallbackStorage(Callback&& callback) {
        ::new (GetCallbackBuffer()) Callback(std::forward<Callback>(callback));
        alive_ = true;
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    bool Alive() const {
        return alive_;
    }

    void Kill() {
        reinterpret_cast<Callback*>(&callback_buffer_)->~Callback();
        alive_ = false;
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }
};

template <typename Callback>
class Defer final {
private:
    CallbackStorage<Callback> storage_;

public:
    Defer(const Callback& callback) = delete;
    Defer(Callback&& callback) : storage_(std::forward<Callback>(callback)) {
    }
    void Cancel() && {
        storage_.Kill();
    }
    void Invoke() && {
        if (storage_.Alive()) {
            std::move(storage_.GetCallback())();
            storage_.Kill();
        }
    }
    ~Defer() {
        if (storage_.Alive()) {
            std::move(storage_.GetCallback())();
            storage_.Kill();
        }
    }
};
