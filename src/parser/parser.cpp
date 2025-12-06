#include "parser/parser.hpp"
#include "storage/stringstore.hpp"
#include "storage/liststore.hpp"

#include <sstream>
#include <algorithm>

// Constructor (dependency injection)
Parser::Parser(RedisHashMap& map)
    : baseMap(map) {}

// --------------------------------------------------------
// TOKENIZER — splits input into tokens by spaces
// --------------------------------------------------------
std::vector<std::string> Parser::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream ss(input);
    std::string tok;

    while (ss >> tok) tokens.push_back(tok);
    return tokens;
}

// --------------------------------------------------------
// PUBLIC ENTRY POINT — called from your server
// --------------------------------------------------------
std::string Parser::route(const std::string& rawInput) {
    if (rawInput.empty())
        return "-ERR empty request";

    // Cleanup TCP newlines
    std::string clean = rawInput;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());

    auto tokens = tokenize(clean);

    if (tokens.empty())
        return "-ERR empty command";

    return processCommand(tokens);
}

// --------------------------------------------------------
// INTERNAL: Command router
// --------------------------------------------------------
std::string Parser::processCommand(const std::vector<std::string>& tokens) {
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    // ----------------------------------------------------
    // STRING COMMANDS
    // ----------------------------------------------------
    if (cmd == "SET") {
        if (tokens.size() < 3) return "-ERR SET requires key and value";
        return stringstore::set(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "GET") {
        if (tokens.size() < 2) return "-ERR GET requires key";
        return stringstore::get(baseMap, tokens[1]);
    }

    if (cmd == "DEL") {
        if (tokens.size() < 2) return "-ERR DEL requires key";
        return stringstore::del(baseMap, tokens[1]);
    }

    if (cmd == "EXISTS") {
        if (tokens.size() < 2) return "-ERR EXISTS requires key";
        return stringstore::exists(baseMap, tokens[1]);
    }

    if (cmd == "RENAME") {
        if (tokens.size() < 3) return "-ERR RENAME requires oldKey newKey";
        return stringstore::rename(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "COPY") {
        if (tokens.size() < 3) return "-ERR COPY requires sourceKey destKey";
        return stringstore::copy(baseMap, tokens[1], tokens[2]);
    }

    // ----------------------------------------------------
    // LIST COMMANDS
    // ----------------------------------------------------
    if (cmd == "LPUSH") {
        if (tokens.size() < 3) return "-ERR LPUSH requires key and value";
        return liststore::lpush(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "RPUSH") {
        if (tokens.size() < 3) return "-ERR RPUSH requires key and value";
        return liststore::rpush(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "LPOP") {
        if (tokens.size() < 2) return "-ERR LPOP requires key";
        return liststore::lpop(baseMap, tokens[1]);
    }

    if (cmd == "RPOP") {
        if (tokens.size() < 2) return "-ERR RPOP requires key";
        return liststore::rpop(baseMap, tokens[1]);
    }

    // ----------------------------------------------------
    // UNKNOWN COMMAND
    // ----------------------------------------------------
    return "-ERR unknown command";
}
