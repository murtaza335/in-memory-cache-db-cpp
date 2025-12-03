# In-Memory Cache DB (C++)

A lightweight, high-performance **in-memory key-value cache/database** implemented in modern C++. This project provides a simple and efficient RAM-based storage engine designed for extremely fast reads/writes, making it ideal for caching, temporary storage, and learning database internals.

---

## 🚀 Features

* Fast in-memory key-value operations
* Simple and clean C++ API
* No external dependencies
* Modular design (separate parser, string store, list store, etc.)
* Supports strings, lists, and other data structures depending on your implementation
* CMake-based build system
* Easy to extend with additional data types or persistence

---

## 📂 Project Structure

```
├── include/            # Header files
├── src/                # Core implementation (Parser, Stores, etc.)
├── build/              # Build artifacts (after cmake ..)
├── CMakeLists.txt      # Build configuration
└── README.md           # Project documentation
```

---

## 🛠️ Building the Project

### **Requirements**

* C++11 or newer
* CMake 3.10+
* g++/clang++/MSVC

### **Build Steps**

```bash
git clone https://github.com/murtaza335/in-memory-cache-db-cpp.git
cd in-memory-cache-db-cpp
mkdir build && cd build
cmake ..
make
```

This will generate an executable inside the `build/` directory.

---

## 📘 Basic Usage Example

Below is a simplified example showing how you may interact with the cache:

```cpp
#include "StringStore.hpp"
#include "ListStore.hpp"
#include <iostream>

int main() {
    StringStore sstore;
    sstore.set("name", "Murtaza");

    auto val = sstore.get("name");
    if (val) std::cout << "Name = " << *val << std::endl;

    ListStore lstore;
    lstore.push("numbers", "10");
    lstore.push("numbers", "20");

    auto list = lstore.get("numbers");
    if (list) {
        for (const auto& item : *list) std::cout << item << " ";
    }

    return 0;
}
```

---

## 🧠 How It Works

### **Core Components**

* **StringStore** → manages key-value string pairs
* **ListStore** → manages lists of values under a key
* **Parser** → interprets user commands (SET, GET, DEL, LIST operations, etc.)
* **Command Dispatcher** → routes commands to the correct store

The design makes it easy to add:

* HashMaps for fast lookup
* TTL (time-to-live) expiration
* Concurrency (mutexes, shared locks)
* Persistence (AOF, snapshots)

---

## 📝 Supported Commands (example)

```
SET key value
GET key
DEL key
LPUSH key value
LGET key
```

Command support depends on your Parser and Store logic.

---

## 📊 Future Improvements

* Add TTL support
* LRU/LFU eviction
* Multi-threaded access
* Transaction support
* Snapshot-based persistence
* Network server mode (turn it into a mini-Redis clone)

---

## 🤝 Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch
3. Implement your change
4. Submit a pull request

---

## 📄 License

Add your preferred license here (MIT, Apache 2.0, etc.)

---

## 🙌 Acknowledgments

Inspired by common in-memory cache systems and modular C++ design patterns.
