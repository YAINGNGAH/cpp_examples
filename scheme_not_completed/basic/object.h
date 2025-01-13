#pragma once

#include "error.h"
#include <memory>
#include <typeinfo>
#include <functional>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::string Serialization() {
        throw RuntimeError("Not implemented");
    }
    virtual std::shared_ptr<Object> Clone() {
        return std::make_shared<Object>();
    }
    virtual std::shared_ptr<Object> Eval() {
        throw RuntimeError("Not implemented");
    }
    virtual void Imply(std::shared_ptr<Object>) {
        throw RuntimeError("Not implemented");
    }
    virtual ~Object() = default;
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

/////////////////////////////////////////////////////////////////////////////
class Number : public Object {
public:
    Number(int64_t value) : value_(value) {
    }
    int GetValue() const {
        return value_;
    }
    std::string Serialization() override {
        return std::to_string(value_);
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Number>(value_);
    }
    std::shared_ptr<Object> Eval() override {
        return shared_from_this();
    }

private:
    int64_t value_;
};

class Symbol;

class Boolean : public Object {
private:
    bool value_;

public:
    Boolean(bool value) {
        value_ = value;
    }
    bool GetValue() const {
        return value_;
    }
    std::string Serialization() override {
        if (value_) {
            return "#t";
        } else {
            return "#f";
        }
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Boolean>(value_);
    }
    std::shared_ptr<Object> Eval() override {
        return shared_from_this();
    }
};
class SpecialForm : public Object{

};
class Quote : public SpecialForm {
private:
    std::shared_ptr<Object> ans_;

public:
    Quote(std::shared_ptr<Object> a) : ans_(a) {
    }
    std::string Serialization() override {
        if (ans_ == nullptr) {
            return "(quote ())";
        }
        return "(quote " + ans_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Quote>(ans_);
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
};

// FUNCTIONS
class Function : public Object {
public:
    size_t arguments_;
    std::string name_;
    std::shared_ptr<Object> type_;
    Function(int arg, std::shared_ptr<Object> type, std::string name) {
        arguments_ = arg;
        type_ = type;
        name_ = name;
    }
    std::string Serialization() override {
        return name_;
    }
};

// INTEGER
class IntegerPredicate : public Function {
public:
    std::shared_ptr<Boolean> ans_;
    IntegerPredicate() : Function(1, nullptr, "number?"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<IntegerPredicate>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Number>(obj)) {
            ans_ = std::make_shared<Boolean>(true);
        } else {
            ans_ = std::make_shared<Boolean>(false);
        }
    }
};

class CompareOperator : public Function {
public:
    std::function<bool(int64_t, int64_t)> op_;
    std::shared_ptr<Boolean> ans_ = std::make_shared<Boolean>(true);
    std::shared_ptr<Number> last_ = nullptr;
    CompareOperator() = delete;
    CompareOperator(std::function<bool(int64_t, int64_t)> op, std::string name)
        : Function(0, std::make_shared<Number>(0), name) {
        op_ = op;
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<CompareOperator>(op_, name_);
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (!Is<Number>(obj)) {
            throw RuntimeError("Not int in compareintoperator");
        }
        if (last_) {
            ans_ = std::make_shared<Boolean>(ans_->GetValue() &&
                                             op_(last_->GetValue(), As<Number>(obj)->GetValue()));
        } else {
            ans_ = std::make_shared<Boolean>(true);
            last_ = As<Number>(obj);
        }
    }
};

