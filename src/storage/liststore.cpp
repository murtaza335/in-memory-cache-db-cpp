#include "storage/liststore.hpp"
#include "storage/RedisObject.hpp"
#include "storage/LinkedList.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>

namespace liststore {

// timestamp utility
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return buffer;
}

// Lpush
std::string lpush(RedisHashMap& map, const std::string& key, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] LPUSH operation - Key: " << key 
              << ", Value: " << value << std::endl;
    
    RedisObject* obj = map.get(key);
    LinkedList* list;

    if (!obj) {
        list = new LinkedList();
        list->push_front(value);          
        map.add(key, RedisObject(list));
        std::cout << "[" << getTimestamp() << "] [INFO] LPUSH - New list created for key: " 
                  << key << ", List size: 1" << std::endl;
        return ":1";
    }

    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LPUSH - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    list = static_cast<LinkedList*>(obj->getPtr());
    list->push_front(value);
    std::cout << "[" << getTimestamp() << "] [INFO] LPUSH - Key: " << key 
              << ", Value pushed to front, List size: " << list->size << std::endl;

    return ":" + std::to_string(list->size);
}


// Rpush
std::string rpush(RedisHashMap& map, const std::string& key, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] RPUSH operation - Key: " << key 
              << ", Value: " << value << std::endl;
    
    RedisObject* obj = map.get(key);
    LinkedList* list;

    if (!obj) {
        list = new LinkedList();
        list->push_back(value);            
        map.add(key, RedisObject(list));
        std::cout << "[" << getTimestamp() << "] [INFO] RPUSH - New list created for key: " 
                  << key << ", List size: 1" << std::endl;
        return ":1";
    }

    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] RPUSH - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    list = static_cast<LinkedList*>(obj->getPtr());
    list->push_back(value);
    std::cout << "[" << getTimestamp() << "] [INFO] RPUSH - Key: " << key 
              << ", Value pushed to back, List size: " << list->size << std::endl;

    return ":" + std::to_string(list->size);
}


// Lpop
std::string lpop(RedisHashMap& map, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] LPOP operation - Key: " << key << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] LPOP - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LPOP - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (list->empty()) {
        std::cout << "[" << getTimestamp() << "] [WARN] LPOP - List is empty for key: " << key << std::endl;
        return "$-1";
    }

    std::string val = list->pop_front();
    std::cout << "[" << getTimestamp() << "] [INFO] LPOP - SUCCESS - Key: " << key 
              << ", Popped value: " << val << ", Remaining size: " << list->size << std::endl;
    return val;
}

// Rpop
std::string rpop(RedisHashMap& map, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] RPOP operation - Key: " << key << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] RPOP - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] RPOP - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (list->empty()) {
        std::cout << "[" << getTimestamp() << "] [WARN] RPOP - List is empty for key: " << key << std::endl;
        return "$-1";
    }

    std::string val = list->pop_back();
    std::cout << "[" << getTimestamp() << "] [INFO] RPOP - SUCCESS - Key: " << key 
              << ", Popped value: " << val << ", Remaining size: " << list->size << std::endl;
    return val;
}

// Llen
std::string llen(RedisHashMap& map, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] LLEN operation - Key: " << key << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] LLEN - Key not found: " << key << std::endl;
        return ":0";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LLEN - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    std::cout << "[" << getTimestamp() << "] [INFO] LLEN - Key: " << key 
              << ", List size: " << list->size << std::endl;
    return ":" + std::to_string(list->size);
}

// Lindex
std::string lindex(RedisHashMap& map, const std::string& key, const std::string& indexStr) {
    std::cout << "[" << getTimestamp() << "] [INFO] LINDEX operation - Key: " << key 
              << ", Index: " << indexStr << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] LINDEX - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LINDEX - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    long long idx;
    try { idx = std::stoll(indexStr); } 
    catch (...) { 
        std::cout << "[" << getTimestamp() << "] [ERROR] LINDEX - Invalid index: " << indexStr << std::endl;
        return "-ERR invalid index"; 
    }

    try {
        std::string val = list->get(idx);
        std::cout << "[" << getTimestamp() << "] [INFO] LINDEX - SUCCESS - Key: " << key 
                  << ", Index: " << idx << ", Value: " << val << std::endl;
        return val;
    } catch (...) {
        std::cout << "[" << getTimestamp() << "] [WARN] LINDEX - Index out of range - Key: " << key 
                  << ", Index: " << idx << std::endl;
        return "$-1";
    }
}

