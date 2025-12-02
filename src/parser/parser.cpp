#include "Parser.hpp"
#include "StringStore.hpp"
#include "ListStore.hpp"
#include <sstream>
#include <vector>
#include <algorithm>

// Helper function to split string by spaces
static std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

std::string Parser::process(const std::string& input) {
    if (input.empty()) return "ERROR: Empty command";

    std::vector<std::string> tokens = split(input);
    if (tokens.empty()) return "ERROR: Empty command";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    // ----------------------------------------------------------------
    // Route to appropriate store based on command
    // ----------------------------------------------------------------

    // String commands
    if (cmd == "SET") {
        if (tokens.size() < 3) return "ERROR: SET requires key and value";
        return stringstore::set(tokens[1], tokens[2]);
    }
    if (cmd == "GET") {
        if (tokens.size() < 2) return "ERROR: GET requires key";
        return stringstore::get(tokens[1]);
    }
    if (cmd == "DEL") {
        if (tokens.size() < 2) return "ERROR: DEL requires key";
        return stringstore::del(tokens[1]);
    }

    // List commands
    if (cmd == "LPUSH") {
        if (tokens.size() < 3) return "ERROR: LPUSH requires key and value";
        return liststore::lpush(tokens[1], tokens[2]);
    }
    if (cmd == "RPUSH") {
        if (tokens.size() < 3) return "ERROR: RPUSH requires key and value";
        return liststore::rpush(tokens[1], tokens[2]);
    }
    if (cmd == "LPOP") {
        if (tokens.size() < 2) return "ERROR: LPOP requires key";
        return liststore::lpop(tokens[1]);
    }
    if (cmd == "RPOP") {
        if (tokens.size() < 2) return "ERROR: RPOP requires key";
        return liststore::rpop(tokens[1]);
    }

    return "ERROR: Unknown command";
}
