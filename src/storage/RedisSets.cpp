#include "storage/RedisSets.hpp"
#include <sstream>
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>

namespace setstore {

    // Utility function for timestamp
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
        return buffer;
    }

    // ---------- Helper: get or create set ----------
    std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>* getOrCreateSet(RedisHashMap& map, const std::string& key) {
        RedisObject* obj = map.get(key);
        if (!obj) {
            std::cout << "[" << getTimestamp() << "] [DEBUG] Creating new set for key: " << key << std::endl;
            std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual> s;
            map.add(key, RedisObject(s));
            obj = map.get(key);
        }

        if (obj->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [ERROR] Key exists but is not a set - Key: " << key << std::endl;
            return nullptr;
        }
        return static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
    }

    // ---------- SADD ----------
    std::string sadd(RedisHashMap& map, const std::string& key, const std::string& value) {
        std::cout << "[" << getTimestamp() << "] [INFO] SADD operation - Key: " << key 
                  << ", Value: " << value << std::endl;
        
        auto* s = getOrCreateSet(map, key);
        if (!s) {
            std::cout << "[" << getTimestamp() << "] [ERROR] SADD failed - Key is not a set: " << key << std::endl;
            return "-ERR Key exists but is not a set";
        }
        
        size_t inserted = s->emplace(RedisObject(value)).second ? 1 : 0;
        std::cout << "[" << getTimestamp() << "] [INFO] SADD - Key: " << key << ", Value: " << value 
                  << ", " << (inserted ? "INSERTED" : "ALREADY_EXISTS") << ", Set size: " << s->size() << std::endl;
        return std::to_string(inserted);
    }

    // ---------- SREM ----------
    std::string srem(RedisHashMap& map, const std::string& key, const std::string& value) {
        std::cout << "[" << getTimestamp() << "] [INFO] SREM operation - Key: " << key 
                  << ", Value: " << value << std::endl;
        
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [WARN] SREM - Set not found or wrong type - Key: " << key << std::endl;
            return "0";
        }
        
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        size_t erased = s->erase(RedisObject(value));
        
        std::cout << "[" << getTimestamp() << "] [INFO] SREM - Key: " << key << ", Value: " << value 
                  << ", " << (erased ? "REMOVED" : "NOT_FOUND") << ", Remaining size: " << s->size() << std::endl;
        return std::to_string(erased);
    }

    // ---------- SMEMBERS ----------
    std::string smembers(RedisHashMap& map, const std::string& key) {
        std::cout << "[" << getTimestamp() << "] [INFO] SMEMBERS operation - Key: " << key << std::endl;
        
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [ERROR] SMEMBERS - Set not found - Key: " << key << std::endl;
            return "-ERR no such set";
        }
        
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        std::stringstream ss;
        for (const auto& item : *s) {
            ss << item.getValue<std::string>() << " ";
        }
        std::string res = ss.str();
        if (!res.empty()) res.pop_back(); // remove trailing space
        
        std::cout << "[" << getTimestamp() << "] [INFO] SMEMBERS - SUCCESS - Key: " << key 
                  << ", Members count: " << s->size() << std::endl;
        return res;
    }

    // ---------- SCARD ----------
    std::string scard(RedisHashMap& map, const std::string& key) {
        std::cout << "[" << getTimestamp() << "] [INFO] SCARD operation - Key: " << key << std::endl;
        
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [WARN] SCARD - Set not found - Key: " << key << std::endl;
            return "0";
        }
        
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        std::cout << "[" << getTimestamp() << "] [INFO] SCARD - Key: " << key 
                  << ", Set size: " << s->size() << std::endl;
        return std::to_string(s->size());
    }

    // ---------- SPOP ----------
    std::string spop(RedisHashMap& map, const std::string& key) {
        std::cout << "[" << getTimestamp() << "] [INFO] SPOP operation - Key: " << key << std::endl;
        
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [ERROR] SPOP - Set not found - Key: " << key << std::endl;
            return "-ERR no such set";
        }
        
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        if (s->empty()) {
            std::cout << "[" << getTimestamp() << "] [WARN] SPOP - Set is empty - Key: " << key << std::endl;
            return "-ERR set empty";
        }

        auto it = s->begin();
        std::advance(it, rand() % s->size());
        std::string val = it->getValue<std::string>();
        s->erase(it);
        
        std::cout << "[" << getTimestamp() << "] [INFO] SPOP - SUCCESS - Key: " << key 
                  << ", Popped value: " << val << ", Remaining size: " << s->size() << std::endl;
        return val;
    }

    // ---------- SISMEMBER ----------
    std::string sismember(RedisHashMap& map, const std::string& key, const std::string& value) {
        std::cout << "[" << getTimestamp() << "] [INFO] SISMEMBER operation - Key: " << key 
                  << ", Value: " << value << std::endl;
        
        RedisObject* obj = map.get(key);
        if (!obj || obj->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [WARN] SISMEMBER - Set not found - Key: " << key << std::endl;
            return "0";
        }
        
        auto* s = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj->getPtr());
        bool isMember = s->count(RedisObject(value)) > 0;
        
        std::cout << "[" << getTimestamp() << "] [INFO] SISMEMBER - Key: " << key << ", Value: " << value 
                  << ", Is member: " << (isMember ? "YES" : "NO") << std::endl;
        return isMember ? "1" : "0";
    }

    // ---------- SUNION ----------
    std::string sunion(RedisHashMap& map, const std::string& key1, const std::string& key2) {
        std::cout << "[" << getTimestamp() << "] [INFO] SUNION operation - Key1: " << key1 
                  << ", Key2: " << key2 << std::endl;
        
        RedisObject* obj1 = map.get(key1);
        RedisObject* obj2 = map.get(key2);
        std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual> result;
        
        size_t set1Size = 0, set2Size = 0;

        if (obj1 && obj1->getType() == RedisType::SET) {
            auto* s1 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj1->getPtr());
            set1Size = s1->size();
            result.insert(s1->begin(), s1->end());
        }

        if (obj2 && obj2->getType() == RedisType::SET) {
            auto* s2 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj2->getPtr());
            set2Size = s2->size();
            result.insert(s2->begin(), s2->end());
        }

        std::stringstream ss;
        for (const auto& item : result) ss << item.getValue<std::string>() << " ";
        std::string res = ss.str();
        if (!res.empty()) res.pop_back();
        
        std::cout << "[" << getTimestamp() << "] [INFO] SUNION - SUCCESS - Key1: " << key1 << " (size: " << set1Size 
                  << "), Key2: " << key2 << " (size: " << set2Size << "), Union size: " << result.size() << std::endl;
        return res;
    }

    // ---------- SINTER ----------
    std::string sinter(RedisHashMap& map, const std::string& key1, const std::string& key2) {
        std::cout << "[" << getTimestamp() << "] [INFO] SINTER operation - Key1: " << key1 
                  << ", Key2: " << key2 << std::endl;
        
        RedisObject* obj1 = map.get(key1);
        RedisObject* obj2 = map.get(key2);
        
        if (!obj1 || obj1->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [ERROR] SINTER - First set not found - Key: " << key1 << std::endl;
            return "";
        }
        if (!obj2 || obj2->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [ERROR] SINTER - Second set not found - Key: " << key2 << std::endl;
            return "";
        }

        auto* s1 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj1->getPtr());
        auto* s2 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj2->getPtr());

        std::stringstream ss;
        size_t intersectionSize = 0;
        for (const auto& item : *s1) {
            if (s2->count(item)) {
                ss << item.getValue<std::string>() << " ";
                intersectionSize++;
            }
        }
        std::string res = ss.str();
        if (!res.empty()) res.pop_back();
        
        std::cout << "[" << getTimestamp() << "] [INFO] SINTER - SUCCESS - Key1: " << key1 << " (size: " << s1->size() 
                  << "), Key2: " << key2 << " (size: " << s2->size() << "), Intersection size: " << intersectionSize << std::endl;
        return res;
    }

    // ---------- SDIFF ----------
    std::string sdiff(RedisHashMap& map, const std::string& key1, const std::string& key2) {
        std::cout << "[" << getTimestamp() << "] [INFO] SDIFF operation - Key1: " << key1 
                  << ", Key2: " << key2 << std::endl;
        
        RedisObject* obj1 = map.get(key1);
        RedisObject* obj2 = map.get(key2);
        
        if (!obj1 || obj1->getType() != RedisType::SET) {
            std::cout << "[" << getTimestamp() << "] [ERROR] SDIFF - First set not found - Key: " << key1 << std::endl;
            return "";
        }
        
        auto* s1 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj1->getPtr());
        std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual> result = *s1;
        
        size_t set2Size = 0;
        if (obj2 && obj2->getType() == RedisType::SET) {
            auto* s2 = static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(obj2->getPtr());
            set2Size = s2->size();
            for (const auto& item : *s2) result.erase(item);
        }

        std::stringstream ss;
        for (const auto& item : result) ss << item.getValue<std::string>() << " ";
        std::string res = ss.str();
        if (!res.empty()) res.pop_back();
        
        std::cout << "[" << getTimestamp() << "] [INFO] SDIFF - SUCCESS - Key1: " << key1 << " (size: " << s1->size() 
                  << "), Key2: " << key2 << " (size: " << set2Size << "), Difference size: " << result.size() << std::endl;
        return res;
    }

}