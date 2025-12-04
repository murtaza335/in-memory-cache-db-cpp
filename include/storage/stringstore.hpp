#ifndef STRINGSTORE_HPP
#define STRINGSTORE_HPP

#include <string>
#include "../RedisHashMap.hpp" // include your base hashmap
#include "../RedisObject.hpp"

namespace stringstore {
    std::string set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    std::string del(const std::string& key);
    std::string exists(const std::string& key);
    std::string rename(const std::string& oldKey, const std::string& newKey);
    std::string copy(const std::string& sourceKey, const std::string& destKey);
}

#endif // STRINGSTORE_HPP
