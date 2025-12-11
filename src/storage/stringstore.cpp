#include "storage/stringstore.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <chrono>

namespace stringstore {

// Utility function for timestamp
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return buffer;
}

// -------------------- SET --------------------
std::string set(RedisHashMap& db, const std::string& key, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] SET operation - Key: " << key 
              << ", Value length: " << value.size() << std::endl;
    
    RedisObject obj(value);
    db.add(key, obj);
    
    std::cout << "[" << getTimestamp() << "] [INFO] SET - SUCCESS - Key: " << key 
              << ", Value: " << value << std::endl;
    return "+OK";
}

// -------------------- SETNX --------------------
std::string setnx(RedisHashMap& db, const std::string& key, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] SETNX operation - Key: " << key 
              << ", Value length: " << value.size() << std::endl;
    
    if (db.exists(key)) {
        std::cout << "[" << getTimestamp() << "] [WARN] SETNX - Key already exists: " << key << std::endl;
        return ":0";
    }
    
    RedisObject obj(value);
    db.add(key, obj);
    
    std::cout << "[" << getTimestamp() << "] [INFO] SETNX - SUCCESS - Key: " << key 
              << ", Value: " << value << std::endl;
    return ":1";
}

// -------------------- MSET --------------------
std::string mset(RedisHashMap& db, const std::vector<std::string>& kvs) {
    std::cout << "[" << getTimestamp() << "] [INFO] MSET operation - Pairs count: " 
              << (kvs.size() / 2) << std::endl;
    
    if (kvs.size() % 2 != 0) {
        std::cout << "[" << getTimestamp() << "] [ERROR] MSET - Wrong number of arguments: " 
                  << kvs.size() << std::endl;
        return "-ERR wrong number of arguments for MSET";
    }

    for (size_t i = 0; i < kvs.size(); i += 2) {
        RedisObject obj(kvs[i + 1]);
        db.add(kvs[i], obj);
        std::cout << "[" << getTimestamp() << "] [DEBUG] MSET - Setting key: " << kvs[i] 
                  << ", Value length: " << kvs[i + 1].size() << std::endl;
    }
    
    std::cout << "[" << getTimestamp() << "] [INFO] MSET - SUCCESS - Total pairs set: " 
              << (kvs.size() / 2) << std::endl;
    return "+OK";
}

// -------------------- MGET --------------------
std::string mget(RedisHashMap& db, const std::vector<std::string>& keys) {
    std::cout << "[" << getTimestamp() << "] [INFO] MGET operation - Keys count: " 
              << keys.size() << std::endl;
    
    std::ostringstream out;
    bool first = true;
    int foundCount = 0, notFoundCount = 0, wrongTypeCount = 0;

    for (const auto& key : keys) {
        if (!first) out << " ";
        first = false;

        RedisObject* obj = db.get(key);
        if (!obj) {
            out << "$-1";
            notFoundCount++;
            continue;
        }
        if (obj->getType() != RedisType::STRING) {
            out << "-ERR wrong type";
            wrongTypeCount++;
            continue;
        }
        out << obj->getValue<std::string>();
        foundCount++;
    }
    
    std::cout << "[" << getTimestamp() << "] [INFO] MGET - SUCCESS - Found: " << foundCount 
              << ", Not found: " << notFoundCount << ", Wrong type: " << wrongTypeCount << std::endl;
    return out.str();
}

// -------------------- GET --------------------
std::string get(RedisHashMap& db, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] GET operation - Key: " << key << std::endl;
    
    RedisObject* obj = db.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] GET - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::STRING) {
        std::cout << "[" << getTimestamp() << "] [ERROR] GET - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }
    
    std::string value = obj->getValue<std::string>();
    std::cout << "[" << getTimestamp() << "] [INFO] GET - SUCCESS - Key: " << key 
              << ", Value: " << value << std::endl;
    return value;
}

// -------------------- DEL --------------------
std::string del(RedisHashMap& db, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] DEL operation - Key: " << key << std::endl;
    
    bool deleted = db.del(key);
    std::cout << "[" << getTimestamp() << "] [INFO] DEL - " << (deleted ? "SUCCESS" : "KEY_NOT_FOUND") 
              << " - Key: " << key << std::endl;
    return deleted ? ":1" : ":0";
}

// -------------------- EXISTS --------------------
std::string exists(RedisHashMap& db, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] EXISTS operation - Key: " << key << std::endl;
    
    bool found = db.exists(key);
    std::cout << "[" << getTimestamp() << "] [INFO] EXISTS - Key: " << key 
              << ", Exists: " << (found ? "YES" : "NO") << std::endl;
    return found ? ":1" : ":0";
}

// -------------------- APPEND --------------------
std::string append(RedisHashMap& db, const std::string& key, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] APPEND operation - Key: " << key 
              << ", Append length: " << value.size() << std::endl;
    
    RedisObject* obj = db.get(key);

    if (!obj) {
        // key doesn't exist -> set new string
        RedisObject newObj(value);
        db.add(key, newObj);
        std::cout << "[" << getTimestamp() << "] [INFO] APPEND - New key created: " << key 
                  << ", Final length: " << value.size() << std::endl;
        return ":" + std::to_string(value.size());
    }

    if (obj->getType() != RedisType::STRING) {
        std::cout << "[" << getTimestamp() << "] [ERROR] APPEND - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    // Modify in-place
    std::string* strPtr = static_cast<std::string*>(obj->getPtr());
    size_t oldLen = strPtr->size();
    strPtr->append(value);
    
    std::cout << "[" << getTimestamp() << "] [INFO] APPEND - SUCCESS - Key: " << key 
              << ", Old length: " << oldLen << ", New length: " << strPtr->size() << std::endl;

    return ":" + std::to_string(strPtr->size());
}

