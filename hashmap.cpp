#include <iostream>
#include <vector>
#include <list>
#include <string>

template <typename KeyType, typename ValueType>
class MyHashMap {
private:
    static const int DEFAULT_CAPACITY = 10; // Initial size of the hash table
    std::vector<std::list<std::pair<KeyType, ValueType>>> table;
    int currentSize;

    // A simple hash function (can be made more robust)
    size_t hashFunction(const KeyType& key) const {
        // Using std::hash for basic types, or you can implement your own
        return std::hash<KeyType>{}(key) % table.size();
    }

public:
    MyHashMap() : currentSize(0) {
        table.resize(DEFAULT_CAPACITY);
    }

    // Insert or update a key-value pair
    void put(const KeyType& key, const ValueType& value) {
        size_t index = hashFunction(key);
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value; // Key exists, update value
                return;
            }
        }
        // Key not found, insert new pair
        table[index].push_back({key, value});
        currentSize++;
    }

    // Retrieve the value associated with a key
    ValueType get(const KeyType& key) const {
        size_t index = hashFunction(key);
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                return pair.second;
            }
        }
        // Key not found, handle error (e.g., throw exception or return default value)
        throw std::runtime_error("Key not found");
    }

    // Remove a key-value pair
    void remove(const KeyType& key) {
        size_t index = hashFunction(key);
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                currentSize--;
                return;
            }
        }
    }

    // Check if a key exists
    bool containsKey(const KeyType& key) const {
        size_t index = hashFunction(key);
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                return true;
            }
        }
        return false;
    }

    int size() const {
        return currentSize;
    }
};

int main() {
    MyHashMap<std::string, int> myMap;

    myMap.put("apple", 10);
    myMap.put("banana", 20);
    myMap.put("cherry", 30);

    std::cout << "Value of apple: " << myMap.get("apple") << std::endl;
    std::cout << "Value of banana: " << myMap.get("banana") << std::endl;

    myMap.put("apple", 15); // Update apple's value
    std::cout << "New value of apple: " << myMap.get("apple") << std::endl;

    myMap.remove("banana");
    std::cout << "Contains banana? " << (myMap.containsKey("banana") ? "Yes" : "No") << std::endl;

    try {
        myMap.get("grape"); // This will throw an exception
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}