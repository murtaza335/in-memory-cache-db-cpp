#include "storage/RedisSets.hpp"
#include <sstream>
#include <algorithm>
#include <random>

namespace setstore {

    // ---------- Helper: get or create set ----------
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

    // ---------- SADD ----------
    std::string sadd(RedisHashMap& map, const std::string& key, const std::string& value) {
        auto* s = getOrCreateSet(map, key);
        if (!s) return "-ERR Key exists but is not a set";
        size_t inserted = s->emplace(RedisObject(value)).second ? 1 : 0;
        return std::to_string(inserted);
    }

    // ---------- SREM ----------
    std::string srem(RedisHashMap& map, const std::string& key, const std::string& value) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "0";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        size_t erased = s->erase(RedisObject(value));
        return std::to_string(erased);
    }

    // ---------- SMEMBERS ----------
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

    // ---------- SCARD ----------
    std::string scard(RedisHashMap& map, const std::string& key) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "0";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        return std::to_string(s->size());
    }

    // ---------- SPOP ----------
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

    // ---------- SISMEMBER ----------
    std::string sismember(RedisHashMap& map, const std::string& key, const std::string& value) {
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) return "0";
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        return s->count(RedisObject(value)) ? "1" : "0";
    }

    // ---------- SUNION ----------
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

    // ---------- SINTER ----------
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

    // ---------- SDIFF ----------
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