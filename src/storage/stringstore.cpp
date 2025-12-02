#include "storage/stringstore.hpp"
#include <unordered_map>
#include <mutex>

namespace stringstore {

std::unordered_map<std::string, std::string> store;
std::mutex storeMutex;

std::string set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(storeMutex);
    store[key] = value;
    return "OK";
}

std::string get(const std::string& key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    auto it = store.find(key);
    if (it != store.end()) return it->second;
    return "NULL";
}

std::string del(const std::string& key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    if (store.erase(key) > 0) return "1"; // 1 key deleted
    return "0"; // key not found
}

}
