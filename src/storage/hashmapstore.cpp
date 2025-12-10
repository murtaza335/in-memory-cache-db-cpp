#include "storage/hashmapstore.hpp"
#include "storage/RedisObject.hpp"
#include <sstream>

namespace hashmapstore {

// -------------------- HSET --------------------
std::string hset(RedisHashMap& map, const std::string& key,
                 const std::string& field, const std::string& value) {

    RedisObject* obj = map.get(key);
    std::unordered_map<std::string, RedisObject>* hash;

    if (!obj) {
        hash = new std::unordered_map<std::string, RedisObject>();
        hash->insert_or_assign(field, RedisObject(value));
        map.add(key, RedisObject(*hash));
        return ":1";
    }

    if (obj->getType() != RedisType::HASH)
        return "-ERR wrong type";

    hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    bool isNew = hash->find(field) == hash->end();
    hash->insert_or_assign(field, RedisObject(value));

    return isNew ? ":1" : ":0";
}

// -------------------- HGET --------------------
std::string hget(RedisHashMap& map, const std::string& key,
                 const std::string& field) {
    RedisObject* obj = map.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::HASH) return "-ERR wrong type";

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    auto it = hash->find(field);
    if (it == hash->end()) return "$-1";

    return it->second.getValue<std::string>();
}

// -------------------- HDEL --------------------
std::string hdel(RedisHashMap& map, const std::string& key,
                 const std::vector<std::string>& fields) {
    RedisObject* obj = map.get(key);
    if (!obj) return ":0";
    if (obj->getType() != RedisType::HASH) return "-ERR wrong type";

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    int deleted = 0;

    for (const auto& field : fields) {
        if (hash->erase(field) > 0) deleted++;
    }

    return ":" + std::to_string(deleted);
}

// -------------------- HGETALL --------------------
std::string hgetall(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::HASH) return "-ERR wrong type";

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    std::ostringstream out;
    out << "{";

    bool first = true;
    for (auto& [field, val] : *hash) {
        if (!first) out << ", ";
        out << field << ": " << val.getValue<std::string>();
        first = false;
    }
    out << "}";
    return out.str();
}

// -------------------- HEXISTS --------------------
std::string hexists(RedisHashMap& map, const std::string& key,
                    const std::string& field) {
    RedisObject* obj = map.get(key);
    if (!obj) return ":0";
    if (obj->getType() != RedisType::HASH) return "-ERR wrong type";

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    return hash->count(field) > 0 ? ":1" : ":0";
}

// -------------------- HLEN --------------------
std::string hlen(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);
    if (!obj) return ":0";
    if (obj->getType() != RedisType::HASH) return "-ERR wrong type";

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    return ":" + std::to_string(hash->size());
}

} // namespace hashmapstore