class Plus : public Function {
public:
    std::shared_ptr<Number> ans_ = std::make_shared<Number>(0);
    Plus() : Function(0, std::make_shared<Number>(0), "+") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Plus>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (!Is<Number>(obj)) {
            throw RuntimeError("Not int in ArithmeticOperatorint");
        }
        ans_ = std::make_shared<Number>(ans_->GetValue() + As<Number>(obj)->GetValue());
    }
};
class Minus : public Function {
public:
    std::shared_ptr<Number> ans_ = nullptr;
    Minus() : Function(0, std::make_shared<Number>(0), "-") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Minus>();
    }
    std::shared_ptr<Object> Eval() override {
        if (ans_) {
            return ans_;
        } else {
            throw RuntimeError("No argument in minus operator");
        }
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (!Is<Number>(obj)) {
            throw RuntimeError("Not int in ArithmeticOperatorint");
        }
        if (ans_) {
            ans_ = std::make_shared<Number>(ans_->GetValue() - As<Number>(obj)->GetValue());
        } else {
            ans_ = As<Number>(obj);
        }
    }
};
class Multiply : public Function {
public:
    std::shared_ptr<Number> ans_ = std::make_shared<Number>(1);
    Multiply() : Function(0, std::make_shared<Number>(0), "*") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Multiply>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (!Is<Number>(obj)) {
            throw RuntimeError("Not int in ArithmeticOperatorint");
        }
        ans_ = std::make_shared<Number>(ans_->GetValue() * As<Number>(obj)->GetValue());
    }
};
class Divide : public Function {
public:
    std::shared_ptr<Number> ans_ = nullptr;
    Divide() : Function(0, std::make_shared<Number>(0), "/") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Divide>();
    }
    std::shared_ptr<Object> Eval() override {
        if (ans_) {
            return ans_;
        } else {
            throw RuntimeError("No argument in devide operator");
        }
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (!Is<Number>(obj)) {
            throw RuntimeError("Not int in ArithmeticOperatorint");
        }
        if (ans_) {
            ans_ = std::make_shared<Number>(ans_->GetValue() / As<Number>(obj)->GetValue());
        } else {
            ans_ = As<Number>(obj);
        }
    }
};
class Abs : public Function {
public:
    std::shared_ptr<Number> ans_;
    Abs() : Function(1, std::make_shared<Number>(0), "abs") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Abs>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Number>(obj)) {
            ans_ = std::make_shared<Number>(abs(As<Number>(obj)->GetValue()));
        } else {
            throw RuntimeError("Not int in abs");
        }
    }
};

class MinMax : public Function {
public:
    std::shared_ptr<Number> ans_ = nullptr;
    std::function<int64_t(int64_t, int64_t)> op_;
    MinMax() = delete;
    MinMax(std::function<int64_t(int64_t, int64_t)> op, std::string name)
        : Function(0, std::make_shared<Number>(0), name) {
        op_ = op;
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<MinMax>(op_, name_);
    }
    std::shared_ptr<Object> Eval() override {
        if (ans_) {
            return ans_;
        } else {
            throw RuntimeError("No argument in maxmin operator");
        }
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (!Is<Number>(obj)) {
            throw RuntimeError("Not int in ArithmeticOperatorint");
        }
        if (ans_) {
            ans_ = std::make_shared<Number>(op_(ans_->GetValue(), As<Number>(obj)->GetValue()));
        } else {
            ans_ = As<Number>(obj);
        }
    }
};
// boolean
class BooleanPredicate : public Function {
public:
    std::shared_ptr<Boolean> ans_;
    BooleanPredicate() : Function(1, nullptr, "boolean?"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<BooleanPredicate>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Boolean>(obj)) {
            ans_ = std::make_shared<Boolean>(true);
        } else {
            ans_ = std::make_shared<Boolean>(false);
        }
    }
};

class Not : public Function {
public:
    std::shared_ptr<Boolean> ans_;
    Not() : Function(1, nullptr, "not"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Not>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Boolean>(obj)) {
            ans_ = std::make_shared<Boolean>(!As<Boolean>(obj)->GetValue());
        } else {
            ans_ = std::make_shared<Boolean>(false);
        }
    }
};

