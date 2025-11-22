#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "store.h"

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