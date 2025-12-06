#ifndef STRINGSTORE_HPP
#define STRINGSTORE_HPP

#include <string>
#include "storage/RedisHashMap.hpp"
#include "storage/RedisObject.hpp"

namespace stringstore {

    std::string set(RedisHashMap& db, const std::string& key, const std::string& value);
    std::string get(RedisHashMap& db, const std::string& key);
    std::string del(RedisHashMap& db, const std::string& key);
    std::string exists(RedisHashMap& db, const std::string& key);
    std::string rename(RedisHashMap& db, const std::string& oldKey, const std::string& newKey);
    std::string copy(RedisHashMap& db, const std::string& sourceKey, const std::string& destKey);

}

#endif // STRINGSTORE_HPP
