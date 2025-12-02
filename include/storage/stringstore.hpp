#ifndef STRINGSTORE_HPP
#define STRINGSTORE_HPP

#include <string>

namespace stringstore {
    std::string set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    std::string del(const std::string& key);
}

#endif
