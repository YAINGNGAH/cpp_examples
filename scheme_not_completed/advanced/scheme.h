#pragma once

#include "tokenizer.h"
#include "parser.h"
#include "object.h"
#include <string>

class Interpreter {
private:
    std::shared_ptr<Scope> global_ = std::make_shared<Scope>();
public:
    std::string Run(const std::string&);
};
