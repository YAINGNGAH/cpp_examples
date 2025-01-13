#pragma once

#include "tokenizer.h"
#include "parser.h"
#include "object.h"
#include <string>

class Interpreter {
public:
    std::string Run(const std::string&);
};
