#include <iostream>

#include "flowcache/Cache.h"

int main() {
    flowcache::Cache cache;

    cache.put("user:101", "Jayant");
    cache.put("user:102", "Alice");
    cache.put("user:103", "Bob");

    std::cout << "FlowCache Engine v0.1.0\n";
    std::cout << "Cache size: " << cache.size() << '\n';

    auto user = cache.get("user:101");

    if (user.has_value()) {
        std::cout << "GET user:101 -> " << user.value() << '\n';
    } else {
        std::cout << "GET user:101 -> CACHE MISS\n";
    }

    auto missing = cache.get("user:999");

    if (missing.has_value()) {
        std::cout << "GET user:999 -> " << missing.value() << '\n';
    } else {
        std::cout << "GET user:999 -> CACHE MISS\n";
    }

    bool removed = cache.remove("user:102");

    std::cout << "REMOVE user:102 -> "
              << (removed ? "SUCCESS" : "NOT FOUND")
              << '\n';

    std::cout << "Final cache size: " << cache.size() << '\n';

    return 0;
}