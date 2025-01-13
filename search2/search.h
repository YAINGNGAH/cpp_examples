#pragma once

#include <string_view>
#include <vector>
#include <set>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <cctype>
#include <math.h>

class SearchEngine {
private:
    std::unordered_map<std::string_view, long double> idf_;
    std::unordered_map<size_t, std::unordered_map<std::string_view, long double>> tf_;
    std::vector<std::string_view> lines_;
    std::set<std::string_view> Split(std::string_view& str) const;
    std::vector<std::string_view> SplitText(std::string_view& text) const;
    bool Cmp(char a, char b) const;
    bool EqStr(const std::string_view& a, const std::string_view& b) const;
    int64_t CountAllWords(const std::string_view& str) const;
    int64_t CountWord(const std::string_view& str, const std::string_view& t) const;
    bool FindWord(const std::string_view& str, const std::string_view& t) const;
    long double TF(const std::string_view& d, const std::string_view& t) const;
    long double IDF(const std::string_view& nd, const std::string_view& t) const;

public:
    void BuildIndex(std::string_view text);
    std::vector<std::string_view> Search(std::string_view query, size_t results_count) const;
};
