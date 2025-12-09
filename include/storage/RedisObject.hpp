#ifndef REDIS_OBJECT_HPP
#define REDIS_OBJECT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "storage/LinkedList.hpp"

// Forward declaration for recursive types
class RedisObject;

// Hash and equality for RedisObject (for sets)
struct RedisObjectHash;
struct RedisObjectEqual;

// Supported types
enum class RedisType {
    INT,
    STRING,
    BOOL,
    LIST,
    HASH,
    SET
};

class RedisObject {
private:
    RedisType type;
    void* ptr;

    // helper to free ptr and reset
    void clearPtr();

    // helper to clone pointer for copy constructor/assignment
    void* clonePtr() const;

public:
    // ---------- Constructors ----------
    RedisObject(int value);
    RedisObject(const std::string& value);
    RedisObject(bool value);
    RedisObject(LinkedList* list);
    RedisObject(const std::vector<RedisObject>& value);
    RedisObject(const std::unordered_map<std::string, RedisObject>& value);
    RedisObject(const std::unordered_set<RedisObject, RedisObjectHash, RedisObjectEqual>& value);

    // ---------- Rule of five ----------
    // Copy constructor (deep copy)
    RedisObject(const RedisObject& other);

    // Copy assignment (deep copy)
    RedisObject& operator=(const RedisObject& other);

    // Move constructor
    RedisObject(RedisObject&& other) noexcept;

    // Move assignment
    RedisObject& operator=(RedisObject&& other) noexcept;

    // ---------- Destructor ----------
    ~RedisObject();


    // ---------- Type Getter ----------
    RedisType getType() const;

    // ---------- getValue ----------
    template<typename T>
    T& getValue() const {
        return *static_cast<T*>(ptr);
    }

        // ---------- Pointer Getter ----------
    void* getPtr() const {
        return ptr;
    }

    // ---------- Equality operator ----------
    bool operator==(const RedisObject& other) const;

    // Allow hash functions to access private ptr
    friend struct RedisObjectHash;
    friend struct RedisObjectEqual;
};

// ---------- Hash and equality for RedisObject (for sets) ----------
struct RedisObjectHash {
    std::size_t operator()(const RedisObject& obj) const;
};

struct RedisObjectEqual {
    bool operator()(const RedisObject& a, const RedisObject& b) const;
};

#endif // REDIS_OBJECT_HPP
