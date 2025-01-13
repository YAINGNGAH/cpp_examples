#include "search.h"
#include <algorithm>
#include <cctype>
#include "math.h"
#include "set"
#include "map"

std::set<std::string_view> Split(std::string_view& str) {
    std::set<std::string_view> a;
    size_t j = 0;
    bool ls = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isalpha(str[i]) && ls) {
            a.insert(str.substr(j, i - j));
        }
        if (isalpha(str[i]) && !ls) {
            j = i;
        }
        ls = isalpha(str[i]);
    }
    if (ls and !str.empty()) {
        a.insert(str.substr(j, str.size() - j));
    }
    return a;
}
std::vector<std::string_view> SplitText(std::string_view& text) {
    size_t i = 0;
    size_t j = 0;
    std::vector<std::string_view> a;
    while (text.find_first_not_of("\n", i) != std::string_view::npos) {
        j = text.find_first_not_of("\n", i);
        i = text.find("\n", j);
        a.push_back(text.substr(j, i - j));
    }
    return a;
}

bool Cmp(char a, char b) {
    if (std::toupper(a) == b || std::tolower(a) == b) {
        return true;
    }
    return false;
}

bool EqStr(const std::string_view& a, const std::string_view& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (!Cmp(a[i], b[i])) {
            return false;
        }
    }
    return true;
}
int64_t CountAllWords(const std::string_view& str) {
    int64_t c = 0;
    bool ls = false;
    for (const auto& i : str) {
        if (!isalpha(i) && ls) {
            c++;
        }
        ls = isalpha(i);
    }
    if (ls) {
        c++;
    }
    return c;
}

int64_t CountWord(const std::string_view& str, const std::string_view& t) {
    int64_t c = 0;
    size_t j = 0;
    bool ls = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isalpha(str[i]) && ls) {
            if (EqStr(str.substr(j, i - j), t)) {
                c++;
            }
        }
        if (isalpha(str[i]) && !ls) {
            j = i;
        }
        ls = isalpha(str[i]);
    }
    if (ls and !str.empty()) {
        if (EqStr(str.substr(j, str.size() - j), t)) {
            c++;
        }
    }
    return c;
}
bool FindWord(const std::string_view& str, const std::string_view& t) {
    size_t j = 0;
    bool ls = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isalpha(str[i]) && ls) {
            if (EqStr(str.substr(j, i - j), t)) {
                return true;
            }
        }
        if (isalpha(str[i]) && !ls) {
            j = i;
        }
        ls = isalpha(str[i]);
    }
    if (ls and !str.empty()) {
        if (EqStr(str.substr(j, str.size() - j), t)) {
            return true;
        }
    }
    return false;
}

long double TF(const std::string_view& d, const std::string_view& t) {
    int64_t nt = 0;
    int64_t n = 0;
    nt = CountWord(d, t);
    n = CountAllWords(d);
    if (n == 0) {
        return 0;
    }
    return static_cast<long double>(nt) / static_cast<long double>(n);
}

long double IDF(const std::string_view& nd, const std::string_view& t) {
    size_t i = 0;
    size_t j = 0;
    int64_t c = 0;
    int64_t d = 0;
    while (nd.find_first_not_of("\n", i) != std::string_view::npos) {
        j = nd.find_first_not_of("\n", i);
        i = nd.find("\n", j);
        c++;
        if (FindWord(nd.substr(j, i - j), t)) {
            d++;
        }
    }
    if (d == 0) {
        return 0;
    }
    return log(static_cast<long double>(c) / static_cast<long double>(d));
}

std::vector<std::string_view> Search(std::string_view text, std::string_view query, size_t results_count) {
    std::set<std::string_view> select = Split(query);
    std::map<std::string_view, long double> idf;
    std::vector<long double> idf_tf;
    std::vector<std::string_view> lines;
    lines = SplitText(text);
    for (const auto& t : select) {
        idf[t] = IDF(text, t);
    }
    for (const auto& l : lines) {
        idf_tf.push_back(0);
        for (const auto& k : select) {
            idf_tf.back() += idf[k] * TF(l, k);
        }
    }
    std::vector<size_t> line_num;
    for (size_t k = 0; k < lines.size(); ++k) {
        if (idf_tf[k] > 0) {
            line_num.push_back(k);
        }
    }
    auto l = line_num.begin();  // Я хотел сделать нормально, через арифметику указателей, но требования этого не
                                // позволяют, так что терпим
    size_t rs = std::min(results_count, line_num.size());
    for (size_t i = 0; i < rs; ++i) {
        ++l;
    }
    std::partial_sort(line_num.begin(), l, line_num.end(), [idf_tf](const auto& a, const auto& b) -> bool {
        if (idf_tf[a] > idf_tf[b]) {
            return true;
        } else if (idf_tf[a] == idf_tf[b] and a < b) {
            return true;
        }
        return false;
    });
    std::vector<std::string_view> answer;
    for (size_t k = 0; k < rs; ++k) {
        answer.push_back(lines[line_num[k]]);
    }
    return answer;
}
