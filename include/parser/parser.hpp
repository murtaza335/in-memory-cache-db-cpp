#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "storage/RedisHashMap.hpp"

class Parser {
private:
    RedisHashMap& baseMap;  // reference to shared map

public:
    Parser(RedisHashMap& map);  // constructor injection
    // This is the ONLY function you call from your server
    std::string route(const std::string& rawInput);

    // internal helpers
    std::string processCommand(const std::vector<std::string>& tokens);
    std::vector<std::string> tokenize(const std::string& input);

    // Command registry types
    using HandlerFn = std::function<std::string(RedisHashMap&, const std::vector<std::string>&)>;

    struct CommandSpec {
        HandlerFn handler;
        int minArgs;   // minimum token count (including command name)
        int maxArgs;   // maximum token count; -1 == unbounded
        std::string help; // (optional) short help text
    };

    // Exposed for unit tests if needed
    static const std::unordered_map<std::string, CommandSpec>& getCommandTable();
};