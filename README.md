# 🚀 FlowCache

### High-Performance Concurrent In-Memory Cache Engine

FlowCache is a thread-safe, high-performance in-memory cache engine written in modern C++.

It combines hash-table based storage with a doubly linked list to implement **O(1) average-time cache operations** and **Least Recently Used (LRU) eviction**.

FlowCache is designed as a systems-oriented project to explore efficient data structures, concurrent programming, cache behavior, synchronization, performance benchmarking, and workload analysis.

---

## 📌 Overview

Caching is commonly used to reduce application latency and avoid repeatedly accessing slower storage or backend services.

A cache engine must efficiently handle:

- Fast key lookup
- Fast insertion and removal
- Limited memory capacity
- Eviction of unused entries
- Concurrent access from multiple threads
- Runtime performance monitoring

FlowCache addresses these requirements using a combination of:

```text
                FlowCache
                    │
        ┌───────────┴───────────┐
        │                       │
   Hash Table              LRU List
        │                       │
        ▼                       ▼
   Fast Lookup          Eviction Ordering
        │                       │
        └───────────┬───────────┘
                    ▼
             Thread Safety
                    │
                    ▼
             Cache Statistics
                    │
                    ▼
          Workload Benchmarking
```

---

# ✨ Features

## ⚡ Core Cache Operations

FlowCache provides the fundamental operations expected from an in-memory cache:

- `PUT` — insert or update a key/value pair
- `GET` — retrieve a cached value
- `REMOVE` — remove a key
- Configurable cache capacity
- O(1) average hash-table lookup
- LRU-based eviction
- Cache size tracking
- Empty keys and values supported
- Invalid zero-capacity caches rejected

Example:

```cpp
flowcache::Cache cache(1000);

cache.put("user:101", "Jayant");

auto value = cache.get("user:101");

cache.remove("user:101");
```

---

# 🧠 Cache Architecture

FlowCache uses two primary data structures:

```text
                ┌─────────────────┐
                │   Hash Table    │
                │                 │
                │ Key → List Node │
                └────────┬────────┘
                         │
                         ▼
              ┌─────────────────────┐
              │    Doubly Linked    │
              │        List         │
              │                     │
              │ MRU ← ... → LRU     │
              └─────────────────────┘
```

The hash table provides fast access to cache entries.

The doubly linked list maintains the access order required for LRU eviction.

### Why this design?

A hash table alone provides fast lookup but does not efficiently track which entry should be evicted.

A linked list alone can maintain ordering but requires O(n) lookup.

Combining both provides:

| Operation | Average Complexity |
|-----------|--------------------|
| GET | O(1) |
| PUT | O(1) |
| REMOVE | O(1) |
| LRU eviction | O(1) |

---

# 🔄 LRU Eviction

FlowCache uses **Least Recently Used (LRU)** eviction.

The cache maintains entries in the following order:

```text
MRU
 │
 ▼
[A] → [C] → [B]
             │
             ▼
            LRU
```

When an entry is accessed:

```text
Before:

[A] → [C] → [B]
MRU             LRU

GET(B)

After:

[B] → [A] → [C]
MRU             LRU
```

When the cache reaches its capacity and a new entry is inserted, the least recently used entry is removed.

Example:

```text
Capacity = 3

PUT A
PUT B
PUT C

Cache:

C → B → A

GET A

Cache:

A → C → B

PUT D

B is the LRU entry.

Result:

D → A → C
```

FlowCache tracks these evictions through runtime statistics.

---

# 🔒 Thread Safety

FlowCache supports concurrent cache access using mutex-protected shared state.

Multiple threads can safely perform:

- `GET`
- `PUT`
- `REMOVE`
- Cache statistics queries
- Key inspection

Concurrency tests verify that:

- Cache capacity remains valid
- Concurrent GET operations are tracked correctly
- Shared cache state remains consistent
- Key inspection remains safe
- LRU ordering remains correct
- No invalid cache state is produced during concurrent operations

Example workload:

```text
Thread 1 ──┐
Thread 2 ──┤
Thread 3 ──┼──► FlowCache
Thread 4 ──┤
Thread 5 ──┘
```

---

# 📊 Cache Statistics

FlowCache provides runtime cache statistics through the `CacheStats` API.

Available metrics include:

- Current cache size
- Cache capacity
- Cache hits
- Cache misses
- LRU evictions
- Hit ratio

Example:

```cpp
flowcache::CacheStats stats = cache.stats();

cout << stats.size << endl;
cout << stats.capacity << endl;
cout << stats.hits << endl;
cout << stats.misses << endl;
cout << stats.evictions << endl;
cout << stats.hitRatio << endl;
```

