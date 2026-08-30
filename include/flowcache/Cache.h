#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>

using namespace std;

namespace flowcache
{

    struct CacheStats
    {
        size_t size;
        size_t capacity;
        size_t hits;
        size_t misses;
        size_t evictions;
        double hitRatio;
    };

    class Cache
    {
    public:
        explicit Cache(size_t capacity);

        ~Cache();

        void put(const string &key, const string &value);

        optional<string> get(const string &key);

        bool remove(const string &key);

        size_t size() const;
        size_t capacity() const;

        size_t hits() const;
        size_t misses() const;
        size_t evictions() const;

        double hitRatio() const;

        vector<string> keys() const;

        CacheStats stats() const;

    private:
        mutable mutex mutex_;

        struct Node
        {
            string key;
            string value;

            Node *prev;
            Node *next;

            Node(const string &key, const string &value)
                : key(key),
                  value(value),
                  prev(nullptr),
                  next(nullptr) {}
        };

        size_t capacity_;

        size_t hits_;
        size_t misses_;
        size_t evictions_;

        unordered_map<string, Node *> data_;

        Node *head_;
        Node *tail_;

        void addToFront(Node *node);
        void removeNode(Node *node);
        void moveToFront(Node *node);
        void evictLRU();
    };

}