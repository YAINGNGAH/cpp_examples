#include "unixpath.h"
#include "vector"

std::vector<std::string> Split(std::string_view& s, const char& d) {
    std::string str = {s.begin(), s.end()};
    std::vector<std::string> a;
    size_t i = 0;
    size_t j = 0;
    while (str.find_first_not_of(d, i) != std::string_view::npos) {
        j = str.find_first_not_of(d, i);
        i = str.find(d, j);
        a.push_back(str.substr(j, i - j));
    }
    return a;
}

std::string NormalizePath(std::string_view current_working_dir, std::string_view path) {
    std::vector<std::string> cwd = Split(current_working_dir, '/');
    std::vector<std::string> pth = Split(path, '/');
    if (path[0] == '/') {
        cwd.clear();
    }
    for (const auto& i : pth) {
        if (i == "..") {
            if (!cwd.empty()) {
                cwd.pop_back();
            }
        } else if (i == ".") {
            continue;
        } else {
            cwd.push_back(i);
        }
    }
    std::string str;
    for (const auto& i : cwd) {
        str += "/";
        str += i;
    }
    if (str.empty()) {
        str = "/";
    }

    return str;
}