The statistics API is available through:

```cpp
CacheStats stats() const;
```

---

# 📈 Hit Ratio

The cache hit ratio represents the percentage of GET operations successfully served by the cache.

```text
Hit Ratio = Hits / (Hits + Misses)
```

For example:

```text
Hits   = 900
Misses = 100

Hit Ratio = 90%
```

This provides a useful measurement of cache effectiveness under different workloads.

---

# 🔍 Cache Key Inspection

FlowCache provides an API for inspecting the current cache key ordering.

Keys are returned from:

```text
Most Recently Used
        ↓
       LRU
```

Example:

```text
PUT A
PUT B
PUT C

keys():

[C, B, A]
```

After:

```text
GET A
```

the ordering becomes:

```text
[A, C, B]
```

This makes the internal LRU behavior observable and testable.

---

# 🧪 Testing

FlowCache includes a dedicated test suite covering functional correctness, eviction behavior, statistics, and concurrency.

The test suite verifies:

### Basic Operations

- PUT and GET
- GET missing key
- REMOVE
- Removing missing keys

### LRU Behavior

- Recently used entries remain in cache
- Least recently used entries are evicted
- GET updates LRU ordering
- Keys are returned in MRU → LRU order

### Statistics

- Hit counter
- Miss counter
- Eviction counter
- Hit ratio
- Cache size
- Cache capacity

### Edge Cases

- Capacity of one
- Zero-capacity rejection
- Updating existing keys
- Empty keys and values

### Concurrency

- Concurrent GET/PUT operations
- Concurrent key inspection
- Cache capacity under concurrent workloads
- Correct hit tracking under multiple threads

---

# 🏃 Running Tests

Build the project:

```powershell
cmake -S . -B build
cmake --build build
```

Run the test suite:

```powershell
.\build\flowcache_tests.exe
```

Expected result:

```text
==================================
       FlowCache Test Suite
==================================

[PASS] PUT and GET
[PASS] GET missing key
[PASS] REMOVE
[PASS] LRU keeps recently used entry
[PASS] LRU evicts least recently used entry
...
[PASS] Concurrent access respects cache capacity
[PASS] Concurrent GET operations tracked correctly
[PASS] Zero capacity is rejected
[PASS] Concurrent key inspection
[PASS] Keys return MRU to LRU
[PASS] GET updates key order

All tests passed!
```

---

# 🏎️ Benchmarking

FlowCache includes a workload-based benchmarking system for measuring cache performance under different access patterns.

The benchmark measures:

- Operations per second
- Minimum throughput
- Maximum throughput
- Average throughput
- Cache hits
- Cache misses
- Evictions
- Hit ratio
- Multithreaded scalability

---

# 📊 Workload Types

The benchmark currently evaluates multiple workloads.

## Read Heavy

```text
90% GET
5% PUT
5% REMOVE
```

Represents applications where cached data is predominantly read.

---

## Balanced

```text
70% GET
20% PUT
10% REMOVE
```

Represents a general-purpose caching workload.

---

## Write Heavy

```text
30% GET
50% PUT
20% REMOVE
```

Represents applications with frequent updates.

---

## High Churn

```text
20% GET
60% PUT
20% REMOVE
```

Represents workloads with frequent cache replacement and deletion.

---

# 🔥 Stress Benchmark

FlowCache also supports workloads where the working set is significantly larger than the cache capacity.

Example:

```text
Cache capacity: 10,000
Working set:    100,000
```

This creates memory pressure and causes frequent cache misses and LRU evictions.

Example behavior:

```text
Working Set
100,000 entries
       │
       ▼
┌─────────────────┐
│    FlowCache    │
│                 │
│ Capacity 10,000 │
└────────┬────────┘
         │
         ▼
  LRU Evictions
```

This workload is useful for evaluating cache behavior when the cache cannot hold the entire working set.

---

# 📈 Benchmark Results

Example benchmark results obtained during development:

| Workload | Threads | Avg Throughput |
|----------|---------|----------------|
| Read Heavy | 1 | ~2.98M ops/sec |
| Read Heavy | 4 | ~1.52M ops/sec |
| Read Heavy | 8 | ~1.28M ops/sec |
| Balanced | 1 | ~1.06M ops/sec |
| Balanced | 4 | ~0.86M ops/sec |
| Balanced | 8 | ~0.91M ops/sec |
| Write Heavy | 1 | ~0.91M ops/sec |
| Write Heavy | 4 | ~0.60M ops/sec |
| Write Heavy | 8 | ~0.71M ops/sec |
| High Churn | 1 | ~0.82M ops/sec |
| High Churn | 4 | ~0.48M ops/sec |
| High Churn | 8 | ~0.62M ops/sec |

