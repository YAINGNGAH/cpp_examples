#include "word2vec.h"

#include <vector>

int64_t operator*(const std::vector<int>& a, const std::vector<int>& b) {
    int64_t c = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        c += static_cast<int64_t>(a[i]) * static_cast<int64_t>(b[i]);
    }
    return c;
}

std::vector<std::string> FindClosestWords(const std::vector<std::string>& words,
                                          const std::vector<std::vector<int>>& vectors) {
    std::vector<std::string> closest_words;
    if (words.size() <= 1) {
        return closest_words;
    }
    int64_t m = vectors[0] * vectors[1];
    int64_t n = 0;
    for (size_t i = 1; i < words.size(); ++i) {
        n = vectors[0] * vectors[i];
        if (n > m) {
            closest_words.clear();
            closest_words.push_back(words[i]);
            m = n;
        } else if (n == m) {
            closest_words.push_back(words[i]);
        }
    }
    return closest_words;
}
