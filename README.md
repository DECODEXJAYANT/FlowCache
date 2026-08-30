# FlowCache

## High-Performance Concurrent In-Memory Cache Engine

FlowCache is a **thread-safe, high-performance in-memory cache engine written in modern C++**.

It is designed as a systems-oriented project to explore how in-memory caching systems work internally, with a focus on **fast lookup, LRU eviction, concurrency, cache statistics, workload simulation, and performance analysis**.

FlowCache uses a **hash table (`std::unordered_map`)** for O(1) average key lookup and a **doubly linked list (`std::list`)** to maintain Least Recently Used (LRU) ordering.

The project includes:

- Core cache operations
- LRU eviction
- Thread-safe concurrent access
- Runtime cache statistics
- Cache key inspection
- Automated test suite
- Multithreaded benchmarks
- Multiple realistic workload patterns
- Stress testing with working sets larger than cache capacity
- Scalability analysis

---

# Table of Contents

- [Overview](#overview)
- [Why FlowCache?](#why-flowcache)
- [Architecture](#architecture)
- [Core Features](#core-features)
- [LRU Design](#lru-design)
- [Thread Safety](#thread-safety)
- [Cache Statistics](#cache-statistics)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Building the Project](#building-the-project)
- [Running the Cache](#running-the-cache)
- [Running Tests](#running-tests)
- [Benchmarking](#benchmarking)
- [Benchmark Workloads](#benchmark-workloads)
- [Performance Results](#performance-results)
- [API Overview](#api-overview)
- [Example Usage](#example-usage)
- [Correctness Testing](#correctness-testing)
- [Design Decisions](#design-decisions)
- [Complexity Analysis](#complexity-analysis)
- [Limitations](#limitations)
- [Future Work](#future-work)
- [Learning Outcomes](#learning-outcomes)
- [Roadmap](#roadmap)
- [Conclusion](#conclusion)

---

# Overview

Modern applications frequently access the same data repeatedly.

Instead of retrieving data from a slower backend, database, or external service every time, frequently accessed data can be temporarily stored in memory.

This is the fundamental idea behind caching.

FlowCache implements a simplified but realistic cache engine that provides:

```text
Application
     |
     v
+----------------------+
|      FlowCache       |
|                      |
|  Hash Table          |
|       +              |
|  LRU Linked List     |
|       +              |
|  Statistics          |
|       +              |
|  Thread Safety       |
+----------------------+
     |
     v
Fast In-Memory Access