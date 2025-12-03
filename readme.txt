
```markdown
# 🚀 In-Memory Cache DB (C++)

A lightweight, high-performance **in-memory key-value cache/database** implemented in modern C++.

This project provides a simple and efficient RAM-based storage engine designed for extremely fast reads/writes, making it ideal for caching, temporary storage, and learning database internals.

---

## ✨ Features

* **⚡ High Performance:** Fast in-memory key-value operations.
* **🧩 Modular Design:** Separate components for String Store, List Store, Parser, and Command Dispatcher.
* **🛠️ Simple API:** Clean, straightforward C++ API.
* **🚫 Zero Dependencies:** No external libraries required.
* **🏗️ Extensible:** Easy to add new data types (e.g., HashMaps), TTL, and persistence mechanisms.
* **⚙️ Build System:** Uses CMake for easy compilation across platforms.

---

## 📂 Project Structure

```

├── include/            \# Public Header files for API and components
├── src/                \# Core implementation (.cpp files for Parser, Stores, etc.)
├── build/              \# Build artifacts (generated after running 'cmake ..')
├── CMakeLists.txt      \# Main build configuration file
└── README.md           \# Project documentation

````

---

## 🛠️ Building the Project

### **Requirements**

* C++11 or newer
* CMake 3.10+
* A C++ compiler (g++/clang++/MSVC)

### **Build Steps (Linux/macOS/WSL)**

Clone the repository and build using CMake:

```bash
git clone [https://github.com/murtaza335/in-memory-cache-db-cpp.git](https://github.com/murtaza335/in-memory-cache-db-cpp.git)
cd in-memory-cache-db-cpp
mkdir build && cd build
cmake ..
make
````

The compiled executable will be generated inside the `build/` directory.

-----

## 📘 Basic Usage Example

This example demonstrates direct interaction with the core data stores (`StringStore` and `ListStore`):

```cpp
#include "StringStore.hpp"
#include "ListStore.hpp"
#include <iostream>

int main() {
    // --- String Store Example ---
    StringStore sstore;
    sstore.set("name", "Murtaza");

    auto val = sstore.get("name");
    if (val) {
        std::cout << "GET: Name = " << *val << std::endl; // Output: Name = Murtaza
    }

    // --- List Store Example ---
    ListStore lstore;
    lstore.push("numbers", "10");
    lstore.push("numbers", "20");
    lstore.push("numbers", "30");

    auto list = lstore.get("numbers");
    if (list) {
        std::cout << "LGET: Numbers = ";
        for (const auto& item : *list) {
            std::cout << item << " ";
        }
        std::cout << std::endl; // Output: 10 20 30 
    }

    return 0;
}
```

-----

## 🧠 How It Works: Core Components

| Component | Responsibility |
| :--- | :--- |
| **`StringStore`** | Manages key-value pairs where the value is a string. |
| **`ListStore`** | Manages keys mapped to a list/vector of strings (like Redis Lists). |
| **`Parser`** | Interprets raw input strings into structured commands (e.g., `SET`, `GET`). |
| **`Command Dispatcher`** | Routes a parsed command to the correct data store for execution. |

The decoupled architecture simplifies the integration of new features like:

  * HashMaps/Sets
  * **TTL** (Time-To-Live) expiration
  * **Concurrency** (using mutexes/shared locks)
  * **Persistence** (AOF, snapshots)

-----

## 📝 Supported Commands (Parser Example)

The following command syntax is interpreted by the `Parser` and executed by the `Command Dispatcher`:

| Command | Store | Description | Example |
| :--- | :--- | :--- | :--- |
| `SET key value` | String | Sets a string value for a key. | `SET user:id 101` |
| `GET key` | String | Retrieves the string value associated with a key. | `GET user:id` |
| `DEL key` | All | Deletes the key and its associated value from the store. | `DEL user:id` |
| `LPUSH key value` | List | Inserts a value at the head (left) of a list. | `LPUSH log:events "event_A"` |
| `LGET key` | List | Retrieves all elements of a list. | `LGET log:events` |

> Command support is dependent on the current implementation of the Parser and Store logic.

-----

## 📈 Future Improvements

We plan to expand the project with the following features:

  * **🕒 TTL Support:** Implement time-to-live expiration for keys.
  * **🗑️ Eviction Policies:** Add LRU/LFU algorithms for automatic key eviction.
  * **🔒 Concurrency:** Introduce multi-threaded access using mutexes for thread safety.
  * **🤝 Transactions:** Support atomic multi-command execution.
  * **💾 Persistence:** Implement snapshot-based (RDB) and append-only file (AOF) persistence.
  * **🌐 Network Server Mode:** Build a simple TCP server to enable remote client connections (a mini-Redis clone).

-----

## 🤝 Contributing

Contributions are highly appreciated\! Please follow these steps to contribute:

1.  **Fork** the repository.
2.  Create a descriptive feature branch (`git checkout -b feat/add-ttl`).
3.  Implement your changes and ensure tests pass (if any).
4.  Submit a **Pull Request** explaining your contribution.

-----

## 📄 License

\[Add your preferred license here, e.g., **MIT License**]

-----

## 🙌 Acknowledgments

Inspired by the internal designs of common high-performance in-memory cache systems and the pursuit of clean, modular C++ development.

```
```
