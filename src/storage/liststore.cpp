#include "storage/liststore.hpp"
#include "storage/RedisObject.hpp"
#include <vector>
#include <string>

namespace liststore {

// ---------------------- Helper: fetch or create list ----------------------
static std::vector<RedisObject>& getList(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);

    // Create new list if missing or wrong type
    if (!obj || obj->getType() != RedisType::LIST) {

        // Create empty list
        std::vector<RedisObject> newList;
        map.add(key, RedisObject(newList));

        obj = map.get(key);
    }

    // Convert stored pointer to vector<RedisObject>&
    return obj->getValue<std::vector<RedisObject>>();
}

// ---------------------- Commands ----------------------

std::string lpush(RedisHashMap& map, const std::string& key, const std::string& value) {
    auto& list = getList(map, key);

    // insert new RedisObject element at front
    list.insert(list.begin(), RedisObject(value));

    return ":" + std::to_string(list.size());
}

std::string rpush(RedisHashMap& map, const std::string& key, const std::string& value) {
    auto& list = getList(map, key);

    // append new RedisObject element
    list.push_back(RedisObject(value));

    return ":" + std::to_string(list.size());
}

std::string lpop(RedisHashMap& map, const std::string& key) {
    auto& list = getList(map, key);

    if (list.empty())
        return "$-1";

    // first element as RedisObject
    RedisObject& obj = list.front();

    // extract as string
    if (obj.getType() != RedisType::STRING)
        return "-ERR wrong type (expected STRING element)";

    std::string val = obj.getValue<std::string>();

    list.erase(list.begin());
    return val;
}

std::string rpop(RedisHashMap& map, const std::string& key) {
    auto& list = getList(map, key);

    if (list.empty())
        return "$-1";

    // last element
    RedisObject& obj = list.back();

    if (obj.getType() != RedisType::STRING)
        return "-ERR wrong type (expected STRING element)";

    std::string val = obj.getValue<std::string>();

    list.pop_back();
    return val;
}

} // namespace liststore
