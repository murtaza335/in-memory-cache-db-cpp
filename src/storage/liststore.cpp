#include "storage/liststore.hpp"
#include "storage/RedisObject.hpp"
#include "storage/RedisHashMap.hpp"
#include <vector>
#include <string>
#include <memory>
#include <mutex>

namespace liststore {

// ------------------ Base HashMap ------------------
static RedisHashMap redisMap(1024); // default size, can tune

// ------------------ Helper: get list ------------------
static std::vector<RedisObject>& getListRef(const std::string& key) {
    RedisObject* obj = redisMap.get(key);
    if (!obj || obj->getType() != RedisType::LIST) {
        // If key does not exist or not a list, initialize empty list
        redisMap.add(key, RedisObject(std::vector<RedisObject>{}));
        obj = redisMap.get(key);
    }
    return *static_cast<std::vector<RedisObject>*>(obj->getValue<void*>());
}

// ------------------ Operations ------------------

std::string lpush(const std::string& key, const std::string& value) {
    auto& list = getListRef(key);
    list.insert(list.begin(), RedisObject(value));
    return std::to_string(list.size());
}

std::string rpush(const std::string& key, const std::string& value) {
    auto& list = getListRef(key);
    list.push_back(RedisObject(value));
    return std::to_string(list.size());
}

std::string lpop(const std::string& key) {
    auto& list = getListRef(key);
    if (list.empty()) return "NULL";
    std::string val = list.front().getValue<std::string>();
    list.erase(list.begin());
    return val;
}

std::string rpop(const std::string& key) {
    auto& list = getListRef(key);
    if (list.empty()) return "NULL";
    std::string val = list.back().getValue<std::string>();
    list.pop_back();
    return val;
}

} // namespace liststore
