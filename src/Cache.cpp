#include "flowcache/Cache.h"

namespace flowcache {

void Cache::put(const std::string& key, const std::string& value) {
    data_[key] = value;
}

std::optional<std::string> Cache::get(const std::string& key) const {
    auto it = data_.find(key);

    if (it == data_.end()) {
        return std::nullopt;
    }

    return it->second;
}

bool Cache::remove(const std::string& key) {
    return data_.erase(key) > 0;
}

std::size_t Cache::size() const {
    return data_.size();
}

} 