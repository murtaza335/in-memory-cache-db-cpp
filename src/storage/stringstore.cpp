#include "storage/stringstore.hpp"
#include <iostream>

namespace stringstore {

std::string set(RedisHashMap& db, const std::string& key, const std::string& value) {
    RedisObject obj(value);        
    db.add(key, obj);
    return "+OK";
}

std::string get(RedisHashMap& db, const std::string& key) {
    RedisObject* obj = db.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::STRING) return "-ERR wrong type";
    return obj->getValue<std::string>();
}

std::string del(RedisHashMap& db, const std::string& key) {
    return db.del(key) ? ":1" : ":0";
}

std::string exists(RedisHashMap& db, const std::string& key) {
    return db.exists(key) ? ":1" : ":0";
}

std::string rename(RedisHashMap& db, const std::string& oldKey, const std::string& newKey) {
    return db.rename(oldKey, newKey) ? "+OK" : "-ERR key does not exist";
}

std::string copy(RedisHashMap& db, const std::string& sourceKey, const std::string& destKey) {
    return db.copy(sourceKey, destKey) ? "+OK" : "-ERR source key does not exist";
}

} // namespace stringstore
