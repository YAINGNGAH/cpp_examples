#include <parser.h>
#include <error.h>

bool Check(std::shared_ptr<Object> obj){
    if (Is<Cell>(obj) || Is<Number>(obj) || Is<Symbol>(obj) || Is<SpecialForm>(obj)||
        obj == nullptr){
        return true;
    }
    return false;
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Incorrect Syntax");
    }
    Token token = (*tokenizer).GetToken();
    tokenizer->Next();
    if (ConstantToken* consttoken = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(consttoken->value);
    } else if (SymbolToken* symtoken = std::get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(symtoken->name, nullptr);
    } else if (std::get_if<DotToken>(&token)) {
        throw SyntaxError("Incorrect Syntax");
    } else if (std::get_if<QuoteToken>(&token)) {
        std::shared_ptr<Object> next_object = ReadRecursive(tokenizer);
        if (Is<Cell>(next_object) || Is<Number>(next_object) || Is<Symbol>(next_object) || Is<SpecialForm>(next_object)||
            next_object == nullptr) {
            std::shared_ptr<Object> quote = std::make_shared<Quote>(next_object);
            return quote;
        } else {
            throw SyntaxError("Incorrect Syntax");
        }
    } else if (BracketToken* brackettoken = std::get_if<BracketToken>(&token)) {
        if ((*brackettoken) == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            throw SyntaxError("Incorrect Syntax");
        }
    } else {
        throw SyntaxError("No such token");
    }
}

