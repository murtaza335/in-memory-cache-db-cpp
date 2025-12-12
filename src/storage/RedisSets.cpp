// this file basically handles all operations for redis like sets in our custom storage  
// every set is stored inside our main redis hashmap as a redisobject that internally holds an unordered set  
// all these commands mimic the actual redis behaviour but simplified for our own db  

#include "storage/RedisSets.hpp"
#include <sstream>
#include <algorithm>
#include <random>

namespace setstore {

    // this helper either fetches the set if it already exists or creates a new empty one
    // also if the key exists but is not a set we return nullptr so caller can send error
    std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>* getOrCreateSet(RedisHashMap& map, const std::string& key) {
        RedisObject* obj = map.get(key);
        if (!obj) {
            std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual> s;
            map.add(key, RedisObject(s));
            obj = map.get(key);
        }

        if (obj->getType() != RedisType::SET) return nullptr;
        return static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
    }

    // sadd means insert a value into the set stored under key if key doesnt exist we create the set and then add the value
    std::string sadd(RedisHashMap& map, const std::string& key, const std::string& value) {
        auto* s = getOrCreateSet(map, key);
        if (!s) return "-ERR Key exists but is not a set";
        size_t inserted = s->emplace(RedisObject(value)).second ? 1 : 0;
        return std::to_string(inserted);
    }

    // srem removes a value from a set if set exists and has the value it removes it and returns 1 otherwise 0
    std::string srem(RedisHashMap& map, const std::string& key, const std::string& value) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "0";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        size_t erased = s->erase(RedisObject(value));
        return std::to_string(erased);
    }

    // smembers just dumps all members of the set in a single space separated string if key doesnt exist or isnt a set returns an error
    std::string smembers(RedisHashMap& map, const std::string& key) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "-ERR no such set";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        std::stringstream ss;
        for (const auto& item : *s) {
            ss << item.getValue<std::string>() << " ";
        }
        std::string res = ss.str();
        if (!res.empty()) res.pop_back(); // remove trailing space
        return res;
    }

    // scard returns the count of elements inside the set
    std::string scard(RedisHashMap& map, const std::string& key) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "0";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        return std::to_string(s->size());
    }

    // spop randomly picks and removes one element from the set
    // random delete like redis spop
    std::string spop(RedisHashMap& map, const std::string& key) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "-ERR no such set";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        if (s->empty()) return "-ERR set empty";

        auto it = s->begin();
        std::advance(it, rand() % s->size());
        std::string val = it->getValue<std::string>();
        s->erase(it);
        return val;
    }

    // sismember checks if a value is present inside the set returns 1 or 0
    std::string sismember(RedisHashMap& map, const std::string& key, const std::string& value) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "0";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        return s->count(RedisObject(value)) ? "1" : "0";
    }

    // sunion combines members of two sets removes duplicates because set
    // returns all unique values from set1 and set2
    std::string sunion(RedisHashMap& map, const std::string& key1, const std::string& key2) {
        RedisObject* obj1 = map.get(key1);
        RedisObject* obj2 = map.get(key2);
        std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual> result;

        if (obj1 && obj1->getType() == RedisType::SET) {
            auto* s1 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj1->getPtr());
            result.insert(s1->begin(), s1->end());
        }

        if (obj2 && obj2->getType() == RedisType::SET) {
            auto* s2 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj2->getPtr());
            result.insert(s2->begin(), s2->end());
        }

        std::stringstream ss;
        for (const auto& item : result) ss << item.getValue<std::string>() << " ";
        std::string res = ss.str();
        if (!res.empty()) res.pop_back();
        return res;
    }

    // sinter finds common elements between two sets
    // if either key doesnt have a valid set returns empty result
    std::string sinter(RedisHashMap& map, const std::string& key1, const std::string& key2) {
        RedisObject* obj1 = map.get(key1);
        RedisObject* obj2 = map.get(key2);
        if (!obj1 || obj1->getType() != RedisType::SET) return "";
        if (!obj2 || obj2->getType() != RedisType::SET) return "";

        auto* s1 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj1->getPtr());
        auto* s2 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj2->getPtr());

        std::stringstream ss;
        for (const auto& item : *s1) {
            if (s2->count(item)) ss << item.getValue<std::string>() << " ";
        }
        std::string res = ss.str();
        if (!res.empty()) res.pop_back();
        return res;
    }

    // sdiff does set difference meaning everything in set1 minus anything found in set2
    // basically elements unique to first set
    std::string sdiff(RedisHashMap& map, const std::string& key1, const std::string& key2) {
        RedisObject* obj1 = map.get(key1);
        RedisObject* obj2 = map.get(key2);
        if (!obj1 || obj1->getType() != RedisType::SET) return "";
        auto* s1 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj1->getPtr());
        std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual> result = *s1;

        if (obj2 && obj2->getType() == RedisType::SET) {
            auto* s2 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj2->getPtr());
            for (const auto& item : *s2) result.erase(item);
        }

        std::stringstream ss;
        for (const auto& item : result) ss << item.getValue<std::string>() << " ";
        std::string res = ss.str();
        if (!res.empty()) res.pop_back();
        return res;
    }

}
