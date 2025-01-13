#include "associative_operation.h"

bool IsAssociative(const std::vector<std::vector<std::size_t>>& table) {
    for (std::size_t i = 0; i < std::size(table); ++i) {
        for (std::size_t j = 0; j < std::size(table); ++j) {
            for (std::size_t k = 0; k < std::size(table); ++k) {
                if (table[table[i][j]][k] != table[i][table[j][k]]) {
                    return false;
                }
            }
        }
    }
    return true;
}