std::shared_ptr<Object> ReadRecursive(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Incorrect Syntax");
    }
    Token token = (*tokenizer).GetToken();
    tokenizer->Next();
    if (ConstantToken* consttoken = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(consttoken->value);
    } else if (SymbolToken* symtoken = std::get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(symtoken->name, nullptr);
    } else if (std::get_if<DotToken>(&token)) {
        return std::make_shared<Dot>();
    } else if (std::get_if<QuoteToken>(&token)) {
        std::shared_ptr<Object> next_object = ReadRecursive(tokenizer);
        if (Is<Cell>(next_object) || Is<Number>(next_object) || Is<Symbol>(next_object) || Is<SpecialForm>(next_object)||
            next_object == nullptr) {
            std::shared_ptr<Object> quote = std::make_shared<Quote>(next_object);
            return quote;
        } else {
            throw SyntaxError("Incorrect Syntax");
        }
    } else if (BracketToken* brackettoken = std::get_if<BracketToken>(&token)) {
        if ((*brackettoken) == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            return std::make_shared<CloseBracket>();
        }
    } else {
        throw SyntaxError("No such token");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Incorrect Syntax");
    }
    std::shared_ptr<Object> cur_obj = ReadRecursive(tokenizer);
    if (!Is<CloseBracket>(cur_obj)) {
        std::shared_ptr<Object> root = std::make_shared<Cell>();
        std::shared_ptr<Object> cur_root = root;
        bool bullshitpair = false;
        while (!Is<CloseBracket>(cur_obj)) {
            if (bullshitpair) {
                throw SyntaxError("Incorrect Syntax");
            }
            if (Is<Symbol>(cur_obj)){
                if (As<Symbol>(cur_obj)->GetName() == "quote"){
                    if (cur_root != root){
                        throw SyntaxError("Incorrect Syntax");
                    }else{
                        cur_obj = ReadRecursive(tokenizer);
                        if (Check(cur_obj)) {
                            std::shared_ptr<Object> quote = std::make_shared<Quote>(cur_obj);
                            cur_obj = ReadRecursive(tokenizer);
                            if (!Is<CloseBracket>(cur_obj)){
                                throw SyntaxError("Incorrect Syntax");
                            } else{
                                return quote;
                            }
                        } else {
                            throw SyntaxError("Incorrect Syntax");
                        }
                    }
                }else if(As<Symbol>(cur_obj)->GetName() == "if"){
                    std::shared_ptr<IfForm> form;
                    auto cond = ReadRecursive(tokenizer);
                    if (!Check(cond)){ throw SyntaxError("Incorrect Syntax");}
                    auto truth = ReadRecursive(tokenizer);
                    if (!Check(truth)) {throw SyntaxError("Incorrect Syntax");}
                    auto falth = ReadRecursive(tokenizer);
                    if (!Is<CloseBracket>(falth) && !Check(falth)) {throw SyntaxError("Incorrect Syntax");}
                    if (!Is<CloseBracket>(falth)) {
                        form = std::make_shared<IfForm>(cond, truth, falth);
                        cur_obj = ReadRecursive(tokenizer);
                        if (!Is<CloseBracket>(cur_obj)){
                            throw SyntaxError("Incorrect Syntax");
                        }else{
                            return form;
                        }
                    } else {
                        form = std::make_shared<IfForm>(cond, truth);
                        if (!Is<CloseBracket>(falth)){
                            throw SyntaxError("Incorrect Syntax");
                        }else{
                            return form;
                        }
                    }
                }else if(As<Symbol>(cur_obj)->GetName() == "define"){
                    std::shared_ptr<Define> form;
                    std::shared_ptr<Object> name = ReadRecursive(tokenizer);
                    if (!Check(name)){
                        throw SyntaxError("Sisyphus");
                    }
                    std::shared_ptr<Object> val = ReadRecursive(tokenizer);
                    if (!Check(val)){
                        throw SyntaxError("Sisyphus");
                    }
                    form = std::make_shared<Define>(name, val, nullptr);
                    cur_obj = ReadRecursive(tokenizer);
                    if (!Is<CloseBracket>(cur_obj)){
                        throw SyntaxError("i am tired");
                    }else{
                        return form;
                    }
                }else if(As<Symbol>(cur_obj)->GetName() == "set!"){
                    std::shared_ptr<Set> form;
                    std::shared_ptr<Object> name = ReadRecursive(tokenizer);
                    if (!Check(name)){
                        throw SyntaxError("Sisyphus");
                    }
                    std::shared_ptr<Object> val = ReadRecursive(tokenizer);
                    if (!Check(val)){
                        throw SyntaxError("Sisyphus");
                    }
                    form = std::make_shared<Set>(name, val, nullptr);
                    cur_obj = ReadRecursive(tokenizer);
                    if (!Is<CloseBracket>(cur_obj)){
                        throw SyntaxError("i am tired");
                    }else{
                        return form;
                    }
                }else if(As<Symbol>(cur_obj)->GetName() == "set-car!"){
                    std::shared_ptr<SetCar> form;
                    std::shared_ptr<Object> name = ReadRecursive(tokenizer);
                    if (!Check(name)){
                        throw SyntaxError("Sisyphus");
                    }
                    std::shared_ptr<Object> val = ReadRecursive(tokenizer);
                    if (!Check(val)){
                        throw SyntaxError("Sisyphus");
                    }
                    form = std::make_shared<SetCar>(name, val, nullptr);
                    cur_obj = ReadRecursive(tokenizer);
                    if (!Is<CloseBracket>(cur_obj)){
                        throw SyntaxError("i am tired");
                    }else{
                        return form;
                    }
                }else if(As<Symbol>(cur_obj)->GetName() == "set-cdr!"){
                    std::shared_ptr<SetCdr> form;
                    std::shared_ptr<Object> name = ReadRecursive(tokenizer);
                    if (!Check(name)){
                        throw SyntaxError("Sisyphus");
                    }
                    std::shared_ptr<Object> val = ReadRecursive(tokenizer);
                    if (!Check(val)){
                        throw SyntaxError("Sisyphus");
                    }
                    form = std::make_shared<SetCdr>(name, val, nullptr);
                    cur_obj = ReadRecursive(tokenizer);
                    if (!Is<CloseBracket>(cur_obj)){
                        throw SyntaxError("i am tired");
                    }else{
                        return form;
                    }
                }else if(As<Symbol>(cur_obj)->GetName() == "lambda"){
                    std::shared_ptr<Object> arguments = ReadRecursive(tokenizer);
                    if (!Is<Cell>(arguments) && arguments != nullptr){
                        throw SyntaxError("Incorrect arguments of function");
                    }
                    if (Is<CloseBracket>(arguments)){
                        throw SyntaxError("Incorrect arguments of function");
                    }
                    std::shared_ptr<Object> body = ReadList(tokenizer);
                    if (!Check(body) || body == nullptr){throw SyntaxError("I am tired");}
                    std::shared_ptr<Lambda> lambda = std::make_shared<Lambda>(As<Cell>(arguments), body);
                    return lambda;
                }else{
                    As<Cell>(cur_root)->first_ = cur_obj;
                }
            }
            else if (Check(cur_obj)) {
                As<Cell>(cur_root)->first_ = cur_obj;
            } else {
                throw SyntaxError("Incorrect Syntax");
            }
            if (tokenizer->IsEnd()) {
                throw SyntaxError("Incorrect expression");
            }
            cur_obj = ReadRecursive(tokenizer);
            if (Is<CloseBracket>(cur_obj)) {
                As<Cell>(cur_root)->second_ = nullptr;
            } else if (Is<Dot>(cur_obj)) {
                cur_obj = ReadRecursive(tokenizer);
                if (!Is<CloseBracket>(cur_obj)) {
                    As<Cell>(cur_root)->second_ = cur_obj;
                } else {
                    throw SyntaxError("Incorrect expression");
                }
                cur_obj = ReadRecursive(tokenizer);
                bullshitpair = true;
            } else {
                std::shared_ptr<Object> new_root = std::make_shared<Cell>();
                As<Cell>(cur_root)->second_ = new_root;
                cur_root = new_root;
            }
        }
        return root;
    } else {
        return nullptr;
    }
}
