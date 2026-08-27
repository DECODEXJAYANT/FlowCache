#include <iostream>

#include "flowcache/Cache.h"

using namespace std;

int main() {

    // Create cache with capacity 3
    flowcache::Cache cache(3);

    cout << "FlowCache Engine v0.3.0" << endl;
    cout << "Cache capacity: " << cache.capacity() << endl;

    // Add three entries
    cache.put("A", "Apple");
    cache.put("B", "Banana");
    cache.put("C", "Cherry");

    cout << "\nAfter adding A, B, C:" << endl;
    cout << "Cache size: " << cache.size() << endl;

    // Access A
    // A becomes the most recently used entry
    cache.get("A");

    cout << "\nAccessed A" << endl;

    // Add D
    // Cache is full, so the least recently used entry is removed
    cache.put("D", "Dragonfruit");

    cout << "\nAdded D" << endl;
    cout << "Cache size: " << cache.size() << endl;

    // Check which entries survived
    auto a = cache.get("A");
    auto b = cache.get("B");
    auto c = cache.get("C");
    auto d = cache.get("D");

    cout << "\nCache contents:" << endl;

    cout << "A -> "
         << (a.has_value() ? a.value() : "CACHE MISS")
         << endl;

    cout << "B -> "
         << (b.has_value() ? b.value() : "CACHE MISS")
         << endl;

    cout << "C -> "
         << (c.has_value() ? c.value() : "CACHE MISS")
         << endl;

    cout << "D -> "
         << (d.has_value() ? d.value() : "CACHE MISS")
         << endl;

    // Display cache statistics
    cout << "\nCache statistics:" << endl;

    cout << "Hits: "
         << cache.hits()
         << endl;

    cout << "Misses: "
         << cache.misses()
         << endl;

    cout << "Evictions: "
         << cache.evictions()
         << endl;

    cout << "Hit ratio: "
         << cache.hitRatio() * 100
         << "%"
         << endl;

    return 0;
}