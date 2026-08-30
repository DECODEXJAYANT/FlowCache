@'
# FlowCache

## High-Performance Concurrent In-Memory Cache Engine

FlowCache is a thread-safe in-memory cache engine written in modern C++.

It uses a hash table for O(1) average key lookup and a doubly linked list to implement **Least Recently Used (LRU)** eviction. The engine supports concurrent cache operations, runtime statistics, cache inspection, automated tests, and workload-based performance benchmarking.

The project was designed as a systems-oriented C++ project to explore:

- In-memory data structures
- LRU cache design
- Hash-table based lookup
- Thread-safe concurrent access
- Cache statistics
- Multithreaded workloads
- Performance benchmarking
- Scalability analysis

---

## Features

### Core Cache

- `PUT` key/value storage
- `GET` key/value retrieval
- `REMOVE` key deletion
- Configurable cache capacity
- O(1) average hash-table lookup
- LRU eviction
- Updating an existing key without increasing cache size
- Empty keys and values supported
- Invalid zero-capacity cache rejected

### Thread Safety

FlowCache supports concurrent access using a mutex-protected cache implementation.

Concurrent operations are tested across multiple threads to verify:

- Cache capacity remains valid
- GET operations are tracked correctly
- Cache key inspection remains consistent
- LRU ordering remains correct

### Cache Statistics

FlowCache exposes runtime statistics including:

- Current cache size
- Cache capacity
- Cache hits
- Cache misses
- LRU evictions
- Hit ratio

The statistics API is available through:

```cpp
CacheStats stats() const;