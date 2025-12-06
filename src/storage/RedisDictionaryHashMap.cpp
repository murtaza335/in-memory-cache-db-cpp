#include "../../include/storage/RedisDictionaryHashMap.hpp"
#include "storage/murmurhash/murmurhash3.hpp"

HashNode::HashNode(const std::string& k, const std::string& v)
    : key(k), value(v), next(nullptr) {}

int MyHashMap::hashFunc(const std::string& key) const {
    uint32_t hash = MurmurHash3_x86_32(key, 0);
    return hash % capacity;
}

MyHashMap::MyHashMap() {
    capacity = 1024;
    count = 0;
    table = new HashNode*[capacity];
    for (int i = 0; i < capacity; i++)
        table[i] = nullptr;
}

MyHashMap::~MyHashMap() {
    for (int i = 0; i < capacity; i++) {
        HashNode* node = table[i];
        while (node) {
            HashNode* temp = node;
            node = node->next;
            delete temp;
        }
    }
    delete[] table;
}

void MyHashMap::rehash() {
    int oldCap = capacity;
    capacity *= 2;

    HashNode** oldTable = table;
    table = new HashNode*[capacity];

    for (int i = 0; i < capacity; i++)
        table[i] = nullptr;

    count = 0;

    for (int i = 0; i < oldCap; i++) {
        HashNode* node = oldTable[i];
        while (node) {
            hset(node->key, node->value);
            HashNode* temp = node;
            node = node->next;
            delete temp;
        }
    }

    delete[] oldTable;
}

bool MyHashMap::hset(const std::string& key, const std::string& value) {
    if (count > capacity * 0.75)
        rehash();

    int idx = hashFunc(key);
    HashNode* node = table[idx];

    while (node) {
        if (node->key == key) {
            node->value = value;
            return false;
        }
        node = node->next;
    }

    HashNode* newNode = new HashNode(key, value);
    newNode->next = table[idx];
    table[idx] = newNode;

    count++;
    return true;
}

std::string MyHashMap::hget(const std::string& key) const {
    int idx = hashFunc(key);
    HashNode* node = table[idx];

    while (node) {
        if (node->key == key)
            return node->value;
        node = node->next;
    }

    return "";
}

bool MyHashMap::hexists(const std::string& key) const {
    int idx = hashFunc(key);
    HashNode* node = table[idx];

    while (node) {
        if (node->key == key)
            return true;
        node = node->next;
    }
    return false;
}

bool MyHashMap::hdel(const std::string& key) {
    int idx = hashFunc(key);
    HashNode* node = table[idx];
    HashNode* prev = nullptr;

    while (node) {
        if (node->key == key) {
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

int MyHashMap::hlen() const {
    return count;
}
