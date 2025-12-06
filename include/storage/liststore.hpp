#ifndef LISTSTORE_HPP
#define LISTSTORE_HPP

#include <string>
#include <stdexcept>
#include "storage/RedisHashMap.hpp"
#include "storage/RedisObject.hpp"
#include "LinkedList.hpp"   // include your custom linked list

namespace liststore {

    // Push value to the left (head)
    std::string lpush(RedisHashMap& map, const std::string& key, const std::string& value);

    // Push value to the right (tail)
    std::string rpush(RedisHashMap& map, const std::string& key, const std::string& value);

    // Pop value from the left (head)
    std::string lpop(RedisHashMap& map, const std::string& key);

    // Pop value from the right (tail)
    std::string rpop(RedisHashMap& map, const std::string& key);

    // Return list length
    std::string llen(RedisHashMap& map, const std::string& key);

    // Return element at index
    std::string lindex(RedisHashMap& map, const std::string& key, const std::string& indexStr);

    // Set element at index
    std::string lset(RedisHashMap& map, const std::string& key, const std::string& indexStr, const std::string& value);

}

#endif
