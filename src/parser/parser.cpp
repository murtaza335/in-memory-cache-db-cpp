#include "Parser.hpp"
#include "StringStore.hpp"
#include "ListStore.hpp"
#include <sstream>
#include <algorithm>
#include <vector>

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
// PUBLIC ENTRY POINT — call THIS from your server
// --------------------------------------------------------
std::string Parser::route(const std::string& rawInput) {
    if (rawInput.empty())
        return "ERROR: Empty request";

    // Remove \r or \n at end (from TCP)
    std::string clean = rawInput;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());

    auto tokens = tokenize(clean);

    if (tokens.empty())
        return "ERROR: Empty command";

    return processCommand(tokens);
}

// --------------------------------------------------------
// INTERNAL: process parsed tokens and route to correct store
// --------------------------------------------------------
std::string Parser::processCommand(const std::vector<std::string>& tokens) {
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    // -----------------------------------
    // STRING STORE COMMANDS
    // -----------------------------------
    if (cmd == "SET") {
        if (tokens.size() < 3)
            return "ERROR: SET requires key and value";

        return stringstore::set(tokens[1], tokens[2]);
    }

    if (cmd == "GET") {
        if (tokens.size() < 2)
            return "ERROR: GET requires key";

        return stringstore::get(tokens[1]);
    }

    if (cmd == "DEL") {
        if (tokens.size() < 2)
            return "ERROR: DEL requires key";

        return stringstore::del(tokens[1]);
    }

    // -----------------------------------
    // LIST STORE COMMANDS
    // -----------------------------------
    if (cmd == "LPUSH") {
        if (tokens.size() < 3)
            return "ERROR: LPUSH requires key and value";

        return liststore::lpush(tokens[1], tokens[2]);
    }

    if (cmd == "RPUSH") {
        if (tokens.size() < 3)
            return "ERROR: RPUSH requires key and value";

        return liststore::rpush(tokens[1], tokens[2]);
    }

    if (cmd == "LPOP") {
        if (tokens.size() < 2)
            return "ERROR: LPOP requires key";

        return liststore::lpop(tokens[1]);
    }

    if (cmd == "RPOP") {
        if (tokens.size() < 2)
            return "ERROR: RPOP requires key";

        return liststore::rpop(tokens[1]);
    }

    // -----------------------------------
    // UNKNOWN COMMAND
    // -----------------------------------
    return "ERROR: Unknown command";
}
