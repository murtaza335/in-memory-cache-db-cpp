#include "storage/stringstore.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace stringstore {

// -------------------- SET --------------------
std::string set(RedisHashMap& db, const std::string& key, const std::string& value) {
    RedisObject obj(value);
    db.add(key, obj);
    return "+OK";
}

// -------------------- SETNX --------------------
std::string setnx(RedisHashMap& db, const std::string& key, const std::string& value) {
    if (db.exists(key)) return ":0";
    RedisObject obj(value);
    db.add(key, obj);
    return ":1";
}

// -------------------- MSET --------------------
std::string mset(RedisHashMap& db, const std::vector<std::string>& kvs) {
    if (kvs.size() % 2 != 0) return "-ERR wrong number of arguments for MSET";

    for (size_t i = 0; i < kvs.size(); i += 2) {
        RedisObject obj(kvs[i + 1]);
        db.add(kvs[i], obj);
    }
    return "+OK";
}

// -------------------- MGET --------------------
std::string mget(RedisHashMap& db, const std::vector<std::string>& keys) {
    std::ostringstream out;
    bool first = true;

    for (const auto& key : keys) {
        if (!first) out << " ";
        first = false;

        RedisObject* obj = db.get(key);
        if (!obj) {
            out << "$-1"; 
            continue;
        }
        if (obj->getType() != RedisType::STRING) {
            out << "-ERR wrong type";
            continue;
        }
        out << obj->getValue<std::string>();
    }
    return out.str();
}

// -------------------- GET --------------------
std::string get(RedisHashMap& db, const std::string& key) {
    RedisObject* obj = db.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::STRING) return "-ERR wrong type";
    return obj->getValue<std::string>();
}

// -------------------- DEL --------------------
std::string del(RedisHashMap& db, const std::string& key) {
    return db.del(key) ? ":1" : ":0";
}

// -------------------- EXISTS --------------------
std::string exists(RedisHashMap& db, const std::string& key) {
    return db.exists(key) ? ":1" : ":0";
}

// -------------------- APPEND --------------------
std::string append(RedisHashMap& db, const std::string& key, const std::string& value) {
    RedisObject* obj = db.get(key);

    if (!obj) {
        // key doesn't exist -> set new string
        RedisObject newObj(value);
        db.add(key, newObj);
        return ":" + std::to_string(value.size());
    }

    if (obj->getType() != RedisType::STRING) return "-ERR wrong type";

    // Modify in-place
    std::string* strPtr = static_cast<std::string*>(obj->getPtr());
    strPtr->append(value);

    return ":" + std::to_string(strPtr->size());
}

// -------------------- STRLEN --------------------
std::string strlen_(RedisHashMap& db, const std::string& key) {
    RedisObject* obj = db.get(key);
    if (!obj) return ":0";
    if (obj->getType() != RedisType::STRING) return "-ERR wrong type";

    return ":" + std::to_string(obj->getValue<std::string>().size());
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
        if (obj->getType() != RedisType::STRING) return "-ERR wrong type";

        std::string* valPtr = static_cast<std::string*>(obj->getPtr());
        if (!parseInt(*valPtr, current))
            return "-ERR value is not an integer or out of range";

        current += amount;
        *valPtr = std::to_string(current); // modify in-place
    } else {
        // key doesn't exist, create new string
        RedisObject newObj(std::to_string(amount));
        db.add(key, newObj);
        current = amount;
    }

    return ":" + std::to_string(current);
}

// -------------------- INCR --------------------
std::string incr(RedisHashMap& db, const std::string& key) {
    return incrByInternal(db, key, 1);
}

// -------------------- INCRBY --------------------
std::string incrby(RedisHashMap& db, const std::string& key, const std::string& amountStr) {
    long long amount;
    if (!parseInt(amountStr, amount)) return "-ERR value is not an integer or out of range";
    return incrByInternal(db, key, amount);
}

// -------------------- DECR --------------------
std::string decr(RedisHashMap& db, const std::string& key) {
    return incrByInternal(db, key, -1);
}

// -------------------- DECRBY --------------------
std::string decrby(RedisHashMap& db, const std::string& key, const std::string& amountStr) {
    long long amount;
    if (!parseInt(amountStr, amount)) return "-ERR value is not an integer or out of range";
    return incrByInternal(db, key, -amount);
}

// -------------------- RENAME --------------------
std::string rename(RedisHashMap& db, const std::string& oldKey, const std::string& newKey) {
    return db.rename(oldKey, newKey) ? "+OK" : "-ERR key does not exist";
}

// -------------------- COPY --------------------
std::string copy(RedisHashMap& db, const std::string& sourceKey, const std::string& destKey) {
    return db.copy(sourceKey, destKey) ? "+OK" : "-ERR source key does not exist";
}

// -------------------- EXPIRE (stub only) --------------------
std::string expire(RedisHashMap& db, const std::string& key, const std::string& seconds) {
    // No implementation yet
    return "+OK (expire stub)";
}

} // namespace stringstore
