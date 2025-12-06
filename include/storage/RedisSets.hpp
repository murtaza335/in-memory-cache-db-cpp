#ifndef REDISSETS_HPP
#define REDISSETS_HPP

#include <string>

class SetNode {
public:
    std::string value;
    SetNode* next;

    SetNode(const std::string& val);
};

class MySet {
private:
    int capacity;
    int count;
    SetNode** table;

    int hashFunc(const std::string& val) const;
    void rehash();

public:
    MySet();
    ~MySet();

    bool sadd(const std::string& val);
    bool srem(const std::string& val);
    bool sismember(const std::string& val) const;
    int scard() const;

    void smembers(std::string* output, int& size) const;
};

#endif
