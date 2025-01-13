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


std::shared_ptr<Object> Evaluation(std::shared_ptr<Object> obj);
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

class Reference : public Object{
public:
    std::shared_ptr<Object> val_;
    Reference(std::shared_ptr<Object> val) : val_(val){
    }
    std::string Serialization() override{
        return val_->Serialization();
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<Reference>(val_->Clone());
    }
    std::shared_ptr<Object> Eval() override{
        return val_;
    }
};

class Scope : public Object{
private:
    std::unordered_map<std::string, std::shared_ptr<Reference>> variables_;
public:
    Scope() = default;
    Scope(std::unordered_map<std::string, std::shared_ptr<Reference>> var) : variables_(var){
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<Scope>(variables_);
    }
    void Define(const std::string& name, std::shared_ptr<Object> val){
        variables_[name] = std::make_shared<Reference>(val);
    }
    void Set(const std::string& name, std::shared_ptr<Object> val){
        if (variables_.contains(name)) {
            variables_[name] = std::make_shared<Reference>(val);
        } else{
            throw NameError("No such name");
        }
    }
    std::shared_ptr<Reference> Get(const std::string& name){
        if (variables_.contains(name)){
            return variables_[name];
        }else{
            throw NameError("No such name");
        }
    }
    bool Contain(const std::string& name){
        if (variables_.contains(name)){
            return true;
        }else{
            return false;
        }
    }
};

class Symbol : public Object {
public:
    std::shared_ptr<Scope> scope_;
    Symbol(const std::string& name, std::shared_ptr<Scope> scope) : scope_(scope), name_(name) {
    }
    const std::string& GetName() const {
        return name_;
    }
    std::string Serialization() override {
        return name_;
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Symbol>(name_, scope_);
    }
    std::shared_ptr<Object> Eval() override;

private:
    std::string name_;
};


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
public:
    std::shared_ptr<Object> ans_;
    Quote(std::shared_ptr<Object> a) : ans_(a) {
    }
    std::string Serialization() override {
        if (ans_ == nullptr) {
            return "(quote ())";
        }
        return "(quote " + ans_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Quote>(ans_->Clone());
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
};

class IfForm: public SpecialForm{
public:
    std::shared_ptr<Object> condition_;
    std::shared_ptr<Object> truthbranch_;
    std::shared_ptr<Object> falsebranch_;
    IfForm(std::shared_ptr<Object> c, std::shared_ptr<Object> t, std::shared_ptr<Object> f = nullptr){
        condition_ = c;
        truthbranch_ = t;
        falsebranch_ = f;
    }
    std::string Serialization() override{
        if (falsebranch_) {
            return "(if " + condition_->Serialization() + " " + truthbranch_->Serialization() +
                   " " + falsebranch_->Serialization() + ")";
        }else{
            return "(if " + condition_->Serialization() + " " + truthbranch_->Serialization() + ")";
        }
    }
    std::shared_ptr<Object> Clone() override{
        if (falsebranch_) {
            return std::make_shared<IfForm>(condition_->Clone(), truthbranch_->Clone(),
                                            falsebranch_->Clone());
        }else{
            return std::make_shared<IfForm>(condition_->Clone(), truthbranch_->Clone());
        }
    }
    std::shared_ptr<Object> Eval() override{
        condition_ = Evaluation(condition_);
        if (Is<Boolean>(condition_)){
            if (As<Boolean>(condition_)->GetValue()){
                return truthbranch_->Eval();
            }else{
                if (falsebranch_){
                    return falsebranch_->Eval();
                }else{
                    return nullptr;
                }
            }
        }else{
            throw RuntimeError("not bool condition");
        }
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        if (second_) {
            As<Cell>(clone)->second_ = second_->Clone();
        }else{
            As<Cell>(clone)->second_ = nullptr;
        }
        return clone;
    }
    std::shared_ptr<Object> Eval() override;
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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
        obj = Evaluation(obj);
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

//symbol
class SymbolPredicate : public Function{
public:
    std::shared_ptr<Boolean> ans_;
    SymbolPredicate() : Function(1, nullptr, "symbol?"){};
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<SymbolPredicate>();
    }
    std::shared_ptr<Object> Eval() override {
        return ans_;
    }
    void Imply(std::shared_ptr<Object> obj) override {
        obj = Evaluation(obj);
        if (Is<Symbol>(obj)) {
            ans_ = std::make_shared<Boolean>(true);
        } else {
            ans_ = std::make_shared<Boolean>(false);
        }
    }
};


