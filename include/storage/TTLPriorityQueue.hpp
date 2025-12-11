#ifndef TTL_PRIORITY_QUEUE_HPP
#define TTL_PRIORITY_QUEUE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include "storage\RedisHashMap.hpp"

/*
 * TTLPriorityQueue
 *
 * Maintains a min-heap (by expiry time) of ttlObject entries. Provides:
 *  - insert/update TTL for a key
 *  - remove a key from the queue
 *  - get remaining TTL for a key
 *  - background thread (10s wake) that removes expired keys and calls db->del(key)
 *
 * NOTE: This class is thread-safe for its public methods.
 */

struct ttlObject {
    std::string key;
    std::chrono::system_clock::time_point expireAt;
};

class TTLPriorityQueue {
public:
    explicit TTLPriorityQueue(RedisHashMap* db = nullptr);
    ~TTLPriorityQueue();

    // Start worker (if not started). db pointer is required for expiration deletes.
    void start(RedisHashMap* db);

    // Stop worker thread and clean up.
    void stop();

    // Insert or update TTL for key (seconds from now). If seconds <= 0, treat as immediate expiration.
    // Returns true if inserted/updated; false if key didn't exist in DB (and no action taken).
    bool insertOrUpdate(const std::string& key, long long seconds);

    // Remove TTL entry for key if present. Does NOT delete key from DB.
    // Returns true if removed from the TTL queue, false if not found.
    bool remove(const std::string& key);

    // Query remaining TTL in seconds.
    // Returns:
    //  >=0 : seconds remaining
    //  -1  : key exists in DB but no TTL associated
    //  -2  : key does not exist in DB
    long long getTTLSeconds(const std::string& key) const;

    // Size of the heap
    size_t size() const;

private:
    // Heap helpers
    void heapifyUp(size_t idx);
    void heapifyDown(size_t idx);
    void swapNodes(size_t a, size_t b);

    // Remove root (assumes mutex held) and return key of popped item.
    std::string popRootNoLock();

    // Process any expired keys up to now (assumes mutex held if called privately)
    void processExpiredLocked();

    // Worker thread function
    void workerLoop();

private:
    mutable std::mutex mu;
    std::vector<ttlObject> heap;
    std::unordered_map<std::string, size_t> indexMap; // key -> index in heap

    RedisHashMap* dbPtr; // not owned
    std::thread worker;
    std::atomic<bool> running;
    std::condition_variable cv;
    mutable std::mutex cvMu;
    std::chrono::seconds workerInterval{10};

    // small helper to get current time
    static std::chrono::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }
};

// Global accessor - create on demand. Implementation in TTLPriorityQueue.cpp
TTLPriorityQueue* getGlobalTTL(RedisHashMap* db = nullptr);

#endif // TTL_PRIORITY_QUEUE_HPP
