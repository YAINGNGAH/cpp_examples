#include <parser.h>
#include <error.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Incorrect Syntax");
    }
    Token token = (*tokenizer).GetToken();
    tokenizer->Next();
    if (ConstantToken* consttoken = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(consttoken->value);
    } else if (SymbolToken* symtoken = std::get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(symtoken->name);
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
        return std::make_shared<Symbol>(symtoken->name);
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
                        if (Is<Cell>(cur_obj) || Is<Number>(cur_obj) || Is<Symbol>(cur_obj) || Is<SpecialForm>(cur_obj)||
                            cur_obj == nullptr) {
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
                }else{
                    As<Cell>(cur_root)->first_ = cur_obj;
                }
            }
            else if (Is<Cell>(cur_obj) || Is<Number>(cur_obj) || Is<SpecialForm>(cur_obj) ||
                cur_obj == nullptr) {
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
