#ifndef REDIS_SETS_HPP
#define REDIS_SETS_HPP

#include "RedisHashMap.hpp"
#include "RedisObject.hpp"
#include <string>
#include <unordered_set>
#include <vector>

namespace setstore {

    // ---------------- Basic Set Commands ----------------
    std::string sadd(RedisHashMap& map, const std::string& key, const std::string& value);
    std::string srem(RedisHashMap& map, const std::string& key, const std::string& value);
    std::string smembers(RedisHashMap& map, const std::string& key);
    std::string scard(RedisHashMap& map, const std::string& key);
    std::string spop(RedisHashMap& map, const std::string& key);
    std::string sismember(RedisHashMap& map, const std::string& key, const std::string& value);

    // ---------------- Set Operations ----------------
    std::string sunion(RedisHashMap& map, const std::string& key1, const std::string& key2);
    std::string sinter(RedisHashMap& map, const std::string& key1, const std::string& key2);
    std::string sdiff(RedisHashMap& map, const std::string& key1, const std::string& key2);

}

#endif // REDIS_SETS_HPP
