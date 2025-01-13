#include "cmdcheckfunctions.h"
#include <sstream>

bool IsNumber(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) {
        ++it;
    }
    return !s.empty() && it == s.end();
}

bool IsDouble(const std::string& s) {
    long double ld = 0;
    return ((std::istringstream(s) >> ld >> std::ws).eof()) && s[0] != '-';
}