class And : public Function {
public:
    std::shared_ptr<Object> ans_ = std::make_shared<Boolean>(1);
    And() : Function(0, std::make_shared<Number>(0), "and") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<And>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        if (Is<Boolean>(ans_) && (!As<Boolean>(ans_)->GetValue())) {
            return;
        }
        obj = obj->Eval();
        if (!Is<Boolean>(obj)) {
            ans_ = obj;
            return;
        }
        ans_ = std::make_shared<Boolean>(As<Boolean>(obj)->GetValue());
    }
};

class Or : public Function {
public:
    std::shared_ptr<Object> ans_ = std::make_shared<Boolean>(0);
    Or() : Function(0, std::make_shared<Number>(0), "or") {
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Or>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        if (Is<Boolean>(ans_) && (As<Boolean>(ans_)->GetValue())) {
            return;
        }
        obj = obj->Eval();
        if (!Is<Boolean>(obj)) {
            ans_ = obj;
            return;
        }
        ans_ = std::make_shared<Boolean>(As<Boolean>(obj)->GetValue());
    }
};
////////////////////////////////////////////////////////////////////////////////////
// list
class Cell : public Object {
public:
    static std::vector<std::shared_ptr<Object>> Unwarp(std::shared_ptr<Cell> c) {
        std::vector<std::shared_ptr<Object>> unwarp;
        while (Is<Cell>(c->second_)) {
            unwarp.push_back(c->first_);
            c = As<Cell>(c->second_);
        }
        if (c->second_ == nullptr) {
            unwarp.push_back(c->first_);
        } else {
            unwarp.push_back(c);
        }
        return unwarp;
    }

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
    std::string Serialization() override {
        if (Is<Cell>(second_)) {
            auto unwarp = Unwarp(As<Cell>(shared_from_this()));
            std::string serial = "(";
            for (size_t i = 0; i < unwarp.size(); ++i) {
                if (Is<Cell>(unwarp[i])) {
                    serial += As<Cell>(unwarp[i])->first_->Serialization();
                    serial += " . ";
                    serial += As<Cell>(unwarp[i])->second_->Serialization();
                } else {
                    serial += unwarp[i]->Serialization();
                }
                serial += " ";
            }
            serial.pop_back();
            serial += ")";
            return serial;
        } else if (second_) {
            std::string serial = "(";
            serial += first_->Serialization();
            serial += " . ";
            serial += second_->Serialization();
            serial += ")";
            return serial;
        } else if (first_) {
            std::string serial = "(";
            serial += first_->Serialization();
            serial += ")";
            return serial;
        } else {
            return "(())";
        }
    }
    std::shared_ptr<Object> Clone() override {
        std::shared_ptr<Object> clone = std::make_shared<Cell>();
        As<Cell>(clone)->first_ = first_->Clone();
        As<Cell>(clone)->second_ = second_->Clone();
        return clone;
    }
    std::shared_ptr<Object> Eval() override {
        if (first_ == nullptr) {
            throw RuntimeError("Not self-evaluated");
        }
        auto unwarp = Unwarp(As<Cell>(shared_from_this()));
        if (!Is<Symbol>(unwarp[0])) {
            throw RuntimeError("Not self-evaluated");
        }
        unwarp[0] = unwarp[0]->Eval();
        if (Is<Function>(unwarp[0])) {
            if (As<Function>(unwarp[0])->arguments_ == 0 ||
                (As<Function>(unwarp[0])->arguments_ + 1 == unwarp.size())) {
                for (size_t i = 1; i < unwarp.size(); ++i) {
                    if (unwarp[i]) {
                        unwarp[0]->Imply(unwarp[i]);
                        } else {
                        throw RuntimeError("Blank argument");
                        }
                }
            } else {
                throw RuntimeError("Incorrect number of arguments");
            }
            return unwarp[0]->Eval();
        } else {
            throw RuntimeError("Not self-evaluated");
        }
    }
};
class PairPredicate : public Function {
public:
    std::shared_ptr<Boolean> ans_;
    PairPredicate() : Function(1, nullptr, "pair?"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<PairPredicate>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Cell>(obj)) {
            auto c = Cell::Unwarp(As<Cell>(obj));
            if (c.size() == 2) {
                ans_ = std::make_shared<Boolean>(1);
            } else if (c.size() == 1 && Is<Cell>(c[0])) {
                if (As<Cell>(c[0])->first_ != nullptr && As<Cell>(c[0])->second_ != nullptr) {
                    ans_ = std::make_shared<Boolean>(1);
                } else {
                    ans_ = std::make_shared<Boolean>(0);
                }
            } else {
                ans_ = std::make_shared<Boolean>(0);
            }
        } else {
            ans_ = std::make_shared<Boolean>(0);
        }
    }
};

