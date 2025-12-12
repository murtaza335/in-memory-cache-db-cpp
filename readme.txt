# In-Memory Cache Database

A high-performance, Redis-inspired in-memory cache database implemented in C++ with custom data structures and algorithms.

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Data Structures](#data-structures)
- [Algorithms](#algorithms)
- [Performance](#performance)
- [Installation](#installation)
- [Usage](#usage)
- [Supported Commands](#supported-commands)
- [Test Cases](#test-cases)
- [Team](#team)

## 🎯 Overview

This project implements a lightweight, Redis-like in-memory cache database from scratch using custom data structures. Built as part of the Data Structures & Algorithms course (CS-250), it demonstrates practical application of fundamental computer science concepts in a real-world system.

**Key Highlights:**
- 🚀 Fast O(1) average time complexity for key operations
- 🔧 Custom hash map implementation (no STL unordered_map)
- ⏱️ Built-in TTL (Time To Live) management
- 🌐 TCP server for network access
- 📦 Support for multiple data types (strings, lists, sets, hash maps)

## ✨ Features

### Core Functionality
- **Custom Hash Map Engine**: Fully implemented hash table with MurmurHash3 for optimal key distribution
- **Multiple Data Types**: 
  - Strings
  - Lists (with merge sort)
  - Sets
  - Hash maps (nested key-value pairs)
- **TTL Management**: Automatic key expiration with lazy deletion
- **Network Layer**: Lightweight TCP server for client connections
- **Command Parser**: Redis-compatible command syntax

### Technical Features
- Zero STL container dependencies for core storage
- Custom linked list implementation for lists and queues
- Min-heap based priority queue for TTL tracking
- Dynamic rehashing with 0.75 load factor threshold
- Comprehensive logging and diagnostics

## 🏗️ Architecture



### Key Modules

1. **RedisHashMap**: Core storage engine with custom hash table implementation
2. **RedisObject System**: Type abstraction layer for polymorphic data handling
3. **Data Type Stores**: Specialized operations for strings, lists, sets, and hash maps
4. **TTL/Expiry Manager**: Min-heap based priority queue for key expiration
5. **Command Parser**: Tokenizes and validates user commands
6. **TCP Server**: Socket-based networking layer for client connections
7. **MurmurHash3**: Fast, non-cryptographic hash function
8. **Logging System**: Comprehensive activity and error tracking

## 🗂️ Data Structures

| Data Structure | Usage | Implementation |
|---------------|--------|----------------|
| **Hash Table** | Base storage engine | Custom implementation with chaining |
| **Linked List** | Lists, stacks, queues | Custom node-based structure |
| **Min Heap** | TTL priority queue | Array-based implementation |
| **Hash Table** | TTL key lookup | O(1) expiry checking |
| **RedisObject** | Type abstraction | Variant-type container |

## 🧮 Algorithms

### 1. MurmurHash3 Hashing
- **Purpose**: Distribute keys evenly across hash table buckets
- **Process**: Keys → MurmurHash3 → 32-bit hash → modulo bucket count
- **Benefit**: Minimizes collisions, enables O(1) operations

### 2. Key Expiration (Lazy Deletion)
- **Frequency**: Check every 10 seconds
- **Method**: Min-heap root inspection
- **Strategy**: Remove expired keys on access (lazy evaluation)

### 3. Merge Sort for Lists
- **Complexity**: O(n log n)
- **Implementation**: Recursive split-merge on custom linked list
- **Use Case**: `SORT` command on RedisList

### 4. Dynamic Rehashing
- **Trigger**: Load factor > 0.75
- **Process**: Double capacity → rehash all entries
- **Goal**: Maintain O(1) average performance

## ⚡ Performance

| Operation | Time Complexity | Description |
|-----------|----------------|-------------|
| `SET` / `GET` | O(1) average | Hash table operations with MurmurHash3 |
| `DEL` | O(1) average | Key removal with bucket adjustment |
| `LPUSH` / `RPUSH` | O(1) | Linked list insertion |
| `SORT` (lists) | O(n log n) | Custom merge sort implementation |
| `TTL` check | O(log n) | Min-heap lookup + removal |
| `EXPIRE` | O(log n) | Min-heap insertion/update |

### Benchmarks
- **Insertion**: ~1-2 microseconds per key (average)
- **Lookup**: ~0.5-1 microseconds per key (average)
- **Memory**: Minimal overhead with custom allocations

## 🛠️ Installation

### Prerequisites
- C++17 or later
- CMake 3.10+
- Linux/macOS (Windows with WSL)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/murtaza335/in-memory-cache-db-cpp.git
cd in-memory-cache-db-cpp

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the server
./redis_cache_server
```

The server will start on **port 6379** by default.

## 🚀 Usage

### Starting the Server

```bash
./redis_cache_server
# Server listening on 0.0.0.0:6379
```

### Connecting a Client

#### Option 1: Python Client (Included)
```bash
python3 client.py
```

#### Option 2: Netcat
```bash
nc localhost 6379
```

#### Option 3: Redis CLI
```bash
redis-cli -p 6379
```

## 📝 Supported Commands

### String Operations
```bash
SET key value          # Set a key-value pair
GET key                # Retrieve value by key
DEL key                # Delete a key
EXPIRE key seconds     # Set TTL for a key
```

### List Operations
```bash
LPUSH list value       # Push to list head
RPUSH list value       # Push to list tail
LRANGE list start end  # Get range of elements
SORT list              # Sort list using merge sort
```

### Set Operations
```bash
SADD set member        # Add member to set
SMEMBERS set           # Get all set members
SREM set member        # Remove member from set
```

### Hash Map Operations
```bash
HSET hash field value  # Set field in hash
HGET hash field        # Get field from hash
HDEL hash field        # Delete field from hash
```

## 🧪 Test Cases

### Test Case 1: Basic String Operations
```bash
SET x 10
GET x
# Expected: 10
```

### Test Case 2: TTL Expiry
```bash
SET y 100
EXPIRE y 2
GET y          # Returns: 100
# Wait 3 seconds
GET y          # Returns: (nil)
```

### Test Case 3: List Operations with Sorting
```bash
LPUSH mylist 3 1 2
LRANGE mylist 0 -1    # Returns: 2 1 3
SORT mylist
LRANGE mylist 0 -1    # Returns: 1 2 3
```

### Test Case 4: Hash Map Fields
```bash
HSET myhash name John
HGET myhash name      # Returns: John
HSET myhash age 25
HGET myhash age       # Returns: 25
```

### Test Case 5: Set Operations
```bash
SADD myset a b c
SMEMBERS myset        # Returns: a b c
SREM myset b
SMEMBERS myset        # Returns: a c
```

### Test Case 6: Mixed Commands with TTL
```bash
SET temp 50
EXPIRE temp 1
GET temp              # Returns: 50
# Wait 2 seconds
GET temp              # Returns: (nil)
```

### Test Case 7: Invalid Command Handling
```bash
GET unknown_key              # Returns: (nil)
HGET unknown_hash field      # Returns: (nil)
```

## 👥 Team

| Name | ID | Responsibilities |
|------|-------|-----------------|
| **Muhammad Murtaza** | 503477 | RedisHashMap, Server, Parser, MurmurHash, RedisObject, Project Architecture |
| **Malik Abdullah Naeem** | 520654 | Strings Module, Lists Module, TTL/Expiry Management |
| **Hammad Asim Kayani** | 513776 | Sets and Dictionary Module, Logging System |

## 📚 Academic Context

**Course**: Data Structures & Algorithms (CS-250)  
**Project**: Week 14 Code Submission  
**Learning Objective**: CLO-3 - Apply data structure and algorithmic techniques to implement a functional software system

## 🔧 Project Structure

```
in-memory-cache-db-cpp/
├── include/
│   ├── RedisHashMap.hpp
│   ├── RedisObject.hpp
│   ├── StringStore.hpp
│   ├── ListStore.hpp
│   ├── SetStore.hpp
│   ├── HashMapStore.hpp
│   ├── TTLManager.hpp
│   ├── Parser.hpp
│   ├── Server.hpp
│   └── murmurhash3.hpp
├── src/
│   ├── RedisHashMap.cpp
│   ├── RedisObject.cpp
│   ├── StringStore.cpp
│   ├── ListStore.cpp
│   ├── SetStore.cpp
│   ├── HashMapStore.cpp
│   ├── TTLManager.cpp
│   ├── Parser.cpp
│   ├── Server.cpp
│   └── main.cpp
├── CMakeLists.txt
├── client.py
└── README.md
```

## 🤝 Contributing

This is an academic project. For educational purposes, feel free to fork and experiment!

## 📄 License

This project is part of an academic submission. Please respect academic integrity policies if using this code for educational purposes.

## 🙏 Acknowledgments

- Inspired by Redis architecture
- MurmurHash3 algorithm by Austin Appleby
- Course instructors and teaching assistants

---

**Repository**: [github.com/murtaza335/in-memory-cache-db-cpp](https://github.com/murtaza335/in-memory-cache-db-cpp)

Built with ❤️ for CS-250 | Data Structures & Algorithms
