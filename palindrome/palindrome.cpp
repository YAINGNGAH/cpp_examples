#include "palindrome.h"

bool IsPalindrome(const std::string& str) {
    int64_t i = 0;
    int64_t j = static_cast<int64_t>(std::size(str) - 1);
    while (i < j) {
        if (str[i] == ' ') {
            i++;
        } else if (str[j] == ' ') {
            j--;
        } else if (str[i] == str[j]) {
            i++;
            j--;
        } else {
            return false;
        }
    }
    return true;
}
