#pragma once

#include <utility>
#include <functional>

// Идея решения взята с https://github.com/TartanLlama/function_ref/blob/master/include/tl/function_ref.hpp#L101
// Если решение недостаточно оригинальное, прошу (при обнаружении до дедлайна) сообщить - я переделаю на более или (После дедлайна) занулить только эту задачу.

template <typename Signature>
class FunctionRef;

template <class R, class... Args>
class FunctionRef<R(Args...)> {
private:
    void* f_ = nullptr;
    R (*trampline_)(void*, Args...) = nullptr;

public:
    FunctionRef() = delete;
    template <typename F>
    FunctionRef(F&& f) {
        if constexpr (std::is_pointer_v<F>) {
            f_ = reinterpret_cast<void*>(f);
            trampline_ = [](void* f, Args... args) -> R {
                return (*reinterpret_cast<F>(f))(std::forward<Args>(args)...);
            };
        } else {
            f_ = reinterpret_cast<void*>(&f);
            trampline_ = [](void* f, Args... args) -> R {
                return (*reinterpret_cast<typename std::add_pointer<F>::type>(f))(
                    std::forward<Args>(args)...);
            };
        }
    }
    R operator()(Args... args) {
        return trampline_(f_, std::forward<Args>(args)...);
    }
};

template <class... Args>
class FunctionRef<void(Args...)> {
private:
    void* f_ = nullptr;
    void (*trampline_)(void*, Args...) = nullptr;

public:
    FunctionRef() = delete;
    template <typename F>
    FunctionRef(F&& f) {
        if constexpr (std::is_pointer_v<F>) {
            f_ = reinterpret_cast<void*>(f);
            trampline_ = [](void* f, Args... args) {
                (*reinterpret_cast<F>(f))(std::forward<Args>(args)...);
            };
        } else {
            f_ = reinterpret_cast<void*>(&f);
            trampline_ = [](void* f, Args... args) {
                (*reinterpret_cast<typename std::add_pointer<F>::type>(f))(
                    std::forward<Args>(args)...);
            };
        }
    }
    void operator()(Args... args) {
        trampline_(f_, std::forward<Args>(args)...);
    }
};
