#include "storage/RedisHashMap.hpp"
#include <iostream>
#include <chrono>

// Utility function for timestamp
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return buffer;
}

// -------------------- Constructor --------------------
RedisHashMap::RedisHashMap(size_t size)
    : capacity(size)
{
    buckets.resize(capacity);
    std::cout << "[" << getTimestamp() << "] [INFO] RedisHashMap initialized - Capacity: " 
              << capacity << ", Load factor: " << loadFactor << std::endl;
}

// -------------------- Compute bucket index --------------------
size_t RedisHashMap::getIndex(const std::string& key) const {
    uint32_t hash = MurmurHash3_x86_32(key);
    return hash % capacity;
}

// resize method
void RedisHashMap::resize(size_t newCapacity) {
    std::cout << "[" << getTimestamp() << "] [INFO] RESIZE operation started - Old capacity: " 
              << capacity << ", New capacity: " << newCapacity << ", Current entries: " << count << std::endl;
    
    std::vector<std::vector<HashEntry>> newBuckets;
    newBuckets.resize(newCapacity);

    // Rehash all entries into the new bucket table
    for (auto& bucket : buckets) {
        for (auto& entry : bucket) {
            uint32_t hash = MurmurHash3_x86_32(entry.key);
            size_t newIdx = hash % newCapacity;
            newBuckets[newIdx].emplace_back(entry.key, entry.value);
        }
    }

    buckets.swap(newBuckets);
    capacity = newCapacity;
    
    float newLoadFactor = (float)count / (float)capacity;
    std::cout << "[" << getTimestamp() << "] [INFO] RESIZE completed - New capacity: " 
              << capacity << ", New load factor: " << newLoadFactor << std::endl;
}


// -------------------- Add/Insert --------------------
bool RedisHashMap::add(const std::string& key, const RedisObject& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] ADD operation - Key: " << key 
              << ", Current entries: " << count << std::endl;
    
    size_t idx = getIndex(key);
    auto& bucket = buckets[idx];

    // Check if key already exists → replace
    for (auto& entry : bucket) {
        if (entry.key == key) {
            entry.value = value;
            std::cout << "[" << getTimestamp() << "] [INFO] ADD - Key updated (already existed): " 
                      << key << ", Bucket index: " << idx << std::endl;
            return true;
        }
    }

    bucket.emplace_back(key, value);
    count++;
    float currentLoadFactor = (float)count / (float)capacity;
    
    std::cout << "[" << getTimestamp() << "] [INFO] ADD - New key inserted: " << key 
              << ", Bucket index: " << idx << ", Total entries: " << count 
              << ", Load factor: " << currentLoadFactor << std::endl;

    // Check load factor
    if (currentLoadFactor > loadFactor) {
        std::cout << "[" << getTimestamp() << "] [WARN] ADD - Load factor exceeded (" 
                  << currentLoadFactor << "), triggering resize..." << std::endl;
        resize(capacity * 2);     // double the size
    }
    return true;
}

// -------------------- Delete --------------------
bool RedisHashMap::del(const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] DEL operation - Key: " << key 
              << ", Current entries: " << count << std::endl;
    
    size_t idx = getIndex(key);
    auto& bucket = buckets[idx];

    auto it = std::find_if(bucket.begin(), bucket.end(),
        [&](const HashEntry& e) { return e.key == key; });

    if (it != bucket.end()) {
        bucket.erase(it);
        count--;     // decrement count
        std::cout << "[" << getTimestamp() << "] [INFO] DEL - SUCCESS - Key deleted: " << key 
                  << ", Bucket index: " << idx << ", Remaining entries: " << count << std::endl;
        return true;
    }

    std::cout << "[" << getTimestamp() << "] [WARN] DEL - Key not found: " << key << std::endl;
    return false; // not found
}

// -------------------- Exists --------------------
bool RedisHashMap::exists(const std::string& key) const {
    size_t idx = getIndex(key);
    const auto& bucket = buckets[idx];

    bool found = std::any_of(bucket.begin(), bucket.end(),
        [&](const HashEntry& e) { return e.key == key; });
    
    std::cout << "[" << getTimestamp() << "] [INFO] EXISTS - Key: " << key 
              << ", Exists: " << (found ? "YES" : "NO") << ", Bucket index: " << idx << std::endl;
    
    return found;
}

// -------------------- Rename --------------------
bool RedisHashMap::rename(const std::string& oldKey, const std::string& newKey) {
    std::cout << "[" << getTimestamp() << "] [INFO] RENAME operation - Old key: " << oldKey 
              << ", New key: " << newKey << std::endl;
    
    size_t oldIdx = getIndex(oldKey);
    size_t newIdx = getIndex(newKey);

    auto& oldBucket = buckets[oldIdx];

    auto it = std::find_if(oldBucket.begin(), oldBucket.end(),
        [&](const HashEntry& e) { return e.key == oldKey; });

    if (it == oldBucket.end()) {
        std::cout << "[" << getTimestamp() << "] [ERROR] RENAME - Old key not found: " << oldKey << std::endl;
        return false; // oldKey not found
    }

    RedisObject value = it->value;
    oldBucket.erase(it);

    // Insert newKey
    buckets[newIdx].emplace_back(newKey, value);
    std::cout << "[" << getTimestamp() << "] [INFO] RENAME - SUCCESS - Old key: " << oldKey 
              << " → New key: " << newKey << ", Old bucket: " << oldIdx 
              << ", New bucket: " << newIdx << std::endl;
    return true;
}

// -------------------- Copy --------------------
bool RedisHashMap::copy(const std::string& sourceKey, const std::string& destKey) {
    std::cout << "[" << getTimestamp() << "] [INFO] COPY operation - Source key: " << sourceKey 
              << ", Dest key: " << destKey << std::endl;
    
    size_t srcIdx = getIndex(sourceKey);
    auto& srcBucket = buckets[srcIdx];

    auto it = std::find_if(srcBucket.begin(), srcBucket.end(),
        [&](const HashEntry& e) { return e.key == sourceKey; });

    if (it == srcBucket.end()) {
        std::cout << "[" << getTimestamp() << "] [ERROR] COPY - Source key not found: " << sourceKey << std::endl;
        return false; // sourceKey not found
    }

    RedisObject value = it->value;
    add(destKey, value);
    std::cout << "[" << getTimestamp() << "] [INFO] COPY - SUCCESS - Source: " << sourceKey 
              << ", Destination: " << destKey << ", Total entries: " << count << std::endl;
    return true;
}

// -------------------- Get --------------------
RedisObject* RedisHashMap::get(const std::string& key) {
    size_t idx = getIndex(key);
    auto& bucket = buckets[idx];

    auto it = std::find_if(bucket.begin(), bucket.end(),
        [&](const HashEntry& e) { return e.key == key; });

    if (it != bucket.end()) {
        std::cout << "[" << getTimestamp() << "] [INFO] GET - SUCCESS - Key: " << key 
                  << ", Bucket index: " << idx << std::endl;
        return &it->value;
    }

    std::cout << "[" << getTimestamp() << "] [WARN] GET - Key not found: " << key 
              << ", Bucket index: " << idx << std::endl;
    return nullptr; // not found
}

