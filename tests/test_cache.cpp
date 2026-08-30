#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <vector>
#include <stdexcept>

#include "flowcache/Cache.h"

using namespace std;

void check(bool condition, const string &testName)
{
    if (condition)
    {
        cout << "[PASS] " << testName << endl;
    }
    else
    {
        cout << "[FAIL] " << testName << endl;
        exit(1);
    }
}

void testPutAndGet()
{
    flowcache::Cache cache(3);

    cache.put("A", "Apple");

    auto result = cache.get("A");

    check(
        result.has_value() && result.value() == "Apple",
        "PUT and GET");
}

void testCacheMiss()
{
    flowcache::Cache cache(3);

    auto result = cache.get("Missing");

    check(
        !result.has_value(),
        "GET missing key");
}

void testRemove()
{
    flowcache::Cache cache(3);

    cache.put("A", "Apple");
    cache.remove("A");

    auto result = cache.get("A");

    check(
        !result.has_value(),
        "REMOVE");
}

void testLRUEviction()
{
    flowcache::Cache cache(3);

    cache.put("A", "Apple");
    cache.put("B", "Banana");
    cache.put("C", "Cherry");

    // A becomes recently used.
    cache.get("A");

    // B should now be the least recently used.
    cache.put("D", "Dragonfruit");

    auto a = cache.get("A");
    auto b = cache.get("B");
    auto c = cache.get("C");
    auto d = cache.get("D");

    check(
        a.has_value(),
        "LRU keeps recently used entry");

    check(
        !b.has_value(),
        "LRU evicts least recently used entry");

    check(
        c.has_value(),
        "LRU keeps C");

    check(
        d.has_value(),
        "LRU stores new entry");
}

void testStatistics()
{
    flowcache::Cache cache(2);

    cache.put("A", "Apple");
    cache.put("B", "Banana");

    // Hit
    cache.get("A");

    // Miss
    cache.get("C");

    // Eviction
    cache.put("C", "Cherry");

    check(
        cache.hits() == 1,
        "Hit counter");

    check(
        cache.misses() == 1,
        "Miss counter");

    check(
        cache.evictions() == 1,
        "Eviction counter");

    check(
        cache.hitRatio() == 0.5,
        "Hit ratio");
}

// Phase 8.1:
// Test the structured CacheStats snapshot.
void testCacheStats()
{
    flowcache::Cache cache(3);

    cache.put("A", "Apple");
    cache.put("B", "Banana");
    cache.put("C", "Cherry");

    // Two successful GET operations.
    cache.get("A");
    cache.get("B");

    // One failed GET operation.
    cache.get("Missing");

    // Cache is full, so this causes one LRU eviction.
    cache.put("D", "Dragonfruit");

    flowcache::CacheStats stats = cache.stats();

    check(
        stats.size == 3,
        "CacheStats reports correct size");

    check(
        stats.capacity == 3,
        "CacheStats reports correct capacity");

    check(
        stats.hits == 2,
        "CacheStats reports correct hits");

    check(
        stats.misses == 1,
        "CacheStats reports correct misses");

    check(
        stats.evictions == 1,
        "CacheStats reports correct evictions");

    check(
        stats.hitRatio == (2.0 / 3.0),
        "CacheStats reports correct hit ratio");
}

void testCapacityOne()
{
    flowcache::Cache cache(1);

    cache.put("A", "Apple");
    cache.put("B", "Banana");

    auto a = cache.get("A");
    auto b = cache.get("B");

    check(
        !a.has_value(),
        "Capacity 1 evicts previous entry");

    check(
        b.has_value() && b.value() == "Banana",
        "Capacity 1 keeps newest entry");
}

void testUpdateExistingKey()
{
    flowcache::Cache cache(3);

    cache.put("A", "Apple");
    cache.put("A", "Apricot");

    auto result = cache.get("A");

    check(
        result.has_value() && result.value() == "Apricot",
        "Updating existing key");

    check(
        cache.size() == 1,
        "Updating key does not increase cache size");
}

void testRemoveMissingKey()
{
    flowcache::Cache cache(3);

    bool removed = cache.remove("Missing");

    check(
        !removed,
        "Removing missing key");

    check(
        cache.size() == 0,
        "Cache remains unchanged after removing missing key");
}

void testRepeatedGet()
{
    flowcache::Cache cache(3);

    cache.put("A", "Apple");

    cache.get("A");
    cache.get("A");
    cache.get("A");

    check(
        cache.hits() == 3,
        "Repeated GET hit tracking");

    check(
        cache.hitRatio() == 1.0,
        "100 percent hit ratio");
}

void testEmptyKeyAndValue()
{
    flowcache::Cache cache(3);

    cache.put("", "");

    auto result = cache.get("");

    check(
        result.has_value() && result.value() == "",
        "Empty key and value");
}

void testConcurrentAccess()
{
    flowcache::Cache cache(100);

    const int threadCount = 4;
    const int operationsPerThread = 1000;

    vector<thread> threads;

    for (int i = 0; i < threadCount; i++)
    {
        threads.emplace_back([&cache, i]()
        {
            for (int j = 0; j < operationsPerThread; j++)
            {
                string key =
                    "thread_" + to_string(i) +
                    "_key_" + to_string(j % 10);

                string value =
                    "value_" + to_string(j);

                cache.put(key, value);
                cache.get(key);
            }
        });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    check(
        cache.size() <= cache.capacity(),
        "Concurrent access respects cache capacity");

    check(
        cache.hits() == threadCount * operationsPerThread,
        "Concurrent GET operations tracked correctly");
}

int main()
{
    cout << "==================================" << endl;
    cout << "       FlowCache Test Suite       " << endl;
    cout << "==================================" << endl;

    testPutAndGet();
    testCacheMiss();
    testRemove();
    testLRUEviction();
    testStatistics();

    // Phase 8.1
    testCacheStats();

    testCapacityOne();
    testUpdateExistingKey();
    testRemoveMissingKey();
    testRepeatedGet();
    testEmptyKeyAndValue();

    testConcurrentAccess();

    bool caughtCapacityError = false;

    try
    {
        flowcache::Cache invalidCache(0);
    }
    catch (const invalid_argument &)
    {
        caughtCapacityError = true;
    }

    check(
        caughtCapacityError,
        "Zero capacity is rejected");

    flowcache::Cache keyCache(100);

    for (int i = 0; i < 50; i++)
    {
        keyCache.put(
            "key_" + to_string(i),
            "value");
    }

    vector<thread> keyThreads;

    for (int i = 0; i < 4; i++)
    {
        keyThreads.emplace_back([&keyCache]()
        {
            for (int j = 0; j < 1000; j++)
            {
                keyCache.keys();
            }
        });
    }

    for (auto &thread : keyThreads)
    {
        thread.join();
    }

    auto concurrentKeys = keyCache.keys();

    check(
        concurrentKeys.size() == 50,
        "Concurrent key inspection");

    flowcache::Cache cache(3);

    cache.put("A", "Apple");
    cache.put("B", "Banana");
    cache.put("C", "Cherry");

    auto keys = cache.keys();

    check(
        keys.size() == 3 &&
        keys[0] == "C" &&
        keys[1] == "B" &&
        keys[2] == "A",
        "Keys return MRU to LRU");

    cache.get("A");

    keys = cache.keys();

    check(
        keys.size() == 3 &&
        keys[0] == "A" &&
        keys[1] == "C" &&
        keys[2] == "B",
        "GET updates key order");

    cout << "\nAll tests passed!" << endl;

    return 0;
}