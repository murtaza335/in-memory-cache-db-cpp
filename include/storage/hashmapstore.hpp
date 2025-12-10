#ifndef HASHMAPSTORE_HPP
#define HASHMAPSTORE_HPP

#include <string>
#include <unordered_map>
#include "storage/RedisHashMap.hpp"
#include "storage/RedisObject.hpp"

namespace hashmapstore {

    // Set field in hash
    std::string hset(RedisHashMap& map, const std::string& key,
                     const std::string& field, const std::string& value);

    // Get field from hash
    std::string hget(RedisHashMap& map, const std::string& key,
                     const std::string& field);

    // Delete field(s) from hash
    std::string hdel(RedisHashMap& map, const std::string& key,
                     const std::vector<std::string>& fields);

    // Get all fields and values
    std::string hgetall(RedisHashMap& map, const std::string& key);

    // Check if field exists
    std::string hexists(RedisHashMap& map, const std::string& key,
                        const std::string& field);

    // Get number of fields
    std::string hlen(RedisHashMap& map, const std::string& key);

}

#endif
