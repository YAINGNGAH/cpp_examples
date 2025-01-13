#include "object.h"

std::shared_ptr<Object> Evaluation(std::shared_ptr<Object> obj){
    obj = obj->Eval();
    while (Is<Reference>(obj)){
        obj = obj->Eval();
    }
    return obj;
}

std::shared_ptr<Object> Cell::Eval() {
    if (first_ == nullptr) {
        throw RuntimeError("Not self-evaluated");
    }
    auto unwarp = Unwarp(As<Cell>(shared_from_this()));
    unwarp[0] = Evaluation(unwarp[0]);
    if (Is<Lambda>(unwarp[0])){
        unwarp[0] = Evaluation(unwarp[0]);
    }
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
std::shared_ptr<Object> Symbol::Eval() {
    if (functions.contains(name_)) {
        return functions[name_]->Clone();
    } else if(scope_->Contain(name_)){
        return scope_->Get(name_);
    } else if (name_ == "#t") {
        return std::make_shared<Boolean>(1);
    } else if (name_ == "#f") {
        return std::make_shared<Boolean>(0);
    } else {
        throw NameError("not self evaluated");
    }
}
std::shared_ptr<Object> Define::Eval() {
    if (Is<Symbol>(name_)) {
        if (Is<Lambda>(value_)){
            As<Lambda>(value_)->name_ = As<Symbol>(name_);
            scope_->Define(As<Symbol>(name_)->GetName(), value_);
            return nullptr;
        }
        scope_->Define(As<Symbol>(name_)->GetName(), Evaluation(value_));
        return nullptr;
    }
    throw NameError("Not a symbol");
}
void SetScope(std::shared_ptr<Object> obj,std::shared_ptr<Scope> scope) {
    if (Is<SpecialForm>(obj)){
        if (Is<Quote>(obj)){
            SetScope(As<Quote>(obj)->ans_, scope);
        }else if(Is<IfForm>(obj)){
            SetScope(As<IfForm>(obj)->condition_, scope);
            SetScope(As<IfForm>(obj)->truthbranch_, scope);
            SetScope(As<IfForm>(obj)->falsebranch_, scope);
        }else if(Is<Define>(obj)){
            As<Define>(obj)->scope_ = scope;
            SetScope(As<Define>(obj)->name_, scope);
            SetScope(As<Define>(obj)->value_, scope);
        }else if (Is<Set>(obj)){
            As<Set>(obj)->scope_ = scope;
            SetScope(As<Set>(obj)->ref_, scope);
            SetScope(As<Set>(obj)->value_, scope);
        }else if (Is<SetCar>(obj)){
            As<SetCar>(obj)->scope_ = scope;
            SetScope(As<SetCar>(obj)->ref_, scope);
            SetScope(As<SetCar>(obj)->value_, scope);
        }
        else if (Is<SetCdr>(obj)){
            As<SetCdr>(obj)->scope_ = scope;
            SetScope(As<SetCdr>(obj)->ref_, scope);
            SetScope(As<SetCdr>(obj)->value_, scope);
        }else if (Is<Lambda>(obj)){
            As<Lambda>(obj)->scope_ = scope;
            SetScope(As<Lambda>(obj)->arguments_, scope);
            SetScope(As<Lambda>(obj)->body_, scope);
        }
    }else if (Is<Symbol>(obj)){
        As<Symbol>(obj)->scope_ = scope;
    }else if(Is<Cell>(obj)){
        SetScope(As<Cell>(obj)->GetFirst(), scope);
        SetScope(As<Cell>(obj)->GetSecond(), scope);
    }else{
        return;
    }
}