#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <string>
#include <regex>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};
struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;
static std::regex symbol_begin("[a-zA-Z<=>*/#]");
static std::regex symbol("[a-zA-Z<=>*/#0-9?!-]");
class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    std::istream* in_;
    Token last_token_;
    bool is_end_ = true;
};