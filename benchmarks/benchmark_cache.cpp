#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "flowcache/Cache.h"

using namespace std;
using namespace chrono;

struct Workload
{
    string name;

    int getPercent;
    int putPercent;
    int removePercent;
};

struct BenchmarkResult
{
    double throughput;
    flowcache::CacheStats stats;
};

BenchmarkResult runOnce(
    const Workload& workload,
    int threadCount,
    int operationsPerThread,
    size_t cacheCapacity,
    size_t workingSet)
{
    flowcache::Cache cache(cacheCapacity);

    // Pre-populate the cache.
    for (size_t i = 0; i < min(cacheCapacity, workingSet); i++)
    {
        cache.put(
            "key_" + to_string(i),
            "initial_value");
    }

    auto worker = [&](int threadId)
    {
        for (int i = 0; i < operationsPerThread; i++)
        {
            int operation = i % 100;

            // Spread accesses across the entire working set.
            size_t keyIndex =
                (static_cast<size_t>(threadId) *
                     operationsPerThread +
                 static_cast<size_t>(i)) %
                workingSet;

            string key =
                "key_" + to_string(keyIndex);

            if (operation < workload.getPercent)
            {
                // GET
                cache.get(key);
            }
            else if (
                operation <
                workload.getPercent +
                    workload.putPercent)
            {
                // PUT
                cache.put(
                    key,
                    "value_" + to_string(threadId) +
                        "_" + to_string(i));
            }
            else
            {
                // REMOVE
                cache.remove(key);
            }
        }
    };

    vector<thread> threads;

    auto start =
        high_resolution_clock::now();

    for (int i = 0; i < threadCount; i++)
    {
        threads.emplace_back(
            worker,
            i);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    auto end =
        high_resolution_clock::now();

    auto duration =
        duration_cast<microseconds>(
            end - start);

    double seconds =
        duration.count() / 1'000'000.0;

    int totalOperations =
        threadCount * operationsPerThread;

    double throughput =
        totalOperations / seconds;

    BenchmarkResult result;

    result.throughput = throughput;
    result.stats = cache.stats();

    return result;
}

void runBenchmark(
    const Workload& workload,
    int threadCount,
    int operationsPerThread,
    int runs,
    size_t cacheCapacity,
    size_t workingSet,
    double baselineThroughput)
{
    vector<double> results;

    flowcache::CacheStats finalStats{};

    cout << "\n----------------------------------"
         << endl;

    cout << "Workload: "
         << workload.name
         << endl;

    cout << "Threads: "
         << threadCount
         << endl;

    cout << "Operations per run: "
         << threadCount * operationsPerThread
         << endl;

    cout << "Distribution: "
         << workload.getPercent
         << "% GET, "
         << workload.putPercent
         << "% PUT, "
         << workload.removePercent
         << "% REMOVE"
         << endl;

    cout << "Cache capacity: "
         << cacheCapacity
         << endl;

    cout << "Working set: "
         << workingSet
         << endl;

    cout << "Runs: "
         << runs
         << endl;

    for (int i = 0; i < runs; i++)
    {
        BenchmarkResult result =
            runOnce(
                workload,
                threadCount,
                operationsPerThread,
                cacheCapacity,
                workingSet);

        results.push_back(
            result.throughput);

        finalStats =
            result.stats;

        cout << "Run "
             << i + 1
             << ": "
             << result.throughput
             << " operations/sec"
             << endl;
    }

    double total = 0.0;

    for (double result : results)
    {
        total += result;
    }

    double average =
        total / results.size();

    double minimum =
        *min_element(
            results.begin(),
            results.end());

    double maximum =
        *max_element(
            results.begin(),
            results.end());

    cout << "\nSummary:"
         << endl;

    cout << "Average throughput: "
         << average
         << " operations/sec"
         << endl;

    cout << "Minimum throughput: "
         << minimum
         << " operations/sec"
         << endl;

    cout << "Maximum throughput: "
         << maximum
         << " operations/sec"
         << endl;

    if (baselineThroughput > 0.0)
    {
        double scalability =
            (average / baselineThroughput) *
            100.0;

        cout << "Scalability vs 1 thread: "
             << scalability
             << "%"
             << endl;
    }

    cout << "\nCache statistics:"
         << endl;

    cout << "Size: "
         << finalStats.size
         << endl;

    cout << "Capacity: "
         << finalStats.capacity
         << endl;

    cout << "Hits: "
         << finalStats.hits
         << endl;

    cout << "Misses: "
         << finalStats.misses
         << endl;

    cout << "Evictions: "
         << finalStats.evictions
         << endl;

    cout << "Hit ratio: "
         << finalStats.hitRatio * 100.0
         << "%"
         << endl;
}

int main()
{
    const int operationsPerThread =
        100000;

    const int runs = 3;

    const size_t cacheCapacity =
        10000;

    const size_t workingSet =
        100000;

    vector<Workload> workloads =
    {
        {
            "Read Heavy",
            90,
            5,
            5
        },
        {
            "Balanced",
            70,
            20,
            10
        },
        {
            "Write Heavy",
            30,
            50,
            20
        },
        {
            "High Churn",
            20,
            60,
            20
        }
    };

    cout << "=================================="
         << endl;

    cout << "   FlowCache Workload Benchmark   "
         << endl;

    cout << "=================================="
         << endl;

    cout << "Working set exceeds cache capacity"
         << endl;

    cout << "Cache capacity: "
         << cacheCapacity
         << endl;

    cout << "Working set: "
         << workingSet
         << endl;

    for (const auto& workload : workloads)
    {
        double baselineThroughput = 0.0;

        // First run with one thread.
        BenchmarkResult baseline =
            runOnce(
                workload,
                1,
                operationsPerThread,
                cacheCapacity,
                workingSet);

        baselineThroughput =
            baseline.throughput;

        runBenchmark(
            workload,
            1,
            operationsPerThread,
            runs,
            cacheCapacity,
            workingSet,
            baselineThroughput);

        runBenchmark(
            workload,
            4,
            operationsPerThread,
            runs,
            cacheCapacity,
            workingSet,
            baselineThroughput);

        runBenchmark(
            workload,
            8,
            operationsPerThread,
            runs,
            cacheCapacity,
            workingSet,
            baselineThroughput);
    }

    return 0;
}