class NullPredicate : public Function {
public:
    std::shared_ptr<Boolean> ans_;
    NullPredicate() : Function(1, nullptr, "null?"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<NullPredicate>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (obj == nullptr) {
            ans_ = std::make_shared<Boolean>(1);
        } else {
            ans_ = std::make_shared<Boolean>(0);
        }
    }
};

class ListPredicate : public Function {
public:
    std::shared_ptr<Boolean> ans_;
    ListPredicate() : Function(1, nullptr, "list?"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<ListPredicate>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (obj == nullptr) {
            ans_ = std::make_shared<Boolean>(1);
        } else {
            if (Is<Cell>(obj)) {
                auto c = Cell::Unwarp(As<Cell>(obj));
                bool k = true;
                for (size_t i = 0; i < c.size(); ++i) {
                    k = k && !Is<Cell>(c[i]);
                }
                ans_ = std::make_shared<Boolean>(k);
            }
        }
    }
};

class Cons : public Function {
public:
    std::shared_ptr<Cell> ans_ = nullptr;
    Cons() : Function(2, nullptr, "cons"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Cons>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (ans_) {
            ans_->second_ = obj;
        } else {
            ans_ = std::make_shared<Cell>();
            ans_->first_ = obj;
        }
    }
};

class Car : public Function {
public:
    std::shared_ptr<Object> ans_ = nullptr;
    Car() : Function(1, nullptr, "car"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Car>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Cell>(obj)) {
            if (As<Cell>(obj)->first_) {
                ans_ = As<Cell>(obj)->first_;
            } else {
                throw RuntimeError("wrong argument");
            }
        } else {
            throw RuntimeError("wrong argument");
        }
    }
};
class Cdr : public Function {
public:
    std::shared_ptr<Object> ans_ = nullptr;
    Cdr() : Function(1, nullptr, "cdr"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Cdr>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (Is<Cell>(obj)) {
            if (As<Cell>(obj)->second_) {
                ans_ = As<Cell>(obj)->second_;
            } else {
                ans_ = nullptr;
            }
        } else {
            throw RuntimeError("wrong argument");
        }
    }
};
class List : public Function {
public:
    std::shared_ptr<Cell> root = nullptr;
    std::shared_ptr<Cell> cur_root = nullptr;
    List() : Function(0, nullptr, "list"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<List>();
    }
    std::shared_ptr<Object> Eval() override {
        return root;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (root) {
            cur_root->second_ = std::make_shared<Cell>();
            cur_root = As<Cell>(cur_root->second_);
            cur_root->first_ = obj;
        } else {
            root = std::make_shared<Cell>();
            root->first_ = obj;
            cur_root = root;
        }
    }
};
class ListRef : public Function {
public:
    std::shared_ptr<Cell> root = nullptr;
    std::shared_ptr<Object> ans = nullptr;
    ListRef() : Function(2, nullptr, "list-ref"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<ListRef>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (root) {
            if (Is<Number>(obj)) {
                auto c = Cell::Unwarp(root);
                if (static_cast<size_t>(static_cast<size_t>(As<Number>(obj)->GetValue())) >=
                    c.size()) {
                    throw RuntimeError("wrong size");
                }
                ans = c[As<Number>(obj)->GetValue()];
            } else {
                throw RuntimeError("Wrong argument");
            }
        } else {
            auto check = std::make_shared<ListPredicate>();
            check->Imply(std::make_shared<Quote>(obj));
            if (As<Boolean>(check->Eval())->GetValue()) {
                root = As<Cell>(obj);
            } else {
                throw RuntimeError("Wrong argument");
            }
        }
    }
};
class ListTail : public Function {
public:
    std::shared_ptr<Cell> root = nullptr;
    std::shared_ptr<Object> ans = nullptr;
    ListTail() : Function(2, nullptr, "list-tail"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<ListTail>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = obj->Eval();
        if (root) {
            if (Is<Number>(obj)) {
                ans = root;
                for (int i = 0; i < As<Number>(obj)->GetValue(); ++i) {
                    if (ans == nullptr) {
                        throw RuntimeError("wrong size");
                    }
                    ans = As<Cell>(ans)->second_;
                }
            } else {
                throw RuntimeError("Wrong argument");
            }
        } else {
            auto check = std::make_shared<ListPredicate>();
            check->Imply(std::make_shared<Quote>(obj));
            if (As<Boolean>(check->Eval())->GetValue()) {
                root = As<Cell>(obj);
            } else {
                throw RuntimeError("Wrong argument");
            }
        }
    }
};

