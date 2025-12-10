#include "parser/parser.hpp"
#include "storage/stringstore.hpp"
#include "storage/liststore.hpp"
#include "storage/RedisSets.hpp"  

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

    // ---------------- STRING COMMANDS ----------------
    if (cmd == "SET") {
        if (tokens.size() < 3) return "-ERR SET requires key value";
        return stringstore::set(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "SETNX") {
    if (tokens.size() < 3) return "-ERR SETNX requires key value";
    return stringstore::setnx(baseMap, tokens[1], tokens[2]);
}

    if (cmd == "MSET") {
        if (tokens.size() < 3 || tokens.size() % 2 != 1)
            return "-ERR MSET requires key1 val1 [key2 val2 ...]";
        std::vector<std::string> kvPairs(tokens.begin() + 1, tokens.end());
        return stringstore::mset(baseMap, kvPairs);
    }

    if (cmd == "MGET") {
        if (tokens.size() < 2) return "-ERR MGET requires at least one key";
        std::vector<std::string> keys(tokens.begin() + 1, tokens.end());
        return stringstore::mget(baseMap, keys);
    }

    if (cmd == "GET") {
        if (tokens.size() < 2) return "-ERR GET requires key";
        return stringstore::get(baseMap, tokens[1]);
    }

    if (cmd == "APPEND") {
        if (tokens.size() < 3) return "-ERR APPEND requires key value";
        return stringstore::append(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "STRLEN") {
        if (tokens.size() < 2) return "-ERR STRLEN requires key";
        return stringstore::strlen_(baseMap, tokens[1]);
    }

    if (cmd == "INCR") {
        if (tokens.size() < 2) return "-ERR INCR requires key";
        return stringstore::incr(baseMap, tokens[1]);
    }

    if (cmd == "INCRBY") {
        if (tokens.size() < 3) return "-ERR INCRBY requires key amount";
        return stringstore::incrby(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "DECR") {
        if (tokens.size() < 2) return "-ERR DECR requires key";
        return stringstore::decr(baseMap, tokens[1]);
    }

    if (cmd == "DECRBY") {
        if (tokens.size() < 3) return "-ERR DECRBY requires key amount";
        return stringstore::decrby(baseMap, tokens[1], tokens[2]);
    }


    // if (cmd == "EXPIRE") {
    //     if (tokens.size() < 3) return "-ERR EXPIRE requires key seconds";
    //     return stringstore::expire(baseMap, tokens[1], std::stoi(tokens[2]));
    // }

    if (cmd == "DEL") {
        if (tokens.size() < 2) return "-ERR DEL requires key";
        return stringstore::del(baseMap, tokens[1]);
    }

    // ---------------- LIST COMMANDS ----------------
    if (cmd == "LPUSH") {
        if (tokens.size() < 3) return "-ERR LPUSH requires list value";
        return liststore::lpush(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "RPUSH") {
        if (tokens.size() < 3) return "-ERR RPUSH requires list value";
        return liststore::rpush(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "LPOP") {
        if (tokens.size() < 2) return "-ERR LPOP requires list";
        return liststore::lpop(baseMap, tokens[1]);
    }

    if (cmd == "RPOP") {
        if (tokens.size() < 2) return "-ERR RPOP requires list";
        return liststore::rpop(baseMap, tokens[1]);
    }

    if (cmd == "LLEN") {
    if (tokens.size() < 2) return "-ERR LLEN requires list";
    return liststore::llen(baseMap, tokens[1]);
    }

    if (cmd == "LINDEX") {
        if (tokens.size() < 3) return "-ERR LINDEX requires list and index";
        return liststore::lindex(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "LSET") {
        if (tokens.size() < 4) return "-ERR LSET requires list, index, and value";
        return liststore::lset(baseMap, tokens[1], tokens[2], tokens[3]);
    }

    if (cmd == "LSORT") {
    if (tokens.size() < 3) return "-ERR LSORT requires list and order";
    return liststore::lsort(baseMap, tokens[1], tokens[2]);
    }   

    if (cmd == "LPRINT") {
    if (tokens.size() < 2) return "-ERR LPRINT requires list";
    return liststore::lprint(baseMap, tokens[1]);
    }



    // ---------------- SET COMMANDS ----------------
    if (cmd == "SADD") {
        if (tokens.size() < 3) return "-ERR SADD requires set value";
        return setstore::sadd(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "SREM") {
        if (tokens.size() < 3) return "-ERR SREM requires set value";
        return setstore::srem(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "SMEMBERS") {
        if (tokens.size() < 2) return "-ERR SMEMBERS requires set";
        return setstore::smembers(baseMap, tokens[1]);
    }

    if (cmd == "SCARD") {
        if (tokens.size() < 2) return "-ERR SCARD requires set";
        return setstore::scard(baseMap, tokens[1]);
    }

    if (cmd == "SPOP") {
        if (tokens.size() < 2) return "-ERR SPOP requires set";
        return setstore::spop(baseMap, tokens[1]);
    }

    if (cmd == "SISMEMBER") {
        if (tokens.size() < 3) return "-ERR SISMEMBER requires set value";
        return setstore::sismember(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "SUNION") {
        if (tokens.size() < 3) return "-ERR SUNION requires two sets";
        return setstore::sunion(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "SINTER") {
        if (tokens.size() < 3) return "-ERR SINTER requires two sets";
        return setstore::sinter(baseMap, tokens[1], tokens[2]);
    }

    if (cmd == "SDIFF") {
        if (tokens.size() < 3) return "-ERR SDIFF requires two sets";
        return setstore::sdiff(baseMap, tokens[1], tokens[2]);
    }


    // ---------------- UNKNOWN ----------------
    return "-ERR unknown command";
}
