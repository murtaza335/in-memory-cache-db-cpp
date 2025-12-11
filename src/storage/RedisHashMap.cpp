#include "storage/RedisHashMap.hpp"

// -------------------- Constructor --------------------
RedisHashMap::RedisHashMap(size_t size)
    : capacity(size)
{
    buckets.resize(capacity);
}

// -------------------- Compute bucket index --------------------
size_t RedisHashMap::getIndex(const std::string& key) const {
    uint32_t hash = MurmurHash3_x86_32(key);
    return hash % capacity;
}

// resize method
void RedisHashMap::resize(size_t newCapacity) {
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
}


// -------------------- Add/Insert --------------------
bool RedisHashMap::add(const std::string& key, const RedisObject& value) {
    size_t idx = getIndex(key);
    auto& bucket = buckets[idx];

    // Check if key already exists → replace
    for (auto& entry : bucket) {
        if (entry.key == key) {
            entry.value = value;
            return true;
        }
    }

    bucket.emplace_back(key, value);
    count++;

    // Check load factor
    if ((float)count / (float)capacity > loadFactor) {
        resize(capacity * 2);     // double the size
    }
    return true;
}

// -------------------- Delete --------------------
bool RedisHashMap::del(const std::string& key) {
    size_t idx = getIndex(key);
    auto& bucket = buckets[idx];

    auto it = std::find_if(bucket.begin(), bucket.end(),
        [&](const HashEntry& e) { return e.key == key; });

    if (it != bucket.end()) {
        bucket.erase(it);
        count--;     // decrement count
        return true;
    }

    return false; // not found
}

// -------------------- Exists --------------------
bool RedisHashMap::exists(const std::string& key) const {
    size_t idx = getIndex(key);
    const auto& bucket = buckets[idx];

    return std::any_of(bucket.begin(), bucket.end(),
        [&](const HashEntry& e) { return e.key == key; });
}

// -------------------- Rename --------------------
bool RedisHashMap::rename(const std::string& oldKey, const std::string& newKey) {
    size_t oldIdx = getIndex(oldKey);
    size_t newIdx = getIndex(newKey);

    auto& oldBucket = buckets[oldIdx];

    auto it = std::find_if(oldBucket.begin(), oldBucket.end(),
        [&](const HashEntry& e) { return e.key == oldKey; });

    if (it == oldBucket.end()) return false; // oldKey not found

    RedisObject value = it->value;
    oldBucket.erase(it);

    // Insert newKey
    buckets[newIdx].emplace_back(newKey, value);
    return true;
}

// -------------------- Copy --------------------
bool RedisHashMap::copy(const std::string& sourceKey, const std::string& destKey) {
    size_t srcIdx = getIndex(sourceKey);
    auto& srcBucket = buckets[srcIdx];

    auto it = std::find_if(srcBucket.begin(), srcBucket.end(),
        [&](const HashEntry& e) { return e.key == sourceKey; });

    if (it == srcBucket.end()) return false; // sourceKey not found

    RedisObject value = it->value;
    add(destKey, value);
    return true;
}

// -------------------- Get --------------------
RedisObject* RedisHashMap::get(const std::string& key) {
    size_t idx = getIndex(key);
    auto& bucket = buckets[idx];

    auto it = std::find_if(bucket.begin(), bucket.end(),
        [&](const HashEntry& e) { return e.key == key; });

    if (it != bucket.end()) return &it->value;

    return nullptr; // not found
}

