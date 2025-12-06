#include "../../include/storage/RedisSets.hpp"
#include "storage/murmurhash/murmurhash3.hpp"
#include <cstring>

SetNode::SetNode(const std::string& val) : value(val), next(nullptr) {}

// Hash function
int MySet::hashFunc(const std::string& val) const {
    uint32_t hash = MurmurHash3_x86_32(val, 0);
    return hash % capacity;
}

MySet::MySet() {
    capacity = 1024;
    count = 0;
    table = new SetNode*[capacity];
    for (int i = 0; i < capacity; i++)
        table[i] = nullptr;
}

MySet::~MySet() {
    for (int i = 0; i < capacity; i++) {
        SetNode* node = table[i];
        while (node) {
            SetNode* temp = node;
            node = node->next;
            delete temp;
        }
    }
    delete[] table;
}

void MySet::rehash() {
    int oldCap = capacity;
    capacity *= 2;

    SetNode** oldTable = table;
    table = new SetNode*[capacity];

    for (int i = 0; i < capacity; i++)
        table[i] = nullptr;

    count = 0;

    for (int i = 0; i < oldCap; i++) {
        SetNode* node = oldTable[i];
        while (node) {
            sadd(node->value); // reinsertion
            SetNode* temp = node;
            node = node->next;
            delete temp;
        }
    }

    delete[] oldTable;
}

bool MySet::sadd(const std::string& val) {
    if (sismember(val)) return false;

    if (count > capacity * 0.75)
        rehash();

    int idx = hashFunc(val);
    SetNode* newNode = new SetNode(val);
    newNode->next = table[idx];
    table[idx] = newNode;

    count++;
    return true;
}

bool MySet::srem(const std::string& val) {
    int idx = hashFunc(val);
    SetNode* node = table[idx];
    SetNode* prev = nullptr;

    while (node) {
        if (node->value == val) {
            if (prev) prev->next = node->next;
            else table[idx] = node->next;

            delete node;
            count--;
            return true;
        }
        prev = node;
        node = node->next;
    }
    return false;
}

bool MySet::sismember(const std::string& val) const {
    int idx = hashFunc(val);
    SetNode* node = table[idx];

    while (node) {
        if (node->value == val)
            return true;
        node = node->next;
    }
    return false;
}

int MySet::scard() const {
    return count;
}

void MySet::smembers(std::string* output, int& size) const {
    int idx = 0;

    for (int i = 0; i < capacity; i++) {
        SetNode* node = table[i];
        while (node) {
            output[idx++] = node->value;
            node = node->next;
        }
    }

    size = idx;
}
