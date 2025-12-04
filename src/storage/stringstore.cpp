#include "storage/stringstore.hpp"

// Single global RedisHashMap instance for stringstore
static RedisHashMap baseMap(1024); // 1024 buckets, you can adjust

namespace stringstore {

std::string set(const std::string& key, const std::string& value) {
    RedisObject obj(value);        // wrap value in RedisObject
    baseMap.add(key, obj);
    return "+OK";
}

std::string get(const std::string& key) {
    RedisObject* obj = baseMap.get(key);
    if (!obj) return "$-1";       // Redis-style nil
    if (obj->getType() != RedisType::STRING) return "-ERR wrong type";
    return obj->getValue<std::string>();
}

std::string del(const std::string& key) {
    if (baseMap.del(key)) return ":1";  // 1 key deleted
    return ":0";                        // key not found
}

std::string exists(const std::string& key) {
    return baseMap.exists(key) ? ":1" : ":0";
}

std::string rename(const std::string& oldKey, const std::string& newKey) {
    if (baseMap.rename(oldKey, newKey)) return "+OK";
    return "-ERR key does not exist";
}

std::string copy(const std::string& sourceKey, const std::string& destKey) {
    if (baseMap.copy(sourceKey, destKey)) return "+OK";
    return "-ERR source key does not exist";
}

} // namespace stringstore