> Benchmark results depend on CPU, compiler, operating system, build configuration, and system load. These numbers should be treated as development measurements rather than universal performance guarantees.

---

# 📉 Cache Pressure Results

With a working set of:

```text
100,000 keys
```

and cache capacity:

```text
10,000 entries
```

the benchmark produces significant cache pressure.

Example:

```text
Hit ratio: approximately 10%

Misses:     hundreds of thousands
Evictions:  thousands to hundreds of thousands
```

This demonstrates the effect of a working set exceeding available cache capacity.

---

# 📁 Project Structure

```text
FlowCache/
│
├── include/
│   └── flowcache/
│       └── Cache.h
│
├── src/
│   └── Cache.cpp
│
├── tests/
│   └── test_cache.cpp
│
├── benchmarks/
│   └── benchmark_cache.cpp
│
├── docs/
│
├── scripts/
│
├── CMakeLists.txt
│
├── README.md
│
└── build/
```

### Main Components

#### `include/flowcache/Cache.h`

Public cache API and data structures.

#### `src/Cache.cpp`

Core cache implementation including:

- Hash-table storage
- LRU management
- Synchronization
- Cache statistics
- Key inspection

#### `tests/test_cache.cpp`

Functional and concurrency test suite.

#### `benchmarks/benchmark_cache.cpp`

Workload-based and multithreaded performance benchmarking.

#### `CMakeLists.txt`

CMake build configuration.

---

# 🛠️ Tech Stack

### Language

- C++17 / C++20

### Standard Library

- STL
- `std::unordered_map`
- `std::list`
- `std::mutex`
- `std::thread`
- `std::optional`
- `std::chrono`

### Build System

- CMake
- Ninja

### Development Tools

- GCC / MinGW
- MSYS2
- Visual Studio Code
- Git
- GitHub

---

# 🚀 Getting Started

## 1. Clone the Repository

```bash
git clone https://github.com/DECODEXJAYANT/FlowCache.git

cd FlowCache
```

---

## 2. Configure the Build

```powershell
cmake -S . -B build
```

---

## 3. Build

```powershell
cmake --build build
```

---

## 4. Run FlowCache

```powershell
.\build\flowcache.exe
```

Example output:

```text
FlowCache Engine v0.3.0
Cache capacity: 3

After adding A, B, C:
Cache size: 3

Accessed A

Added D
Cache size: 3

Cache contents:
A -> Apple
B -> CACHE MISS
C -> Cherry
D -> Dragonfruit

Cache statistics:
Hits: 4
Misses: 1
Evictions: 1
Hit ratio: 80%
```

---

## 5. Run Tests

```powershell
.\build\flowcache_tests.exe
```

---

## 6. Run Benchmarks

```powershell
.\build\flowcache_benchmark.exe
```

---

# ⚙️ Design Goals

FlowCache is being developed around several systems-engineering principles:

### 1. Efficient Data Structures

Use appropriate data structures to minimize operation complexity.

### 2. Correctness

Cache behavior should remain deterministic and correct under normal and concurrent workloads.

### 3. Thread Safety

Shared cache state must remain consistent when accessed by multiple threads.

### 4. Measurable Performance

Performance improvements should be validated through benchmarks rather than assumptions.

### 5. Observability

Runtime statistics should make cache behavior measurable and understandable.

### 6. Extensibility

The architecture should allow additional cache policies and features to be introduced without unnecessarily rewriting the core engine.

---

# 🔬 Engineering Focus

FlowCache is primarily a systems-programming project.

The project explores concepts including:

```text
Data Structures
      │
      ▼
Hash Tables + Linked Lists
      │
      ▼
Cache Algorithms
      │
      ▼
LRU Eviction
      │
      ▼
Concurrency
      │
      ▼
Synchronization
      │
      ▼
Performance Engineering
      │
      ▼
Benchmarking
```

The goal is not simply to implement a cache, but to understand the engineering trade-offs involved in building one.

---

# ⚠️ Current Limitations

The current version intentionally focuses on the core cache engine.

Current limitations include:

- Single cache instance synchronization uses mutex-based locking.
- LRU operations require synchronization around shared cache state.
- No TTL-based expiration yet.
- No LFU eviction policy yet.
- No persistence layer.
- No distributed caching support.
- No network protocol/API layer.
- Benchmark results are hardware-dependent.
- Current benchmarking focuses primarily on throughput rather than detailed latency distributions.

---

# 🔮 Future Work

FlowCache will be improved incrementally as a systems-engineering project.

## Phase 1 — Cache Features

