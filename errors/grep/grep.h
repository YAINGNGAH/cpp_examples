#pragma once

#include <optional>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <iterator>
#include "utf8.h"

using std::optional;

struct GrepOptions {
    optional<size_t> look_ahead_length;
    size_t max_matches_per_line;

    GrepOptions() {
        max_matches_per_line = 10;
    }

    GrepOptions(size_t look_ahead_length) : GrepOptions() {
        this->look_ahead_length = look_ahead_length;
    }

    GrepOptions(optional<size_t> look_ahead_length, size_t max_matches_per_line) {
        this->look_ahead_length = look_ahead_length;
        this->max_matches_per_line = max_matches_per_line;
    }
};

std::string Utf8Substr(const std::string& str, size_t start, size_t leng) {
    std::string substr;
    auto it = str.begin();
    utf8::advance(it, start, str.end());
    if (utf8::distance(it, str.end()) < leng) {
        leng = utf8::distance(it, str.end());
    }
    for (int i = 0; i < leng; ++i) {
        utf8::append(utf8::next(it, str.end()), std::back_inserter(substr));
    }
    return substr;
}
bool ValidUtf8File(const char* file_name) {
    std::ifstream ifs(file_name);
    std::istreambuf_iterator<char> it(ifs.rdbuf());
    std::istreambuf_iterator<char> eos;
    return utf8::is_valid(it, eos);
}
template <class Visitor>
void Grep(const std::string& path, const std::string& pattern, Visitor visitor,
          const GrepOptions& options) {

    if (std::filesystem::is_symlink(path) || std::filesystem::is_fifo(path) ||
        std::filesystem::is_character_file(path) || std::filesystem::is_block_file(path)) {
        return;
    } else if (std::filesystem::is_directory(path)) {
        try {
            auto diriter = std::filesystem::directory_iterator(std::filesystem::path(path));
            for (auto dir_entry : diriter) {
                Grep(dir_entry.path(), pattern, visitor, options);
            }
        } catch (...) {
        }
    } else {
        try {
            std::ifstream file(path, std::ios::in);
            if (!file) {
                throw std::logic_error("No permission");
            }
            if (!ValidUtf8File(&path[0])) {
                throw std::logic_error("Invalid UTF-8");
            }
            std::string line;
            size_t lineind = 0;
            while (std::getline(file, line)) {
                lineind++;
                auto it = std::search(line.begin(), line.end(),
                                      std::boyer_moore_searcher(pattern.begin(), pattern.end()));
                for (size_t i = 0; i < options.max_matches_per_line; ++i) {
                    if (it == line.end()) {
                        break;
                    }
                    if (options.look_ahead_length == std::nullopt) {
                        visitor.OnMatch(path, lineind, utf8::distance(line.begin(), it) + 1,
                                        std::nullopt);
                    } else {
                        visitor.OnMatch(
                            path, lineind, utf8::distance(line.begin(), it) + 1,
                            Utf8Substr(line,
                                       utf8::distance(line.begin(), it) +
                                           utf8::distance(pattern.begin(), pattern.end()),
                                       *options.look_ahead_length));
                    }

                    it = std::search(it + pattern.size(), line.end(),
                                     std::boyer_moore_searcher(pattern.begin(), pattern.end()));
                }
            }
            file.close();
        } catch (const std::exception& e) {
            visitor.OnError(e.what());
        }
    }
}
