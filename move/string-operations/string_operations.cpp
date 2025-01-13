#include "string_operations.h"

bool StartsWith(const std::string_view string, const std::string_view text) {
    if (text.size() > string.size()) {
        return false;
    }
    if (text.data() == string.data()) {
        return true;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] != string[i]) {
            return false;
        }
    }
    return true;
}

bool EndsWith(const std::string_view string, const std::string_view text) {
    if (text.size() > string.size()) {
        return false;
    }
    if (text.data() == string.data() + (string.size() - text.size())) {
        return true;
    }
    for (size_t i = string.size() - text.size(); i < string.size(); ++i) {
        if (text[i - string.size() + text.size()] != string[i]) {
            return false;
        }
    }
    return true;
}

std::string_view StripPrefix(std::string_view string, const std::string_view prefix) {
    if (StartsWith(string, prefix)) {
        string.remove_prefix(prefix.size());
        return string;
    }
    return string;
}

std::string_view StripSuffix(std::string_view string, const std::string_view suffix) {
    if (EndsWith(string, suffix)) {
        string.remove_suffix(suffix.size());
        return string;
    }
    return string;
}
std::string_view ClippedSubstr(const std::string_view s, size_t pos, size_t n) {
    if (n > s.size() - pos) {
        return s.substr(pos, std::string::npos);
    }
    return s.substr(pos, n);
}

std::string_view StripAsciiWhitespace(const std::string_view s) {
    if (s.empty()) {
        return s;
    }
    size_t l;
    size_t r;
    for (l = 0; l < s.size() && isspace(s[l]); ++l) {
    }
    if (l == s.size()) {
        return "";
    }
    for (r = s.size() - 1; r > l && isspace(s[r]); --r) {
    }
    return ClippedSubstr(s, l, r - l + 1);
}

std::vector<std::string_view> StrSplit(std::string_view text, const std::string_view delim) {
    size_t c = 0;
    size_t i = text.find(delim, 0);
    size_t j = 0;
    while (j != std::string_view::npos) {
        c++;
        j = i;
        i = text.find(delim, i + delim.size());
    }
    std::vector<std::string_view> v(c);
    c = 0;
    i = text.find(delim, 0);
    ;
    j = 0;
    while (j != std::string_view::npos) {
        if (c == 0) {
            v[c] = ClippedSubstr(text, j, i - j);
        } else {
            v[c] = ClippedSubstr(text, j + delim.size(), i - j - delim.size());
        }
        c++;
        j = i;
        i = text.find(delim, i + delim.size());
    }
    return v;
}

std::string ReadN(const std::string& filename, size_t n) {
    int file = open(filename.c_str(), O_RDONLY);
    if (file == -1) {
        return "";
    }
    std::string s(n, ' ');
    int ret;
    size_t c = 0;
    char ch;
    while ((ret = read(file, &ch, 1)) > 0 && c < n) {
        s[c] = ch;
        c++;
    }
    if (ret < 0) {
        return "";
    }
    close(file);
    return s;
}

std::string AddSlash(std::string_view path) {
    if (path.back() != '/') {
        std::string s(path.size() + 1, ' ');
        std::copy(path.begin(), path.end(), s.begin());
        s[path.size()] = '/';
        return s;
    }
    std::string s(path.size(), ' ');
    std::copy(path.begin(), path.end(), s.begin());
    return s;
}

std::string_view RemoveSlash(std::string_view path) {
    if (path.back() == '/') {
        if (path.size() == 1) {
            return path;
        }
        path.remove_suffix(1);
        return path;
    }
    return path;
}

std::string_view Dirname(std::string_view path) {
    size_t i = path.size() - 1;
    while (path[i] != '/') {
        i--;
    }
    while (path[i] == '/' && i > 0) {
        i--;
    }
    if (i == 0) {
        return "/";
    }
    path.remove_suffix(path.size() - i - 1);
    return path;
}

std::string_view Basename(std::string_view path) {
    size_t i = path.size() - 1;
    while (path[i] == '/') {
        i--;
    }
    while (path[i] != '/') {
        i--;
    }
    path.remove_prefix(i + 1);
    return path;
}

std::string CollapseSlashes(std::string_view path) {
    size_t i = 0;
    size_t c = 0;
    while (i < path.size()) {
        if (path[i] == '/') {
            c++;
        }
        while (path[i] == '/' && i < path.size()) {
            i++;
        }
        while (path[i] != '/' && i < path.size()) {
            c++;
            i++;
        }
    }
    std::string s(c, ' ');
    i = 0;
    c = 0;
    while (i < path.size()) {
        if (path[i] == '/') {
            s[c] = '/';
            c++;
        }
        while (path[i] == '/' && i < path.size()) {
            i++;
        }
        while (path[i] != '/' && i < path.size()) {
            s[c] = path[i];
            c++;
            i++;
        }
    }
    return s;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    size_t r = 0;
    for (size_t i = 0; i < strings.size(); ++i) {
        r += strings[i].size();
        if (i != strings.size() - 1) {
            r += delimiter.size();
        }
    }
    std::string s(r, ' ');
    size_t c = 0;
    for (size_t i = 0; i < strings.size(); ++i) {
        for (auto j : strings[i]) {
            s[c] = j;
            c++;
        }
        if (i != strings.size() - 1) {
            for (auto j : delimiter) {
                s[c] = j;
                c++;
            }
        }
    }
    return s;
}
