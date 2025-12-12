#include "storage/RedisObject.hpp"
#include "storage/LinkedList.hpp"

// clear pointer header
void RedisObject::clearPtr() {
    if (!ptr) return;
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
            delete static_cast<LinkedList*>(ptr);
            break;
        case RedisType::HASH:
            delete static_cast<std::unordered_map<std::string, RedisObject>*>(ptr);
            break;
        case RedisType::SET:
            delete static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(ptr);
            break;
    }
    ptr = nullptr;
}

// -clone pointer for deep copy
void* RedisObject::clonePtr() const {
    if (!ptr) return nullptr;
    switch (type) {
        case RedisType::INT:
            return new int(*static_cast<int*>(ptr));
        case RedisType::STRING:
            return new std::string(*static_cast<std::string*>(ptr));
        case RedisType::BOOL:
            return new bool(*static_cast<bool*>(ptr));
        case RedisType::LIST: {
            // We expect a LinkedList* (created elsewhere)
            LinkedList* src = static_cast<LinkedList*>(ptr);
            if (!src) return nullptr;
            return src->clone(); // uses LinkedList::clone()
        }
        case RedisType::HASH:
            return new std::unordered_map<std::string, RedisObject>(*static_cast<std::unordered_map<std::string, RedisObject>*>(ptr));
        case RedisType::SET:
            return new std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>(*static_cast<std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>*>(ptr));
    }
    return nullptr;
}

// constructors
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

RedisObject::RedisObject(LinkedList* list) {
    type = RedisType::LIST;
    ptr = list; // ownership transferred in current design
}

RedisObject::RedisObject(const std::vector<RedisObject>& value) {
    type = RedisType::LIST;
    // If this ctor is used store a vector
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

// copy constructor deep
RedisObject::RedisObject(const RedisObject& other) {
    type = other.type;
    if (other.ptr)
        ptr = other.clonePtr();
    else
        ptr = nullptr;
}

// copy asssignment deep
RedisObject& RedisObject::operator=(const RedisObject& other) {
    if (this == &other) return *this;
    // free current
    clearPtr();
    type = other.type;
    ptr = other.ptr ? other.clonePtr() : nullptr;
    return *this;
}

// moving constructor
RedisObject::RedisObject(RedisObject&& other) noexcept {
    type = other.type;
    ptr = other.ptr;
    other.ptr = nullptr;
    // Optionally set other.type to some default; we leave it as-is
}

// moving assignment
RedisObject& RedisObject::operator=(RedisObject&& other) noexcept {
    if (this == &other) return *this;
    clearPtr();
    type = other.type;
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
}

// destructor
RedisObject::~RedisObject() {
    clearPtr();
}

// type returner
RedisType RedisObject::getType() const {
    return type;
}

// eqwuality
bool RedisObject::operator==(const RedisObject& other) const {
    if (type != other.type) return false;
    switch (type) {
        case RedisType::INT:
            return *(int*)ptr == *(int*)other.ptr;
        case RedisType::STRING:
            return *(std::string*)ptr == *(std::string*)other.ptr;
        case RedisType::BOOL:
            return *(bool*)ptr == *(bool*)other.ptr;
        case RedisType::LIST:
        case RedisType::HASH:
        case RedisType::SET:
            return ptr == other.ptr; // for complex types we still compare pointer identity
        default:
            return ptr == other.ptr;
    }
}

// hashing for redis object
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
