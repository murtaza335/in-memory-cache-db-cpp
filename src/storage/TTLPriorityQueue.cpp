#include "storage/TTLPriorityQueue.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

// Simple timestamp utility for logging
static std::string getTimestamp() {
    auto now_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now_t));
    return std::string(buf);
}

// ------------------- TTLPriorityQueue implementation -------------------

TTLPriorityQueue::TTLPriorityQueue(RedisHashMap* db)
    : dbPtr(db), running(false) {
    // do not start worker automatically unless start(db) is called
}

TTLPriorityQueue::~TTLPriorityQueue() {
    stop();
}

void TTLPriorityQueue::start(RedisHashMap* db) {
    std::lock_guard<std::mutex> lock(mu);
    if (running.load()) {
        // Already running; if db was null and now provided, update pointer
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
    // Need DB existence check: if key doesn't exist in DB, return false.
    if (!dbPtr) {
        std::cerr << "[" << getTimestamp() << "] [ERROR] TTLPriorityQueue: dbPtr is null in insertOrUpdate\n";
        return false;
    }
    if (!dbPtr->exists(key)) {
        return false; // match Redis behavior: cannot set TTL for non-existing key
    }

    auto expiry = now() + std::chrono::seconds(seconds);

    std::lock_guard<std::mutex> lock(mu);
    auto it = indexMap.find(key);
    if (it != indexMap.end()) {
        // update expiry and reheapify
        size_t idx = it->second;
        heap[idx].expireAt = expiry;
        // reheapify up or down depending on new expiry
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
    if (!dbPtr) return -2; // no db -> treat as absent

    // first check DB presence
    if (!dbPtr->exists(key)) return -2;

    std::lock_guard<std::mutex> lock(mu);
    auto it = indexMap.find(key);
    if (it == indexMap.end()) return -1; // exists but no TTL

    size_t idx = it->second;
    auto expireAt = heap[idx].expireAt;
    auto nowtp = now();
    if (expireAt <= nowtp) return 0;
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(expireAt - nowtp).count();
    return static_cast<long long>(diff);
}

// -------- heap helpers --------

void TTLPriorityQueue::swapNodes(size_t a, size_t b) {
    assert(a < heap.size() && b < heap.size());
    std::swap(heap[a], heap[b]);
    // update indexMap
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
            // call DB delete outside lock? we must call without holding lock to avoid deadlock if DB uses TTL queue again.
            // But currently we are in locked context. We'll release lock when calling db->del to avoid reentrancy issues.
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
        // Wait for either the interval or stop signal
        {
            std::unique_lock<std::mutex> lk(cvMu);
            cv.wait_for(lk, workerInterval, [this](){ return !running.load(); });
        }
        if (!running.load()) break;

        // Process expired entries
        // We will iterate popping expired items. Each pop will call db->del without holding the mutex (see processExpiredLocked).
        // Acquire lock then processExpiredLocked (which temporarily releases the mutex when calling db->del)
        mu.lock();
        // process expired will pop nodes and call db->del while releasing mutex as needed.
        processExpiredLocked();
        mu.unlock();
    }
}

// ----------------- global singleton accessor -----------------

// Single global pointer (internal linkage)
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

