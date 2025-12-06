#ifndef LISTSTORE_HPP
#define LISTSTORE_HPP

#include <string>
#include "storage/RedisHashMap.hpp"
#include "storage/RedisObject.hpp"

namespace liststore {

    std::string lpush(RedisHashMap& map, const std::string& key, const std::string& value);
    std::string rpush(RedisHashMap& map, const std::string& key, const std::string& value);
    std::string lpop(RedisHashMap& map, const std::string& key);
    std::string rpop(RedisHashMap& map, const std::string& key);

}

#endif
