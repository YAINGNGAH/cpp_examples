#include "password.h"
#include "vector"

const int FIRST_SYMBOL = 33;
const int LAST_SYMBOL = 126;
const int MIN_SIZE = 8;
const int MAX_SIZE = 14;
const int DIGIT_VENT = 8;
const int LOWER_VENT = 4;
const int UPPER_VENT = 2;
const int OTHER_VENT = 1;
const int MIN_NUMBER_OF_TYPES = 3;

bool ValidatePassword(const std::string& password) {
    if (std::size(password) < (MIN_SIZE) or std::size(password) > (MAX_SIZE)) {
        return false;
    }
    int c = 0;
    for (char i : password) {
        if (i < (FIRST_SYMBOL) or i > (LAST_SYMBOL)) {
            return false;
        }
        if (std::isdigit(i)) {
            c = c | (DIGIT_VENT);
            continue;
        } else if (std::islower(i)) {
            c = c | (LOWER_VENT);
            continue;
        } else if (std::isupper(i)) {
            c = c | (UPPER_VENT);
            continue;
        } else {
            c = c | (OTHER_VENT);
        }
    }
    int symb_types = 0;
    while (c) {
        symb_types += c % 2;
        c /= 2;
    }
    if (symb_types < MIN_NUMBER_OF_TYPES) {
        return false;
    }
    return true;
}
