#pragma once

#include <memory>
#include <typeinfo>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual const std::type_info& TypeInfo() {
        return typeid(Object);
    }
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }
    int GetValue() const {
        return value_;
    }
    const std::type_info& TypeInfo() override {
        return typeid(Number);
    }

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& name) : name_(name) {
    }
    const std::string& GetName() const {
        return name_;
    }
    const std::type_info& TypeInfo() override {
        return typeid(Symbol);
    }

private:
    std::string name_;
};

class Cell : public Object {
public:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
    Cell() = default;
    Cell(Object* first, Object* second) : first_(first), second_(second) {
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }
    const std::type_info& TypeInfo() override {
        return typeid(Cell);
    }
};

// class only for parsing, not used in further program
class Dot : public Object {
public:
    Dot() = default;
    const std::type_info& TypeInfo() override {
        return typeid(Dot);
    }
};

class CloseBracket : public Object {
public:
    CloseBracket() = default;
    const std::type_info& TypeInfo() override {
        return typeid(CloseBracket);
    }
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (obj == nullptr) {
        return false;
    }
    if (std::dynamic_pointer_cast<T>(obj)) {
        return true;
    }
    return false;
}
