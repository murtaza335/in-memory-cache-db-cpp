#ifndef STRINGSTORE_HPP
#define STRINGSTORE_HPP

#include <string>
#include <vector>
#include "storage/RedisHashMap.hpp"
#include "storage/RedisObject.hpp"

namespace stringstore {

    // Basic string commands
    std::string set(RedisHashMap& db, const std::string& key, const std::string& value);
    std::string get(RedisHashMap& db, const std::string& key);
    std::string del(RedisHashMap& db, const std::string& key);
    std::string exists(RedisHashMap& db, const std::string& key);
    std::string rename(RedisHashMap& db, const std::string& oldKey, const std::string& newKey);
    std::string copy(RedisHashMap& db, const std::string& sourceKey, const std::string& destKey);

    // Extended string commands
    std::string setnx(RedisHashMap& db, const std::string& key, const std::string& value);
    std::string mset(RedisHashMap& db, const std::vector<std::string>& kvs);
    std::string mget(RedisHashMap& db, const std::vector<std::string>& keys);
    std::string append(RedisHashMap& db, const std::string& key, const std::string& value);
    std::string strlen_(RedisHashMap& db, const std::string& key);

    // Increment / Decrement commands
    std::string incr(RedisHashMap& db, const std::string& key);
    std::string incrby(RedisHashMap& db, const std::string& key, const std::string& amount);
    std::string decr(RedisHashMap& db, const std::string& key);
    std::string decrby(RedisHashMap& db, const std::string& key, const std::string& amount);

    // Expire stub
    std::string expire(RedisHashMap& db, const std::string& key, const std::string& seconds);

} // namespace stringstore

#endif // STRINGSTORE_HPP
