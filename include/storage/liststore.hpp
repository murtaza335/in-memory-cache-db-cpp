#ifndef LISTSTORE_HPP
#define LISTSTORE_HPP

#include <string>

namespace liststore {
    std::string lpush(const std::string& key, const std::string& value);
    std::string rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
}

#endif
