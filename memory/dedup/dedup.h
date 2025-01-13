#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> d(items.size());

    for (size_t i = 0; i < items.size(); ++i) {
        d[i] = (std::make_unique<string>(*items[i]));
    }
    return d;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::vector<std::shared_ptr<string>> dd(items.size());
    ;
    std::unordered_map<string, std::shared_ptr<string>> m;
    for (size_t i = 0; i < items.size(); ++i) {
        if (m.contains(*items[i])) {
            dd[i] = m[*items[i]];
        } else {
            dd[i] = std::make_shared<string>(*items[i]);
            m[*items[i]] = dd[i];
        }
    }

    return dd;
}
