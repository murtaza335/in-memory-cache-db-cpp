#pragma once
#include <string>
#include<vector>

class Parser {
public:
    // This is the ONLY function you call from your server
    std::string route(const std::string& rawInput);

private:
    // internal helpers
    std::string processCommand(const std::vector<std::string>& tokens);
    std::vector<std::string> tokenize(const std::string& input);
};
