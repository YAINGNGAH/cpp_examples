#pragma once

#include <exception>
#include <stdexcept>
#include <type_traits>
#include <cstring>

class TryBase {
protected:
    std::exception_ptr e_;

public:
    TryBase() {
        e_ = nullptr;
    }
    template <typename Exception, bool = std::is_base_of_v<std::exception, Exception>>
    TryBase(const Exception& e) {
        e_ = std::make_exception_ptr(e);
    }
    TryBase(std::exception_ptr e) {
        e_ = e;
    }
    bool IsFailed() const {
        if (e_) {
            return true;
        }
        return false;
    }
    void Throw() const {
        if (e_) {
            std::rethrow_exception(e_);
        } else {
            throw std::logic_error("No exception");
        }
    }
};

template <class T>
class Try : private TryBase {
private:
    T value_;
    bool isempty_ = true;

public:
    using TryBase::IsFailed;
    using TryBase::Throw;
    Try() : TryBase() {
    }
    Try(const T& v) : TryBase(), value_(v) {
        isempty_ = false;
    }
    template <typename Exception, bool = std::is_base_of_v<std::exception, Exception>>
    Try(const Exception& e) : TryBase(e) {
    }
    Try(std::exception_ptr e) : TryBase(e) {
    }
    const T& Value() const {
        if (IsFailed()) {
            Throw();
        }
        if (isempty_) {
            throw std::logic_error("Object is empty");
        }
        return value_;
    }
};

template <>
class Try<void> : private TryBase {
private:
    std::exception_ptr e_;

public:
    using TryBase::IsFailed;
    using TryBase::Throw;
    Try() : TryBase() {
    }
    template <typename Exception, bool = std::is_base_of_v<std::exception, Exception>>
    Try(const Exception& e) : TryBase(e) {
    }
    Try(std::exception_ptr e) : TryBase(e) {
    }
};

template <class Function, class... Args>
auto TryRun(Function func, Args... args) {
    using ReturnType = decltype(func(args...));
    try {
        if constexpr (std::is_same_v<ReturnType, void>) {
            func(std::forward<Args>(args)...);
            return Try<void>();
        } else {
            return Try(func(std::forward<Args>(args)...));
        }
    } catch (const std::exception& e) {
        return Try<ReturnType>(std::current_exception());
    } catch (const char* e) {
        return Try<ReturnType>(std::logic_error(e));
    } catch (int e) {
        return Try<ReturnType>(std::logic_error(std::strerror(e)));
    } catch (...) {
        return Try<ReturnType>(std::logic_error("Unknown exception"));
    }
}
