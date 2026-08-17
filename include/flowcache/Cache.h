#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>

namespace flowcache {

class Cache {
public:
    Cache() = default;

    void put(const std::string& key, const std::string& value);

    std::optional<std::string> get(const std::string& key) const;

    bool remove(const std::string& key);

    std::size_t size() const;

private:
    std::unordered_map<std::string, std::string> data_;
};

} 