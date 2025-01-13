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
    } else if (DotToken* dottoken = std::get_if<DotToken>(&token)) {
        return std::make_shared<Dot>();
    } else if (QuoteToken* quotetoken = std::get_if<QuoteToken>(&token)) {
        throw SyntaxError("Not implemented for now");
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
    } else if (DotToken* dottoken = std::get_if<DotToken>(&token)) {
        return std::make_shared<Dot>();
    } else if (QuoteToken* quotetoken = std::get_if<QuoteToken>(&token)) {
        throw SyntaxError("Not implemented for now");
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
            As<Cell>(cur_root)->first_ = cur_obj;
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