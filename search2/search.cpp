#include "search.h"

std::set<std::string_view> SearchEngine::Split(std::string_view& str) const {
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
std::vector<std::string_view> SearchEngine::SplitText(std::string_view& text) const {
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

bool SearchEngine::Cmp(char a, char b) const {
    if (std::toupper(a) == b || std::tolower(a) == b) {
        return true;
    }
    return false;
}

bool SearchEngine::EqStr(const std::string_view& a, const std::string_view& b) const {
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
int64_t SearchEngine::CountAllWords(const std::string_view& str) const {
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

int64_t SearchEngine::CountWord(const std::string_view& str, const std::string_view& t) const {
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
bool SearchEngine::FindWord(const std::string_view& str, const std::string_view& t) const {
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

long double SearchEngine::TF(const std::string_view& d, const std::string_view& t) const {
    int64_t nt = 0;
    int64_t n = 0;
    nt = CountWord(d, t);
    n = CountAllWords(d);
    if (n == 0) {
        return 0;
    }
    return static_cast<long double>(nt) / static_cast<long double>(n);
}

long double SearchEngine::IDF(const std::string_view& nd, const std::string_view& t) const {
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

void SearchEngine::BuildIndex(std::string_view text) {
    idf_.clear();
    tf_.clear();
    lines_.clear();
    lines_ = SplitText(text);
    std::set<std::string_view> words;
    for (auto& i : SplitText(text)) {
        words.merge(Split(i));
    }
    for (const auto& i : words) {
        idf_[i] = IDF(text, i);
    }
    for (size_t l = 0; l < lines_.size(); l++) {
        for (const auto& k : Split(lines_[l])) {
            tf_[l][k] += TF(lines_[l], k);
        }
    }
}
std::vector<std::string_view> SearchEngine::Search(std::string_view query, size_t results_count) const {
    if (lines_.empty()) {
        return std::vector<std::string_view>();
    }
    std::set<std::string_view> select = Split(query);
    std::vector<long double> idf_tf;
    for (size_t l = 0; l < lines_.size(); l++) {
        idf_tf.push_back(0);
        for (const auto& k : select) {
            if (idf_.count(k) and tf_.at(l).count(k)) {
                idf_tf.back() += idf_.at(k) * tf_.at(l).at(k);
            }
        }
    }
    std::vector<size_t> line_num;
    for (size_t k = 0; k < lines_.size(); ++k) {
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
        answer.push_back(lines_[line_num[k]]);
    }
    return answer;
}