class Define : public SpecialForm{
public:
    std::shared_ptr<Object> name_;
    std::shared_ptr<Object> value_;
    std::shared_ptr<Scope> scope_;
    Define(std::shared_ptr<Object> name, std::shared_ptr<Object> value, std::shared_ptr<Scope> scope) : name_(name), value_(value), scope_(scope){
    }
    std::string Serialization() override{
        return "(define " + name_->Serialization() + " " + value_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<Define>(name_, value_, scope_);
    }
    std::shared_ptr<Object> Eval() override;
};
class Set : public SpecialForm{
public:
    std::shared_ptr<Object> ref_;
    std::shared_ptr<Object> value_;
    std::shared_ptr<Scope> scope_;
    Set(std::shared_ptr<Object> name, std::shared_ptr<Object> value, std::shared_ptr<Scope> scope) : ref_(name), value_(value), scope_(scope){
    }
    std::string Serialization() override{
        return "(set! " + ref_->Serialization() + " " + value_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<Set>(ref_, value_, scope_);
    }
    std::shared_ptr<Object> Eval() override{
        ref_ = ref_->Eval();
        if (Is<Reference>(ref_)) {
            As<Reference>(ref_)->val_ = value_->Eval();
            return nullptr;
        }
        throw NameError("Not a symbol");
    }
};
class SetCar : public SpecialForm{
public:
    std::shared_ptr<Object> ref_;
    std::shared_ptr<Object> value_;
    std::shared_ptr<Scope> scope_;
    SetCar(std::shared_ptr<Object> name, std::shared_ptr<Object> value, std::shared_ptr<Scope> scope) : ref_(name), value_(value), scope_(scope){
    }
    std::string Serialization() override{
        return "(set-car! " + ref_->Serialization() + " " + value_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<SetCar>(ref_, value_, scope_);
    }
    std::shared_ptr<Object> Eval() override{
        ref_ = Evaluation(ref_);
        if (Is<Cell>(ref_)) {
            As<Cell>(ref_)->first_ = value_->Eval();
            return nullptr;
        }else{
            throw NameError("Not a cell");
        }
    }
};
class SetCdr : public SpecialForm{
public:
    std::shared_ptr<Object> ref_;
    std::shared_ptr<Object> value_;
    std::shared_ptr<Scope> scope_;
    SetCdr(std::shared_ptr<Object> name, std::shared_ptr<Object> value, std::shared_ptr<Scope> scope) : ref_(name), value_(value), scope_(scope){
    }
    std::string Serialization() override{
        return "(set-cdr! " + ref_->Serialization() + " " + value_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<SetCdr>(ref_, value_, scope_);
    }
    std::shared_ptr<Object> Eval() override{
        ref_ = Evaluation(ref_);
        if (Is<Cell>(ref_)) {
            As<Cell>(ref_)->second_ = value_->Eval();
            return nullptr;
        }else{
            throw NameError("Not a cell");
        }
    }
};
void SetScope(std::shared_ptr<Object> obj,std::shared_ptr<Scope> scope);

class LambdaFunction : public Function{
public:
    std::shared_ptr<Scope> scope_;
    std::vector<std::shared_ptr<Object>> args_;
    std::vector<std::shared_ptr<Object>> body_;
    LambdaFunction(const std::vector<std::shared_ptr<Object>>& args, const std::vector<std::shared_ptr<Object>>& body, std::shared_ptr<Scope> scope = nullptr) : Function(args.size(), nullptr, "lambda"),scope_(scope), args_(args), body_(body){
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<LambdaFunction>(args_, body_, scope_);
    }
    std::shared_ptr<Object> Eval() override{
        std::shared_ptr<Object> ans;
        for (size_t i = 0; i < body_.size(); ++i) {
            ans = body_[i]->Eval();
        }
        return ans;
    }
    void Imply(std::shared_ptr<Object> obj) override{
        obj = obj->Eval();
        for (size_t i = 0; i < args_.size(); ++i) {
            if (!scope_->Contain(As<Symbol>(args_[i])->GetName())){
                scope_->Define(As<Symbol>(args_[i])->GetName(), obj);
                break;
            }
        }
    }
};

class Lambda : public SpecialForm{
public:
    std::shared_ptr<Object> arguments_;
    std::shared_ptr<Object> body_;
    std::shared_ptr<Scope> scope_;
    std::shared_ptr<Symbol> name_;
    Lambda(std::shared_ptr<Object> args, std::shared_ptr<Object> body, std::shared_ptr<Scope> scope = nullptr) : arguments_(args), body_(body), scope_(scope){
    }
    std::string Serialization() override{
        return "(lambda (" + arguments_->Serialization() + ") " + body_->Serialization() + ")";
    }
    std::shared_ptr<Object> Clone() override{
        return std::make_shared<Lambda>(arguments_, body_->Clone(), scope_);
    }
    std::shared_ptr<Object> Eval() override{
        if (arguments_) {
            auto args = Cell::Unwarp(As<Cell>(arguments_));
            for (size_t i = 0; i < args.size(); ++i) {
                if (!Is<Symbol>(args[i])) {
                    throw NameError("Not a symbol");
                }
            }
            std::vector<std::shared_ptr<Object>> body;
            if (Is<Cell>(body_)){
                body = Cell::Unwarp(As<Cell>(body_->Clone()));
            }else{
                body.push_back(body_);
            }
            auto scope = std::make_shared<Scope>();
            if (name_){
                auto clone = std::make_shared<Lambda>(arguments_->Clone(), body_->Clone(), scope);
                clone->name_ = name_;
                scope->Define(name_->GetName(), clone);
            }
            for (size_t i = 0; i < body.size(); ++i) {
                SetScope(body[i], scope);
            }
            auto f = std::make_shared<LambdaFunction>(args, body, scope);
            return f;
        }else{
            std::vector<std::shared_ptr<Object>> args;
            std::vector<std::shared_ptr<Object>> body;
            if (Is<Cell>(body_)){
                body = Cell::Unwarp(As<Cell>(body_));
            }else{
                body.push_back(body_);
            }
            for (size_t i = 0; i < body.size(); ++i) {
                SetScope(body[i], scope_);
            }
            return std::make_shared<LambdaFunction>(args, body, scope_);
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
    {"symbol?", std::make_shared<SymbolPredicate>()},
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





