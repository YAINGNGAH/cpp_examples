#include "unixpath.h"

std::vector<std::string> UnixPath::Split(std::string_view& s, const char& d) {
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

std::vector<std::string> UnixPath::NormalizePath(std::string_view current_working_dir) {
    std::vector<std::string> pth = Split(current_working_dir, '/');
    for (const auto& i : pth) {
        if (i == "..") {
            if (!cwd_.empty()) {
                cwd_.pop_back();
            }
        } else if (i == ".") {
            continue;
        } else {
            cwd_.push_back(i);
        }
    }
    return cwd_;
}

std::vector<std::string> UnixPath::GetDwd() const {
    std::vector<std::string> dwd;
    int64_t bpoint = 0;
    for (int64_t i = static_cast<int64_t>(fd_.size()) - 1; i >= 0; --i) {
        if (static_cast<size_t>(i) >= cwd_.size()) {
            dwd.push_back("..");
        } else if (cwd_[i] != fd_[i]) {
            dwd.push_back("..");
        } else {
            bpoint = i + 1;
            break;
        }
    }
    if (dwd.empty()) {
        dwd.push_back(".");
    }
    for (size_t i = bpoint; i < cwd_.size(); ++i) {
        dwd.push_back(cwd_[i]);
    }
    return dwd;
}

void UnixPath::ChangeDirectory(std::string_view path) {
    std::vector<std::string> pth = Split(path, '/');
    if (path[0] == '/') {
        cwd_.clear();
    }
    for (const auto& i : pth) {
        if (i == "..") {
            if (!cwd_.empty()) {
                cwd_.pop_back();
            }
        } else if (i == ".") {
            continue;
        } else {
            cwd_.push_back(i);
        }
    }
};

UnixPath::UnixPath(std::string_view initial_dir) {
    cwd_ = NormalizePath(initial_dir);
    fd_ = cwd_;
}

std::string UnixPath::GetAbsolutePath() const {
    std::string str;
    for (const auto& i : cwd_) {
        str += "/";
        str += i;
    }
    if (str.empty()) {
        str = "/";
    }
    return str;
}

std::string UnixPath::GetRelativePath() const {
    std::string str;
    std::vector<std::string> dwd = GetDwd();
    for (const auto& i : dwd) {
        str += i;
        str += "/";
    }
    str.pop_back();
    if (str.empty()) {
        str = "/";
    }
    return str;
}
