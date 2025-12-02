#include "storage/liststore.hpp"
#include <unordered_map>
#include <vector>
#include <mutex>

namespace liststore {

std::unordered_map<std::string, std::vector<std::string>> store;
std::mutex storeMutex;

std::string lpush(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(storeMutex);
    store[key].insert(store[key].begin(), value);
    return std::to_string(store[key].size());
}

std::string rpush(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(storeMutex);
    store[key].push_back(value);
    return std::to_string(store[key].size());
}

std::string lpop(const std::string& key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    auto& vec = store[key];
    if (vec.empty()) return "NULL";
    std::string val = vec.front();
    vec.erase(vec.begin());
    return val;
}

std::string rpop(const std::string& key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    auto& vec = store[key];
    if (vec.empty()) return "NULL";
    std::string val = vec.back();
    vec.pop_back();
    return val;
}

}
