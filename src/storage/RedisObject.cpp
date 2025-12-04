#include "RedisObject.hpp"

// ---------- Constructors ----------

RedisObject::RedisObject(int value) {
    type = RedisType::INT;
    ptr = new int(value);
}

RedisObject::RedisObject(const std::string& value) {
    type = RedisType::STRING;
    ptr = new std::string(value);
}

RedisObject::RedisObject(bool value) {
    type = RedisType::BOOL;
    ptr = new bool(value);
}

RedisObject::RedisObject(const std::vector<RedisObject>& value) {
    type = RedisType::LIST;
    ptr = new std::vector<RedisObject>(value);
}

RedisObject::RedisObject(const std::unordered_map<std::string, RedisObject>& value) {
    type = RedisType::HASH;
    ptr = new std::unordered_map<std::string, RedisObject>(value);
}

RedisObject::RedisObject(const std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>& value) {
    type = RedisType::SET;
    ptr = new std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>(value);
}

// ---------- Destructor ----------
RedisObject::~RedisObject() {
    switch (type) {
        case RedisType::INT:
            delete static_cast<int*>(ptr);
            break;
        case RedisType::STRING:
            delete static_cast<std::string*>(ptr);
            break;
        case RedisType::BOOL:
            delete static_cast<bool*>(ptr);
            break;
        case RedisType::LIST:
            delete static_cast<std::vector<RedisObject>*>(ptr);
            break;
        case RedisType::HASH:
            delete static_cast<std::unordered_map<std::string, RedisObject>*>(ptr);
            break;
        case RedisType::SET:
            delete static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(ptr);
            break;
    }
}

// ---------- Type Getter ----------
RedisType RedisObject::getType() const {
    return type;
}

// ---------- Equality ----------
bool RedisObject::operator==(const RedisObject& other) const {
    if (type != other.type) return false;
    switch (type) {
        case RedisType::INT:
            return *(int*)ptr == *(int*)other.ptr;
        case RedisType::STRING:
            return *(std::string*)ptr == *(std::string*)other.ptr;
        case RedisType::BOOL:
            return *(bool*)ptr == *(bool*)other.ptr;
        default:
            return ptr == other.ptr; // pointer compare for complex types
    }
}

// ---------- Hash functions for RedisObject ----------
std::size_t RedisObjectHash::operator()(const RedisObject& obj) const {
    switch (obj.type) {
        case RedisType::INT:
            return std::hash<int>()(*(int*)obj.ptr);
        case RedisType::STRING:
            return std::hash<std::string>()(*(std::string*)obj.ptr);
        case RedisType::BOOL:
            return std::hash<bool>()(*(bool*)obj.ptr);
        default:
            return std::hash<void*>()(obj.ptr); // fallback for list/hash/set
    }
}

bool RedisObjectEqual::operator()(const RedisObject& a, const RedisObject& b) const {
    return a == b;
}
