#include "storage/TTLPriorityQueue.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

// this file implements the ttl priority queue used to track expiring keys.
// each entry is stored with its expiration timestamp and ordered by soonest-to-expire.
// the database uses this queue to efficiently process expired items.
// it ensures that ttl checks are fast even at large scale.

// logging utility with simple timestamp
static std::string getTimestamp() {
    auto now_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now_t));
    return std::string(buf);
}

// ttl priority q implementation

TTLPriorityQueue::TTLPriorityQueue(RedisHashMap* db)
    : dbPtr(db), running(false) {
    // worker started automatically if start(db) enabled
}

TTLPriorityQueue::~TTLPriorityQueue() {
    stop();
}

void TTLPriorityQueue::start(RedisHashMap* db) {
    std::lock_guard<std::mutex> lock(mu);
    if (running.load()) {
        // if db null and now provided then update the pointer
        if (db && !dbPtr) dbPtr = db;
        return;
    }
    if (!db && !dbPtr) {
        std::cerr << "[" << getTimestamp() << "] [ERROR] TTLPriorityQueue::start requires a valid RedisHashMap* db\n";
        return;
    }
    if (db) dbPtr = db;
    running.store(true);
    worker = std::thread(&TTLPriorityQueue::workerLoop, this);
    std::cout << "[" << getTimestamp() << "] [INFO] TTLPriorityQueue started\n";
}

void TTLPriorityQueue::stop() {
    bool hadThread = false;
    {
        std::lock_guard<std::mutex> lk(cvMu);
        if (running.load()) {
            running.store(false);
            hadThread = true;
            cv.notify_all();
        }
    }
    if (hadThread && worker.joinable()) {
        worker.join();
        std::cout << "[" << getTimestamp() << "] [INFO] TTLPriorityQueue stopped\n";
    }
}

size_t TTLPriorityQueue::size() const {
    std::lock_guard<std::mutex> lock(mu);
    return heap.size();
}

bool TTLPriorityQueue::insertOrUpdate(const std::string& key, long long seconds) {
    // db existence check will return false if db key doesnt exist
    if (!dbPtr) {
        std::cerr << "[" << getTimestamp() << "] [ERROR] TTLPriorityQueue: dbPtr is null in insertOrUpdate\n";
        return false;
    }
    if (!dbPtr->exists(key)) {
        return false; // no ttl setup for existing key just like redis
    }

    auto expiry = now() + std::chrono::seconds(seconds);

    std::lock_guard<std::mutex> lock(mu);
    auto it = indexMap.find(key);
    if (it != indexMap.end()) {
        // expiry reheapify update
        size_t idx = it->second;
        heap[idx].expireAt = expiry;
        // up or down reheapify depending on ther quantiry
        heapifyUp(idx);
        heapifyDown(idx);
        return true;
    }

    // new insert
    ttlObject obj;
    obj.key = key;
    obj.expireAt = expiry;
    heap.push_back(std::move(obj));
    size_t idx = heap.size() - 1;
    indexMap[key] = idx;
    heapifyUp(idx);
    return true;
}

bool TTLPriorityQueue::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mu);
    auto it = indexMap.find(key);
    if (it == indexMap.end()) return false;

    size_t idx = it->second;
    size_t last = heap.size() - 1;

    if (idx != last) {
        swapNodes(idx, last);
    }
    // remove last
    indexMap.erase(key);
    heap.pop_back();

    if (idx < heap.size()) {
        // heapify the node now at idx
        heapifyUp(idx);
        heapifyDown(idx);
    }
    return true;
}

long long TTLPriorityQueue::getTTLSeconds(const std::string& key) const {
    if (!dbPtr) return -2; // if no db then treat as absent

    // first check DB presence
    if (!dbPtr->exists(key)) return -2;

    std::lock_guard<std::mutex> lock(mu);
    auto it = indexMap.find(key);
    if (it == indexMap.end()) return -1; // no ttl but exists

    size_t idx = it->second;
    auto expireAt = heap[idx].expireAt;
    auto nowtp = now();
    if (expireAt <= nowtp) return 0;
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(expireAt - nowtp).count();
    return static_cast<long long>(diff);
}

// heap helper methods

void TTLPriorityQueue::swapNodes(size_t a, size_t b) {
    assert(a < heap.size() && b < heap.size());
    std::swap(heap[a], heap[b]);
    // updating indexMap
    indexMap[heap[a].key] = a;
    indexMap[heap[b].key] = b;
}

void TTLPriorityQueue::heapifyUp(size_t idx) {
    if (idx >= heap.size()) return;
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (heap[idx].expireAt < heap[parent].expireAt) {
            swapNodes(idx, parent);
            idx = parent;
        } else break;
    }
}

void TTLPriorityQueue::heapifyDown(size_t idx) {
    size_t n = heap.size();
    if (idx >= n) return;
    while (true) {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        size_t smallest = idx;
        if (left < n && heap[left].expireAt < heap[smallest].expireAt) smallest = left;
        if (right < n && heap[right].expireAt < heap[smallest].expireAt) smallest = right;
        if (smallest != idx) {
            swapNodes(idx, smallest);
            idx = smallest;
        } else break;
    }
}

std::string TTLPriorityQueue::popRootNoLock() {
    if (heap.empty()) return std::string();
    std::string key = heap[0].key;
    size_t last = heap.size() - 1;
    if (last == 0) {
        heap.pop_back();
        indexMap.erase(key);
        return key;
    }
    swapNodes(0, last);
    indexMap.erase(key);
    heap.pop_back();
    heapifyDown(0);
    return key;
}

void TTLPriorityQueue::processExpiredLocked() {
    auto nowtp = now();
    // pop all expired at root
    while (!heap.empty()) {
        if (heap[0].expireAt <= nowtp) {
            std::string keyToExpire = popRootNoLock();
            // we should perform DB delete without holding this lock to avoid potential deadlocks
            // especially if the DB internally interacts with the TTL queue
            // unlock before calling db->del to prevent re entrancy issues
            mu.unlock();
            // log and delete from DB
            std::cout << "[" << getTimestamp() << "] [INFO] TTL EXPIRE - Key expired: " << keyToExpire << std::endl;
            if (dbPtr) {
                dbPtr->del(keyToExpire);
            }
            mu.lock();
            // recompute now to avoid long loops based on stale time
            nowtp = now();
        } else {
            break;
        }
    }
}

void TTLPriorityQueue::workerLoop() {
    while (running.load()) {
        // wait for either the interval or stop signal
        {
            std::unique_lock<std::mutex> lk(cvMu);
            cv.wait_for(lk, workerInterval, [this](){ return !running.load(); });
        }
        if (!running.load()) break;

        // process expired entries each pop triggers db->del outside the lock
        // acquire mutex then processExpiredLocked it will release lock when calling db->del
        mu.lock();
        // process expired will pop nodes and call db->del while releasing mutex as needed.
        processExpiredLocked();
        mu.unlock();
    }
}

// global singleton accessor 
// single global pointer 
static TTLPriorityQueue* g_ttl = nullptr;

TTLPriorityQueue* getGlobalTTL(RedisHashMap* db) {
    // lazy init
    if (!g_ttl) {
        // create and start
        g_ttl = new TTLPriorityQueue(db);
        if (db) g_ttl->start(db);
    } else {
        // ensure started if db provided and not started
        if (db) g_ttl->start(db);
    }
    return g_ttl;
}

