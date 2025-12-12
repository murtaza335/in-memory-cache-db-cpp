#include "parser/parser.hpp"

#include "storage/stringstore.hpp"
#include "storage/liststore.hpp"
#include "storage/RedisSets.hpp"
#include "storage/hashmapstore.hpp"
#include "storage/RedisObject.hpp"
#include "storage/TTLPriorityQueue.hpp"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <stdexcept>

// constructor 
Parser::Parser(RedisHashMap& map)
    : baseMap(map) {}


// tokenizer splits input into tokens by spaces
std::vector<std::string> Parser::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream ss(input);
    std::string tok;

    while (ss >> tok) tokens.push_back(tok);
    return tokens;
}

// public entry point called from your server
std::string Parser::route(const std::string& rawInput) {
    if (rawInput.empty())
        return std::string("-ERR empty request");

    // cleanup tcp newlines
    std::string clean = rawInput;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());

    auto tokens = tokenize(clean);

    if (tokens.empty())
        return std::string("-ERR empty command");

    return processCommand(tokens);
}

// utility to change uppercase a string
static std::string uppercpy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}


// command table 
static const std::unordered_map<std::string, Parser::CommandSpec>& buildCommandTable() {
    // construct once in a functiolocal static toavoid static initialization order issues
    static const std::unordered_map<std::string, Parser::CommandSpec> table = {
        // string commands
        { "SET",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR SET requires key value");
                        return stringstore::set(m, t[1], t[2]);
                    }, 3, 3, "SET key value" } },

        { "SETNX", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR SETNX requires key value");
                        return stringstore::setnx(m, t[1], t[2]);
                    }, 3, 3, "SETNX key value" } },

        { "MSET",  { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR MSET requires key1 val1 [key2 val2 ...]");
                        if ((t.size() - 1) % 2 != 0) return std::string("-ERR MSET requires key value pairs");
                        std::vector<std::string> kvPairs(t.begin() + 1, t.end());
                        return stringstore::mset(m, kvPairs);
                    }, 3, -1, "MSET key value [key value ...]" } },

        { "MGET",  { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR MGET requires at least one key");
                        std::vector<std::string> keys(t.begin() + 1, t.end());
                        return stringstore::mget(m, keys);
                    }, 2, -1, "MGET key [key ...]" } },

        { "GET",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR GET requires key");
                        return stringstore::get(m, t[1]);
                    }, 2, 2, "GET key" } },

        { "APPEND",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR APPEND requires key value");
                        return stringstore::append(m, t[1], t[2]);
                    }, 3, 3, "APPEND key value" } },

        { "STRLEN",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR STRLEN requires key");
                        return stringstore::strlen_(m, t[1]);
                    }, 2, 2, "STRLEN key" } },

        { "INCR",  { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR INCR requires key");
                        return stringstore::incr(m, t[1]);
                    }, 2, 2, "INCR key" } },

        { "INCRBY",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR INCRBY requires key amount");
                        return stringstore::incrby(m, t[1], t[2]);
                    }, 3, 3, "INCRBY key amount" } },

        { "DECR",  { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR DECR requires key");
                        return stringstore::decr(m, t[1]);
                    }, 2, 2, "DECR key" } },

        { "DECRBY",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR DECRBY requires key amount");
                        return stringstore::decrby(m, t[1], t[2]);
                    }, 3, 3, "DECRBY key amount" } },

        { "DEL",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR DEL requires key");
                        return stringstore::del(m, t[1]);
                    }, 2, 2, "DEL key" } },

        // ---------------- LIST COMMANDS ----------------
        { "LPUSH",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR LPUSH requires list value");
                        return liststore::lpush(m, t[1], t[2]);
                    }, 3, 3, "LPUSH list value" } },

        { "RPUSH",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR RPUSH requires list value");
                        return liststore::rpush(m, t[1], t[2]);
                    }, 3, 3, "RPUSH list value" } },

        { "LPOP", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR LPOP requires list");
                        return liststore::lpop(m, t[1]);
                    }, 2, 2, "LPOP list" } },

        { "RPOP", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR RPOP requires list");
                        return liststore::rpop(m, t[1]);
                    }, 2, 2, "RPOP list" } },

        { "LLEN", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR LLEN requires list");
                        return liststore::llen(m, t[1]);
                    }, 2, 2, "LLEN list" } },

        { "LINDEX",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR LINDEX requires list and index");
                        return liststore::lindex(m, t[1], t[2]);
                    }, 3, 3, "LINDEX list index" } },

        { "LSET", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 4) return std::string("-ERR LSET requires list, index, and value");
                        return liststore::lset(m, t[1], t[2], t[3]);
                    }, 4, 4, "LSET list index value" } },

        { "LSORT",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR LSORT requires list and order");
                        return liststore::lsort(m, t[1], t[2]);
                    }, 3, 3, "LSORT list order" } },

        { "LPRINT",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 2) return std::string("-ERR LPRINT requires list");
                        return liststore::lprint(m, t[1]);
                    }, 2, 2, "LPRINT list" } },

        // set commands
        { "SADD",    { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR SADD requires set value");
                         return setstore::sadd(m, t[1], t[2]);
                     }, 3, -1, "SADD key member [member ...]" } },

        { "SREM",    { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR SREM requires set value");
                         return setstore::srem(m, t[1], t[2]);
                     }, 3, -1, "SREM key member [member ...]" } },

        { "SMEMBERS",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 2) return std::string("-ERR SMEMBERS requires set");
                         return setstore::smembers(m, t[1]);
                     }, 2, 2, "SMEMBERS key" } },

        { "SCARD",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 2) return std::string("-ERR SCARD requires set");
                         return setstore::scard(m, t[1]);
                     }, 2, 2, "SCARD key" } },

        { "SPOP",    { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 2) return std::string("-ERR SPOP requires set");
                         return setstore::spop(m, t[1]);
                     }, 2, 2, "SPOP key" } },

        { "SISMEMBER",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR SISMEMBER requires set value");
                         return setstore::sismember(m, t[1], t[2]);
                     }, 3, 3, "SISMEMBER key member" } },

        { "SUNION", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR SUNION requires two sets");
                         return setstore::sunion(m, t[1], t[2]);
                     }, 3, -1, "SUNION key1 key2 [key...]" } },

        { "SINTER", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR SINTER requires two sets");
                         return setstore::sinter(m, t[1], t[2]);
                     }, 3, -1, "SINTER key1 key2 [key...]" } },

        { "SDIFF", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                        if (t.size() < 3) return std::string("-ERR SDIFF requires two sets");
                        return setstore::sdiff(m, t[1], t[2]);
                     }, 3, -1, "SDIFF key1 key2 [key...]" } },

        // ---------------- HASH COMMANDS ----------------
        { "HSET",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 4) return std::string("-ERR HSET requires key field value");
                         return hashmapstore::hset(m, t[1], t[2], t[3]);
                     }, 4, -1, "HSET key field value [field value ...]" } },

        { "HGET",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR HGET requires key field");
                         return hashmapstore::hget(m, t[1], t[2]);
                     }, 3, 3, "HGET key field" } },

        { "HDEL",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR HDEL requires key field(s)");
                         std::vector<std::string> fields(t.begin() + 2, t.end());
                         return hashmapstore::hdel(m, t[1], fields);
                     }, 3, -1, "HDEL key field [field ...]" } },

        { "HEXISTS",{ [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 3) return std::string("-ERR HEXISTS requires key field");
                         return hashmapstore::hexists(m, t[1], t[2]);
                     }, 3, 3, "HEXISTS key field" } },

        { "HLEN",   { [](RedisHashMap& m, const std::vector<std::string>& t) {
                         if (t.size() < 2) return std::string("-ERR HLEN requires key");
                         return hashmapstore::hlen(m, t[1]);
                     }, 2, 2, "HLEN key" } },

        { "EXPIRE", { [](RedisHashMap& m, const std::vector<std::string>& t) {
        if (t.size() < 3) return std::string("-ERR EXPIRE requires key seconds");
        const std::string key = t[1];
        const std::string secStr = t[2];
        // parse seconds
        long long seconds = 0;
        try {
            size_t idx = 0;
            seconds = std::stoll(secStr, &idx);
            if (idx != secStr.size()) return std::string("-ERR invalid seconds");
        } catch (...) {
            return std::string("-ERR invalid seconds");
        }
        // if key doesnt exist in db return 0
        if (!m.exists(key)) return std::string(":0");
        // ensure global ttl queue is created and started
        TTLPriorityQueue* q = getGlobalTTL(&m);
        bool ok = q->insertOrUpdate(key, seconds);
        return ok ? std::string(":1") : std::string(":0");
            }, 3, 3, "EXPIRE key seconds"
} },

        {"TTL", { [](RedisHashMap& m, const std::vector<std::string>& t) {
                if (t.size() < 2) return std::string("-ERR TTL requires key");
                const std::string key = t[1];
                // return -2 when key doesnt exist
                if (!m.exists(key)) return std::string(":-2");
                // if ttl not started return -1
                TTLPriorityQueue* q = getGlobalTTL(&m);
                long long rem = q->getTTLSeconds(key);
                if (rem == -2) return std::string(":-2");
                if (rem == -1) return std::string(":-1");
                return std::string(":") + std::to_string(rem);
            }, 2, 2, "TTL key"
        } },

    };
    return table;
}

const std::unordered_map<std::string, Parser::CommandSpec>& Parser::getCommandTable() {
    return buildCommandTable();
}


// command router now uses table lookup
std::string Parser::processCommand(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return std::string("-ERR empty command");
    std::string cmd = uppercpy(tokens[0]);

    const auto& table = getCommandTable();
    auto it = table.find(cmd);
    if (it == table.end()) {
        return std::string("-ERR unknown command");
    }

    const CommandSpec& spec = it->second;

    // basic arity check
    size_t tcount = tokens.size();
    if (tcount < static_cast<size_t>(spec.minArgs)) {
        return std::string("-ERR wrong number of arguments for ") + cmd;
    }
    if (spec.maxArgs != -1 && tcount > static_cast<size_t>(spec.maxArgs)) {
        return std::string("-ERR wrong number of arguments for ") + cmd;
    }

    // call the handler which is responsible for any deeper validation
    try {
        return spec.handler(baseMap, tokens);
    } catch (const std::exception& e) {
        // protect the server from exceptions in handlers
        return std::string("-ERR handler exception: ") + e.what();
    } catch (...) {
        return std::string("-ERR unknown handler exception");
    }
}
