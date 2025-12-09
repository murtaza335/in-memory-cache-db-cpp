#include "storage/liststore.hpp"
#include "storage/RedisObject.hpp"
#include "storage/LinkedList.hpp"
#include <vector>
#include <string>
#include <sstream>

namespace liststore {

// -------------------- LPUSH --------------------
std::string lpush(RedisHashMap& map, const std::string& key, const std::string& value) {
    RedisObject* obj = map.get(key);
    LinkedList* list;

    if (!obj) {
        list = new LinkedList();
        list->push_front(value);          
        map.add(key, RedisObject(list)); 
        return ":1";
    }

    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    list = static_cast<LinkedList*>(obj->getPtr());
    list->push_front(value);

    return ":" + std::to_string(list->size);
}


// -------------------- RPUSH --------------------
std::string rpush(RedisHashMap& map, const std::string& key, const std::string& value) {
    RedisObject* obj = map.get(key);
    LinkedList* list;

    if (!obj) {
        list = new LinkedList();
        list->push_back(value);            
        map.add(key, RedisObject(list));  
        return ":1";
    }

    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    list = static_cast<LinkedList*>(obj->getPtr());
    list->push_back(value);

    return ":" + std::to_string(list->size);
}


// -------------------- LPOP --------------------
std::string lpop(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (list->empty()) return "$-1";

    std::string val = list->pop_front();
    return val;
}

// -------------------- RPOP --------------------
std::string rpop(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (list->empty()) return "$-1";

    std::string val = list->pop_back();
    return val;
}

// -------------------- LLEN --------------------
std::string llen(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);
    if (!obj) return ":0";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    return ":" + std::to_string(list->size);
}

// -------------------- LINDEX --------------------
std::string lindex(RedisHashMap& map, const std::string& key, const std::string& indexStr) {
    RedisObject* obj = map.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    long long idx;
    try { idx = std::stoll(indexStr); } 
    catch (...) { return "-ERR invalid index"; }

    try {
        std::string val = list->get(idx);
        return val;
    } catch (...) {
        return "$-1";
    }
}

// -------------------- LSET --------------------
std::string lset(RedisHashMap& map, const std::string& key, const std::string& indexStr, const std::string& value) {
    RedisObject* obj = map.get(key);
    if (!obj) return "-ERR no such key";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (!list) return "-ERR internal error: list pointer null";

    long long idx;
    try { idx = std::stoll(indexStr); } 
    catch (...) { return "-ERR invalid index"; }

    long long sz = static_cast<long long>(list->size);
    if (idx < 0) idx += sz;

    if (idx < 0 || idx >= sz) return "-ERR index out of range";

    list->set(idx, value);
    return "+OK";
}

std::string lsort(RedisHashMap& map, const std::string& key, const std::string& orderStr) {
    RedisObject* obj = map.get(key);
    if (!obj) return "-ERR no such key";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    int order;
    try {
        order = std::stoi(orderStr);
    } catch (...) {
        return "-ERR invalid order";
    }

    if (order != 1 && order != 2)
        return "-ERR order must be 1 (asc) or 2 (desc)";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());

    try {
        list->sort(order == 1);
    } catch (...) {
        return "-ERR list contains non-numeric values";
    }

    return "+OK";
}

// -------------------- LPRINT --------------------
std::string lprint(RedisHashMap& map, const std::string& key) {
    RedisObject* obj = map.get(key);
    if (!obj) return "$-1";
    if (obj->getType() != RedisType::LIST) return "-ERR wrong type";

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (!list || list->empty()) return "[]";

    std::ostringstream out;
    out << "[";

    ListNode* curr = list->head;
    while (curr) {
        out << curr->value;
        if (curr->next) out << ", ";
        curr = curr->next;
    }

    out << "]";
    return out.str();
}


} // namespace liststore
