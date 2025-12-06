#ifndef REDISDICTIONARYHASHMAP_HPP
#define REDISDICTIONARYHASHMAP_HPP

#include <string>

class HashNode {
public:
    std::string key;
    std::string value;
    HashNode* next;

    HashNode(const std::string& k, const std::string& v);
};

class MyHashMap {
private:
    int capacity;
    int count;
    HashNode** table;

    int hashFunc(const std::string& key) const;
    void rehash();

public:
    MyHashMap();
    ~MyHashMap();

    bool hset(const std::string& key, const std::string& value);
    std::string hget(const std::string& key) const;
    bool hexists(const std::string& key) const;
    bool hdel(const std::string& key);
    int hlen() const;
};

#endif