// Lset
std::string lset(RedisHashMap& map, const std::string& key, const std::string& indexStr, const std::string& value) {
    std::cout << "[" << getTimestamp() << "] [INFO] LSET operation - Key: " << key 
              << ", Index: " << indexStr << ", Value: " << value << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSET - Key not found: " << key << std::endl;
        return "-ERR no such key";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSET - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (!list) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSET - Internal error: list pointer null for key: " << key << std::endl;
        return "-ERR internal error: list pointer null";
    }

    long long idx;
    try { idx = std::stoll(indexStr); } 
    catch (...) { 
        std::cout << "[" << getTimestamp() << "] [ERROR] LSET - Invalid index: " << indexStr << std::endl;
        return "-ERR invalid index"; 
    }

    long long sz = static_cast<long long>(list->size);
    if (idx < 0) idx += sz;

    if (idx < 0 || idx >= sz) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSET - Index out of range - Key: " << key 
                  << ", Index: " << idx << ", List size: " << sz << std::endl;
        return "-ERR index out of range";
    }

    list->set(idx, value);
    std::cout << "[" << getTimestamp() << "] [INFO] LSET - SUCCESS - Key: " << key 
              << ", Index: " << idx << ", New value: " << value << std::endl;
    return "+OK";
}

std::string lsort(RedisHashMap& map, const std::string& key, const std::string& orderStr) {
    std::cout << "[" << getTimestamp() << "] [INFO] LSORT operation - Key: " << key 
              << ", Order: " << orderStr << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSORT - Key not found: " << key << std::endl;
        return "-ERR no such key";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSORT - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    int order;
    try {
        order = std::stoi(orderStr);
    } catch (...) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSORT - Invalid order value: " << orderStr << std::endl;
        return "-ERR invalid order";
    }

    if (order != 1 && order != 2) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSORT - Order must be 1 or 2, got: " << order << std::endl;
        return "-ERR order must be 1 (asc) or 2 (desc)";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());

    try {
        list->sort(order == 1);
        std::cout << "[" << getTimestamp() << "] [INFO] LSORT - SUCCESS - Key: " << key 
                  << ", Order: " << (order == 1 ? "ASCENDING" : "DESCENDING") 
                  << ", List size: " << list->size << std::endl;
    } catch (...) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LSORT - List contains non-numeric values for key: " << key << std::endl;
        return "-ERR list contains non-numeric values";
    }

    return "+OK";
}

// Lprint
std::string lprint(RedisHashMap& map, const std::string& key) {
    std::cout << "[" << getTimestamp() << "] [INFO] LPRINT operation - Key: " << key << std::endl;
    
    RedisObject* obj = map.get(key);
    if (!obj) {
        std::cout << "[" << getTimestamp() << "] [WARN] LPRINT - Key not found: " << key << std::endl;
        return "$-1";
    }
    if (obj->getType() != RedisType::LIST) {
        std::cout << "[" << getTimestamp() << "] [ERROR] LPRINT - Wrong type for key: " << key << std::endl;
        return "-ERR wrong type";
    }

    LinkedList* list = static_cast<LinkedList*>(obj->getPtr());
    if (!list || list->empty()) {
        std::cout << "[" << getTimestamp() << "] [INFO] LPRINT - Empty list for key: " << key << std::endl;
        return "[]";
    }

    std::ostringstream out;
    out << "[";

    ListNode* curr = list->head;
    while (curr) {
        out << curr->value;
        if (curr->next) out << ", ";
        curr = curr->next;
    }

    out << "]";
    std::cout << "[" << getTimestamp() << "] [INFO] LPRINT - SUCCESS - Key: " << key 
              << ", Elements: " << list->size << std::endl;
    return out.str();
}
} 
