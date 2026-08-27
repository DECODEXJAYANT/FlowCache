#include <iostream>
#include <string>
#include <cstdlib>

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
    
    testCapacityOne();
    testUpdateExistingKey();
    testRemoveMissingKey();
    testRepeatedGet();
    testEmptyKeyAndValue();

    cout << "\nAll tests passed!" << endl;

    return 0;
}