// class only for parsing, not used in further program
class Dot : public Object {
public:
    Dot() = default;
};

class CloseBracket : public Object {
public:
    CloseBracket() = default;
};

// MAIN PART
static std::unordered_map<std::string, std::shared_ptr<Function>> functions{
    {"number?", std::make_shared<IntegerPredicate>()},
    {"+", std::make_shared<Plus>()},
    {"-", std::make_shared<Minus>()},
    {"*", std::make_shared<Multiply>()},
    {"/", std::make_shared<Divide>()},
    {"=", std::make_shared<CompareOperator>(std::equal_to<int64_t>(), "=")},
    {">", std::make_shared<CompareOperator>(std::greater<int64_t>(), ">")},
    {">=", std::make_shared<CompareOperator>(std::greater_equal<int64_t>(), ">-")},
    {"<", std::make_shared<CompareOperator>(std::less<int64_t>(), "<")},
    {"<=", std::make_shared<CompareOperator>(std::less_equal<int64_t>(), "<=")},
    {"abs", std::make_shared<Abs>()},
    {"min", std::make_shared<MinMax>([](int64_t a, int64_t b) -> int64_t { return std::min(a, b); },
                                     "min")},
    {"max", std::make_shared<MinMax>([](int64_t a, int64_t b) -> int64_t { return std::max(a, b); },
                                     "max")},
    {"boolean?", std::make_shared<BooleanPredicate>()},
    {"not", std::make_shared<Not>()},
    {"and", std::make_shared<And>()},
    {"or", std::make_shared<Or>()},
    {"pair?", std::make_shared<PairPredicate>()},
    {"null?", std::make_shared<NullPredicate>()},
    {"list?", std::make_shared<ListPredicate>()},
    {"cons", std::make_shared<Cons>()},
    {"car", std::make_shared<Car>()},
    {"cdr", std::make_shared<Cdr>()},
    {"list", std::make_shared<List>()},
    {"list-ref", std::make_shared<ListRef>()},
    {"list-tail", std::make_shared<ListTail>()}};

class Symbol : public Object {
public:
    Symbol(const std::string& name) : name_(name) {
    }
    const std::string& GetName() const {
        return name_;
    }
    std::string Serialization() override {
        return name_;
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Symbol>(name_);
    }
    std::shared_ptr<Object> Eval() override {
        if (functions.contains(name_)) {
            return functions[name_]->Clone();
        } else if (name_ == "#t") {
            return std::make_shared<Boolean>(1);
        } else if (name_ == "#f") {
            return std::make_shared<Boolean>(0);
        } else {
            return shared_from_this();
        }
    }

private:
    std::string name_;
};