// -------------------- STRLEN --------------------
std::string strlen_(RedisHashMap& db, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] STRLEN operation - Key: " << key << std::endl;
    
    RedisObject* obj = db.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] STRLEN - Key not found: " << key << std::endl;
        return ":0";
    }
    if (obj->getType() != RedisType::STRING) {
        std::cout << "[" << getTimestamp() << "] [ERROR] STRLEN - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    size_t len = obj->getValue<std::string>().size();
    std::cout << "[" << getTimestamp() << "] [INFO] STRLEN - Key: " << key 
              << ", Length: " << len << std::endl;
    return ":" + std::to_string(len);
}

// ---------- Integer helpers ----------
static bool parseInt(const std::string& s, long long& out) {
    try {
        size_t idx = 0;
        out = std::stoll(s, &idx);
        return idx == s.size(); // ensure fully parsed
    } catch (...) {
        return false;
    }
}

// -------------------- INCRBY (core) --------------------
static std::string incrByInternal(RedisHashMap& db, const std::string& key, long long amount) {
    RedisObject* obj = db.get(key);
    long long current = 0;

    if (obj) {
        if (obj->getType() != RedisType::STRING) {
            std::cout << "[" << getTimestamp() << "] [ERROR] INCRBY - Wrong type for key: " << key << std::endl;
            return "-ERR wrong type";
        }

        std::string* valPtr = static_cast<std::string*>(obj->getPtr());
        if (!parseInt(*valPtr, current)) {
            std::cout << "[" << getTimestamp() << "] [ERROR] INCRBY - Non-integer value for key: " << key 
                      << ", Value: " << *valPtr << std::endl;
            return "-ERR value is not an integer or out of range";
        }

        current += amount;
        *valPtr = std::to_string(current); // modify in-place
    } else {
        // key doesn't exist, create new string
        RedisObject newObj(std::to_string(amount));
        db.add(key, newObj);
        current = amount;
    }

    std::cout << "[" << getTimestamp() << "] [INFO] INCRBY - Key: " << key 
              << ", Amount: " << amount << ", New value: " << current << std::endl;
    return ":" + std::to_string(current);
}

// -------------------- INCR --------------------
std::string incr(RedisHashMap& db, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] INCR operation - Key: " << key << std::endl;
    return incrByInternal(db, key, 1);
}

// -------------------- INCRBY --------------------
std::string incrby(RedisHashMap& db, const std::string& key, const std::string& amountStr) {
    std::cout << "[" << getTimestamp() << "] [INFO] INCRBY operation - Key: " << key 
              << ", Amount: " << amountStr << std::endl;
    
    long long amount;
    if (!parseInt(amountStr, amount)) {
        std::cout << "[" << getTimestamp() << "] [ERROR] INCRBY - Invalid amount: " << amountStr << std::endl;
        return "-ERR value is not an integer or out of range";
    }
    return incrByInternal(db, key, amount);
}

// -------------------- DECR --------------------
std::string decr(RedisHashMap& db, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] DECR operation - Key: " << key << std::endl;
    return incrByInternal(db, key, -1);
}

// -------------------- DECRBY --------------------
std::string decrby(RedisHashMap& db, const std::string& key, const std::string& amountStr) {
    std::cout << "[" << getTimestamp() << "] [INFO] DECRBY operation - Key: " << key 
              << ", Amount: " << amountStr << std::endl;
    
    long long amount;
    if (!parseInt(amountStr, amount)) {
        std::cout << "[" << getTimestamp() << "] [ERROR] DECRBY - Invalid amount: " << amountStr << std::endl;
        return "-ERR value is not an integer or out of range";
    }
    return incrByInternal(db, key, -amount);
}

// -------------------- RENAME --------------------
std::string rename(RedisHashMap& db, const std::string& oldKey, const std::string& newKey) {
    std::cout << "[" << getTimestamp() << "] [INFO] RENAME operation - Old key: " << oldKey 
              << ", New key: " << newKey << std::endl;
    
    bool success = db.rename(oldKey, newKey);
    std::cout << "[" << getTimestamp() << "] [INFO] RENAME - " << (success ? "SUCCESS" : "KEY_NOT_FOUND") 
              << " - Old: " << oldKey << ", New: " << newKey << std::endl;
    return success ? "+OK" : "-ERR key does not exist";
}

// -------------------- COPY --------------------
std::string copy(RedisHashMap& db, const std::string& sourceKey, const std::string& destKey) {
    std::cout << "[" << getTimestamp() << "] [INFO] COPY operation - Source: " << sourceKey 
              << ", Destination: " << destKey << std::endl;
    
    bool success = db.copy(sourceKey, destKey);
    std::cout << "[" << getTimestamp() << "] [INFO] COPY - " << (success ? "SUCCESS" : "SOURCE_NOT_FOUND") 
              << " - Source: " << sourceKey << ", Dest: " << destKey << std::endl;
    return success ? "+OK" : "-ERR source key does not exist";
}

// -------------------- EXPIRE (stub only) --------------------
std::string expire(RedisHashMap& db, const std::string& key, const std::string& seconds) {
    std::cout << "[" << getTimestamp() << "] [INFO] EXPIRE operation (stub) - Key: " << key 
              << ", Seconds: " << seconds << std::endl;
    // No implementation yet
    return "+OK (expire stub)";
}

} // namespace stringstore
