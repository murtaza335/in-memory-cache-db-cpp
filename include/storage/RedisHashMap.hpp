#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "RedisObject.hpp"
#include "murmurhash/murmurhash3.hpp"

struct HashEntry {
    std::string key;
    RedisObject value;

    HashEntry(const std::string& k, const RedisObject& v)
        : key(k), value(v) {}
};

class RedisHashMap {
private:
    std::vector<std::vector<HashEntry>> buckets;
    size_t capacity;

    size_t getIndex(const std::string& key) const;

public:
    RedisHashMap(size_t size = 1024); // default 1024 buckets

    // ---------- Key management ----------
    bool add(const std::string& key, const RedisObject& value);
    bool del(const std::string& key);
    bool exists(const std::string& key) const;
    bool rename(const std::string& oldKey, const std::string& newKey);
    bool copy(const std::string& sourceKey, const std::string& destKey);

    // ---------- Value access ----------
    RedisObject* get(const std::string& key);
};
