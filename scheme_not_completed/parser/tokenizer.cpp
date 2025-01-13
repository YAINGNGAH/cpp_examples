#include <tokenizer.h>
#include <error.h>
// token operators

bool SymbolToken::operator==(const SymbolToken &other) const {
    if (name == other.name) {
        return true;
    }
    return false;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    if (value == other.value) {
        return true;
    }
    return false;
}

// Tokenizer

Tokenizer::Tokenizer(std::istream *in) : in_(in) {
    is_end_ = false;
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

Token Tokenizer::GetToken() {
    return last_token_;
}

void Tokenizer::Next() {
    char c;
    if (in_->get(c)) {
        // space symbols
        if (isspace(c)) {
            while (isspace(c)) {
                if (in_->get(c)) {
                } else {
                    is_end_ = true;
                    return;
                }
            }
        }
        // nums
        if (isdigit(c)) {
            ConstantToken token;
            token.value = c - '0';
            while (in_->get(c)) {
                if (!isdigit(c)) {
                    in_->unget();
                    break;
                }
                token.value *= 10;
                token.value += c - '0';
            }
            last_token_ = token;
        } else if (c == '\'') {
            last_token_ = QuoteToken();
        } else if (c == '.') {
            last_token_ = DotToken();
        } else if (c == '(' || c == ')') {
            if (c == '(') {
                last_token_ = BracketToken::OPEN;
            } else {
                last_token_ = BracketToken::CLOSE;
            }
        } else if (std::regex_match(std::string(1, c), symbol_begin)) {
            SymbolToken token;
            token.name += c;
            while (in_->get(c)) {
                if (!std::regex_match(std::string(1, c), symbol)) {
                    in_->unget();
                    break;
                }
                token.name += c;
            }
            last_token_ = token;
        } else if (c == '+' || c == '-') {
            // two variants
            bool minus = false;
            minus = (c == '-');
            if (in_->get(c)) {
                if (isdigit(c)) {
                    ConstantToken token;
                    token.value = c - '0';
                    while (in_->get(c)) {
                        if (!isdigit(c)) {
                            in_->unget();
                            break;
                        }
                        token.value *= 10;
                        token.value += c - '0';
                    }
                    if (minus) {
                        token.value *= -1;
                    }
                    last_token_ = token;
                } else {
                    in_->unget();
                    SymbolToken token;
                    if (minus) {
                        token.name = '-';
                    } else {
                        token.name = '+';
                    }
                    last_token_ = token;
                }
            } else {
                SymbolToken token;
                if (minus) {
                    token.name = '-';
                } else {
                    token.name = '+';
                }
                last_token_ = token;
            }
        } else {
            throw SyntaxError("No such symbol");
        }
    } else {
        is_end_ = true;
        return;
    }
}
