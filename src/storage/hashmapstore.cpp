// handles all hash type operations for our redis clone  
// so any time we want to store multiple fields under a single key we use this system  
// everything here is basically managing an unordered map inside a redisobject and acting like redis hash commands  
// this is where behaviour for hset hget hdel hgetall hexists hlen is defined and hooked into our main redis hashmap  

#include "storage/hashmapstore.hpp"
#include "storage/RedisObject.hpp"
#include <sstream>
#include <iostream>
#include <chrono>

namespace hashmapstore {
// timestamp utility function this gives us a readable string for logs so we can trace when operations happened
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return buffer;
}

// hset adds or updates a field in a hash if the key doesnt exist we create a whole new hash for it  
std::string hset(RedisHashMap& map, const std::string& key,
                 const std::string& field, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] HSET operation started - Key: " << key 
              << ", Field: " << field << std::endl;

    RedisObject* obj = map.get(key);
    std::unordered_map<std::string, RedisObject>* hash;

    if (!obj) {
        hash = new std::unordered_map<std::string, RedisObject>();
        hash->insert_or_assign(field, RedisObject(value));
        map.add(key, RedisObject(*hash));
        std::cout << "[" << getTimestamp() << "] [INFO] HSET - New hash created for key: " 
                  << key << ", Field added: " << field << std::endl;
        return ":1";
    }

    if (obj->getType() != RedisType::HASH) {
        std::cout << "[" << getTimestamp() << "] [ERROR] HSET - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    bool isNew = hash->find(field) == hash->end();
    hash->insert_or_assign(field, RedisObject(value));
    
    std::cout << "[" << getTimestamp() << "] [INFO] HSET - Key: " << key << ", Field: " << field 
              << " (" << (isNew ? "NEW" : "UPDATED") << "), Hash size: " << hash->size() << std::endl;

    return isNew ? ":1" : ":0";
}

// hget simply returns the value inside a hash for a specific field if key doesnt exist we return nil style like redis  
std::string hget(RedisHashMap& map, const std::string& key,
                 const std::string& field) {
    std::cout << "[" << getTimestamp() << "] [INFO] HGET operation - Key: " << key 
              << ", Field: " << field << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] HGET - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::HASH) {
        std::cout << "[" << getTimestamp() << "] [ERROR] HGET - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    auto it = hash->find(field);
    if (it == hash->end()) {
        std::cout << "[" << getTimestamp() << "] [WARN] HGET - Field not found: " << field 
                  << " in key: " << key << std::endl;
        return "$-1";
    }

    std::cout << "[" << getTimestamp() << "] [INFO] HGET - SUCCESS - Key: " << key 
              << ", Field: " << field << std::endl;
    return it->second.getValue<std::string>();
}

// hdel deletes one or more fields from a hash  
// returns number of fields removed similar to redis if key or field missing we just skip but count deleted
std::string hdel(RedisHashMap& map, const std::string& key,
                 const std::vector<std::string>& fields) {
    std::cout << "[" << getTimestamp() << "] [INFO] HDEL operation - Key: " << key 
              << ", Fields count: " << fields.size() << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] HDEL - Key not found: " << key << std::endl;
        return ":0";
    }
    if (obj->getType() != RedisType::HASH) {
        std::cout << "[" << getTimestamp() << "] [ERROR] HDEL - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    int deleted = 0;

    for (const auto& field : fields) {
        if (hash->erase(field) > 0) deleted++;
    }

    std::cout << "[" << getTimestamp() << "] [INFO] HDEL - Key: " << key << ", Deleted: " 
              << deleted << "/" << fields.size() << ", Remaining fields: " << hash->size() << std::endl;

    return ":" + std::to_string(deleted);
}

// hgetall prints all fields and values in a hash returns a formatted structure similar to json but not exactly redis format  
std::string hgetall(RedisHashMap& map, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] HGETALL operation - Key: " << key << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] HGETALL - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::HASH) {
        std::cout << "[" << getTimestamp() << "] [ERROR] HGETALL - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

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
    
    std::cout << "[" << getTimestamp() << "] [INFO] HGETALL - SUCCESS - Key: " << key 
              << ", Fields retrieved: " << hash->size() << std::endl;
    return out.str();
}

// hexists checks if a field exists inside the hash 
std::string hexists(RedisHashMap& map, const std::string& key,
                    const std::string& field) {
    std::cout << "[" << getTimestamp() << "] [INFO] HEXISTS operation - Key: " << key 
              << ", Field: " << field << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] HEXISTS - Key not found: " << key << std::endl;
        return ":0";
    }
    if (obj->getType() != RedisType::HASH) {
        std::cout << "[" << getTimestamp() << "] [ERROR] HEXISTS - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    bool exists = hash->count(field) > 0;
    
    std::cout << "[" << getTimestamp() << "] [INFO] HEXISTS - Key: " << key << ", Field: " 
              << field << ", Exists: " << (exists ? "YES" : "NO") << std::endl;
    return exists ? ":1" : ":0";
}

// hlen returns number of fields in the hash  
// basically size of the unordered map  
std::string hlen(RedisHashMap& map, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] HLEN operation - Key: " << key << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] HLEN - Key not found: " << key << std::endl;
        return ":0";
    }
    if (obj->getType() != RedisType::HASH) {
        std::cout << "[" << getTimestamp() << "] [ERROR] HLEN - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    auto* hash = static_cast<std::unordered_map<std::string, RedisObject>*>(obj->getPtr());
    size_t size = hash->size();
    
    std::cout << "[" << getTimestamp() << "] [INFO] HLEN - Key: " << key 
              << ", Hash size: " << size << std::endl;
    return ":" + std::to_string(size);
}

} 
