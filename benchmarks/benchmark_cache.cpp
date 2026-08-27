#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "flowcache/Cache.h"

using namespace std;
using namespace chrono;

void worker(
    flowcache::Cache& cache,
    int operations
) {

    for (int i = 0; i < operations; i++) {

        int operation = i % 10;

        string key =
            "key_" + to_string(i % 1000);

        if (operation < 7) {

            // 70% GET
            cache.get(key);

        } else if (operation < 9) {

            // 20% PUT
            cache.put(
                key,
                "value_" + to_string(i)
            );

        } else {

            // 10% REMOVE
            cache.remove(key);
        }
    }
}

double runOnce(
    int threadCount,
    int operationsPerThread
) {

    const size_t cacheCapacity = 100000;

    flowcache::Cache cache(cacheCapacity);

    // Pre-populate shared working set.
    for (int i = 0; i < 1000; i++) {

        cache.put(
            "key_" + to_string(i),
            "initial_value"
        );
    }

    vector<thread> threads;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < threadCount; i++) {

        threads.emplace_back(
            worker,
            ref(cache),
            operationsPerThread
        );
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = high_resolution_clock::now();

    auto duration =
        duration_cast<microseconds>(end - start);

    double seconds =
        duration.count() / 1'000'000.0;

    int totalOperations =
        threadCount * operationsPerThread;

    double throughput =
        totalOperations / seconds;

    return throughput;
}

void runBenchmark(
    int threadCount,
    int operationsPerThread,
    int runs
) {

    vector<double> results;

    cout << "\n----------------------------------" << endl;
    cout << "Threads: " << threadCount << endl;
    cout << "Operations per run: "
         << threadCount * operationsPerThread
         << endl;
    cout << "Runs: " << runs << endl;

    for (int i = 0; i < runs; i++) {

        double throughput =
            runOnce(
                threadCount,
                operationsPerThread
            );

        results.push_back(throughput);

        cout << "Run " << i + 1 << ": "
             << throughput
             << " operations/sec"
             << endl;
    }

    double total = 0.0;

    for (double result : results) {
        total += result;
    }

    double average =
        total / results.size();

    double minimum =
        *min_element(results.begin(), results.end());

    double maximum =
        *max_element(results.begin(), results.end());

    cout << "\nSummary:" << endl;

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
}

int main() {

    const int operationsPerThread = 100000;
    const int runs = 5;

    cout << "==================================" << endl;
    cout << "   FlowCache Benchmark Suite      " << endl;
    cout << "==================================" << endl;

    cout << "Workload: 70% GET, 20% PUT, 10% REMOVE"
         << endl;

    runBenchmark(
        1,
        operationsPerThread,
        runs
    );

    runBenchmark(
        4,
        operationsPerThread,
        runs
    );

    runBenchmark(
        8,
        operationsPerThread,
        runs
    );

    return 0;
}