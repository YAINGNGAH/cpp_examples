#pragma once

#include <string>
#include <string_view>
#include <vector>

class UnixPath {
private:
    std::vector<std::string> cwd_;
    std::vector<std::string> fd_;
    std::vector<std::string> Split(std::string_view& s, const char& d);
    std::vector<std::string> NormalizePath(std::string_view current_working_dir);
    std::vector<std::string> GetDwd() const;

public:
    explicit UnixPath(std::string_view initial_dir);
    void ChangeDirectory(std::string_view path);
    std::string GetAbsolutePath() const;
    std::string GetRelativePath() const;
};
