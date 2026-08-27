#include "flowcache/Cache.h"

using namespace std;

namespace flowcache {

Cache::Cache(size_t capacity) {
    capacity_ = capacity;

    hits_ = 0;
    misses_ = 0;
    evictions_ = 0;

    head_ = nullptr;
    tail_ = nullptr;
}

Cache::~Cache() {
    Node* current = head_;

    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

void Cache::addToFront(Node* node) {
    node->next = head_;
    node->prev = nullptr;

    if (head_ != nullptr) {
        head_->prev = node;
    }

    head_ = node;

    if (tail_ == nullptr) {
        tail_ = node;
    }
}

void Cache::removeNode(Node* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        head_ = node->next;
    }

    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        tail_ = node->prev;
    }
}

void Cache::moveToFront(Node* node) {
    removeNode(node);
    addToFront(node);
}

void Cache::evictLRU() {
    if (tail_ == nullptr) {
        return;
    }

    Node* node = tail_;

    removeNode(node);

    data_.erase(node->key);

    delete node;

    evictions_++;
}

void Cache::put(const string& key, const string& value) {

    // Check if key already exists
    auto it = data_.find(key);

    if (it != data_.end()) {
        Node* node = it->second;

        node->value = value;

        moveToFront(node);

        return;
    }

    // Create a new node
    Node* node = new Node(key, value);

    data_[key] = node;

    addToFront(node);

    // Check capacity
    if (data_.size() > capacity_) {
        evictLRU();
    }
}

optional<string> Cache::get(const string& key) {

    auto it = data_.find(key);

    if (it == data_.end()) {
        misses_++;

        return nullopt;
    }

    hits_++;

    Node* node = it->second;

    moveToFront(node);

    return node->value;
}

bool Cache::remove(const string& key) {

    auto it = data_.find(key);

    if (it == data_.end()) {
        return false;
    }

    Node* node = it->second;

    removeNode(node);

    data_.erase(it);

    delete node;

    return true;
}

size_t Cache::size() const {
    return data_.size();
}

size_t Cache::capacity() const {
    return capacity_;
}

size_t Cache::hits() const {
    return hits_;
}

size_t Cache::misses() const {
    return misses_;
}

size_t Cache::evictions() const {
    return evictions_;
}

double Cache::hitRatio() const {

    size_t totalRequests = hits_ + misses_;

    if (totalRequests == 0) {
        return 0.0;
    }

    return static_cast<double>(hits_) / totalRequests;
}

} 