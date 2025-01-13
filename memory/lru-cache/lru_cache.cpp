#include "lru_cache.h"

LruCache::LruCache(size_t max_size) {
    max_size_ = max_size;
    cur_size_ = 0;
}

void LruCache::FindDelete() {
    m_.erase(deq_.front().first);
    deq_.pop_front();
    --cur_size_;
}

void LruCache::Set(const std::string& key, const std::string& value) {
    deq_.emplace_back(key, value);
    if (m_.contains(key)) {
        deq_.erase(m_[key]);
    } else {
        ++cur_size_;
    }
    m_[key] = (--deq_.end());
    if (cur_size_ > max_size_) {
        FindDelete();
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    if (m_.contains(key)) {
        deq_.push_back(*m_[key]);
        deq_.erase(m_[key]);
        m_[key] = (--deq_.end());
        *value = m_[key]->second;
        return true;
    }
    return false;
}
