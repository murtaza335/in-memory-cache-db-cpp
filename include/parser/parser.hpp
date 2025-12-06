#pragma once
#include <string>
#include<vector>
#include "storage/RedisHashMap.hpp"

class Parser {
private:
    RedisHashMap& baseMap;  // reference to shared map

public:
    Parser(RedisHashMap& map);  // constructor injection
    // This is the ONLY function you call from your server
    std::string route(const std::string& rawInput);

private:
    // internal helpers
    std::string processCommand(const std::vector<std::string>& tokens);
    std::vector<std::string> tokenize(const std::string& input);
};