- [ ] TTL-based expiration
- [ ] Configurable expiration policies
- [ ] LFU eviction
- [ ] FIFO eviction
- [ ] Pluggable eviction policies

---

## Phase 2 — Concurrency Improvements

- [ ] Reduce global mutex contention
- [ ] Sharded cache architecture
- [ ] Per-shard locking
- [ ] Improved concurrent scalability
- [ ] Lock contention benchmarking

Potential architecture:

```text
                FlowCache
                    │
        ┌───────────┼───────────┐
        │           │           │
      Shard 0     Shard 1     Shard N
        │           │           │
     Mutex 0     Mutex 1     Mutex N
```

This could allow independent portions of the cache to operate concurrently.

---

## Phase 3 — Advanced Benchmarking

- [ ] Latency measurement
- [ ] p50 latency
- [ ] p95 latency
- [ ] p99 latency
- [ ] Memory usage measurement
- [ ] CPU utilization analysis
- [ ] Lock contention analysis
- [ ] Automated benchmark reports
- [ ] Performance regression detection

---

## Phase 4 — Real-World Comparison

Compare FlowCache against established caching systems such as:

```text
FlowCache
    │
    ├── Throughput
    ├── Latency
    ├── Memory usage
    └── Scalability
          │
          ▼
      Redis
```

The goal would be to understand where a custom in-process cache performs well and where a mature distributed cache is preferable.

---

## Phase 5 — Service Integration

Potential future integrations include:

- [ ] REST API
- [ ] FastAPI integration
- [ ] C++ client library
- [ ] Monitoring endpoint
- [ ] Prometheus metrics
- [ ] Docker deployment

Possible architecture:

```text
Application
     │
     ▼
FlowCache API
     │
     ▼
FlowCache Engine
     │
     ├── Hash Table
     ├── LRU
     ├── Statistics
     └── Concurrency
```

---

## Phase 6 — Production-Oriented Features

Long-term improvements may include:

- [ ] Persistence
- [ ] Snapshotting
- [ ] Configurable memory limits
- [ ] Serialization
- [ ] Metrics and observability
- [ ] Distributed cache support
- [ ] Replication
- [ ] Failure recovery
- [ ] Production deployment experiments

---

# 📚 Learning Objectives

This project provides practical experience with:

- Advanced C++ programming
- STL data structures
- Hash tables
- Linked lists
- Cache algorithms
- LRU eviction
- Big-O analysis
- Mutexes
- Multithreading
- Race-condition prevention
- Performance engineering
- Benchmark design
- Systems architecture
- CMake
- Git/GitHub

---

# 📊 Project Status

**Status: Active Development**

Current implementation includes:

- ✅ In-memory cache
- ✅ Hash-table based lookup
- ✅ LRU eviction
- ✅ PUT / GET / REMOVE
- ✅ Configurable capacity
- ✅ Thread-safe operations
- ✅ Cache statistics
- ✅ Hit ratio
- ✅ Cache key inspection
- ✅ Functional test suite
- ✅ Concurrency tests
- ✅ Workload benchmarks
- ✅ Multithreaded benchmarks
- ✅ Stress testing with oversized working sets
- ✅ Read-heavy workloads
- ✅ Balanced workloads
- ✅ Write-heavy workloads
- ✅ High-churn workloads
- ✅ Scalability measurements

---

# 🧭 Development Philosophy

FlowCache is intended to evolve gradually.

Rather than adding features without measuring their impact, new functionality will be introduced alongside:

```text
Implementation
      ↓
Correctness Tests
      ↓
Benchmark
      ↓
Performance Analysis
      ↓
Optimization
      ↓
Regression Testing
```

This keeps the project focused on measurable engineering improvements.

---

# 🤝 Contributing

Contributions and ideas are welcome.

Potential areas for contribution include:

- Cache algorithms
- Concurrency improvements
- Benchmarking
- Testing
- Documentation
- Performance optimization
- Observability

Before submitting changes, ensure the project builds successfully and the existing test suite passes.

---

# 📜 License

License information will be added as the project moves toward its first stable release.

---

# 👨‍💻 Author

**Jayant Kumar**

M.Tech Computer Science & Engineering

---

# ⭐ Project Vision

FlowCache aims to evolve from a simple LRU cache implementation into a complete systems-oriented caching platform.

The long-term goal is to investigate how different cache architectures, eviction policies, synchronization strategies, and workload characteristics affect:

```text
Latency
  +
Throughput
  +
Memory Efficiency
  +
Concurrency
  +
Scalability
```

Ultimately, FlowCache is an experimental platform for studying **high-performance caching and concurrent systems in modern C++**.