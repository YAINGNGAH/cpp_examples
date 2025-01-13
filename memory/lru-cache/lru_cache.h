#pragma once

#include <string>
#include <list>
#include <unordered_map>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

private:
    size_t max_size_;
    size_t cur_size_;
    std::list<std::pair<std::string, std::string>> deq_;
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> m_;
    void FindDelete();
};
