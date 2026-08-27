cmake_minimum_required(VERSION 3.20)

project(FlowCache
    VERSION 0.1.0
    DESCRIPTION "High-performance concurrent in-memory cache engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(flowcache
    src/main.cpp
    src/Cache.cpp
)

target_include_directories(flowcache
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(flowcache_tests
    tests/test_cache.cpp
    src/Cache.cpp
)

target_include_directories(flowcache_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(flowcache_benchmark
    benchmarks/benchmark_cache.cpp
    src/Cache.cpp
)

target_include_directories(flowcache_benchmark
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)