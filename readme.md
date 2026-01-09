# In-Memory Cache Database

A high-performance, Redis-inspired in-memory cache database implemented in C++ with custom data structures and algorithms.

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Quick Start](#quick-start)
- [Architecture](#architecture)
- [Data Structures](#data-structures)
- [Algorithms](#algorithms)
- [Performance](#performance)
- [Installation](#installation)
- [Usage](#usage)
- [Command Reference & Syntax](#command-reference--syntax)
  - [String Commands](#-string-commands)
  - [List Commands](#-list-commands)
  - [Set Commands](#-set-commands)
  - [Hash Map Commands](#-hash-map-commands)
  - [TTL Commands](#-ttl-time-to-live-commands)
- [Test Cases & Examples](#test-cases--examples)
- [Team](#team)

## 🎯 Overview

This project implements a lightweight, Redis-like in-memory cache database from scratch using custom data structures. Built as part of the Data Structures & Algorithms course (CS-250), it demonstrates practical application of fundamental computer science concepts in a real-world system.

**Key Highlights:**
- 🚀 Fast O(1) average time complexity for key operations
- 🔧 Custom hash map implementation (no STL unordered_map)
- ⏱️ Built-in TTL (Time To Live) management
- 🌐 TCP server for network access
- 📦 Support for multiple data types (strings, lists, sets, hash maps)

---

## 🚀 Quick Start

```bash
# 1. Build the project
mkdir build && cd build
cmake ..
make

# 2. Start the server
./main
# Server starts on port 6379

# 3. In another terminal, run the Python client
python3 client.py

# 4. Try some commands
SET mykey hello
GET mykey
# Response: hello

LPUSH mylist item1 item2 item3
LPRINT mylist
# Response: item3 item2 item1

SADD myset apple banana cherry
SMEMBERS myset
# Response: apple banana cherry
```

---

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

### Module Structure

```
┌─────────────────────────────────────────────┐
│          TCP Server (Port 6379)             │
└──────────────────┬──────────────────────────┘
                   │
┌──────────────────▼──────────────────────────┐
│       Command Parser & Executor             │
└──────────────────┬──────────────────────────┘
                   │
┌──────────────────▼──────────────────────────┐
│          RedisObject System                 │
│     (Unified Data Type Container)           │
└──────────────────┬──────────────────────────┘
                   │
      ┌────────────┼────────────┐
      │            │            │
┌─────▼─────┐ ┌───▼────┐ ┌────▼─────┐
│StringStore│ │ListStore│ │SetStore  │
└───────────┘ └─────────┘ └──────────┘
      │            │            │
      └────────────┼────────────┘
                   │
         ┌─────────▼──────────┐
         │  RedisHashMap      │
         │  (Core Storage)    │
         └────────────────────┘
                   │
         ┌─────────▼──────────┐
         │  TTL Manager       │
         │  (Min Heap + HT)   │
         └────────────────────┘
```

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

### Connecting to the Server

#### Option 1: Python Client (Recommended for Testing)
We provide a simple Python client for testing the cache server:

```bash
python3 client.py
```

**Features:**
- Interactive command-line interface
- Automatic connection handling
- Clean request/response display
- Type `quit` to exit

**Example Session:**
```bash
$ python3 client.py
Connected to server at 127.0.0.1:6379
Enter message (or 'quit' to exit): SET mykey hello
Server: +OK
Enter message (or 'quit' to exit): GET mykey
Server: hello
Enter message (or 'quit' to exit): quit
Disconnected.
```

#### Option 2: Netcat
```bash
nc localhost 6379
```

#### Option 3: Redis CLI
```bash
redis-cli -p 6379
```

#### Option 4: Telnet
```bash
telnet localhost 6379
```

---

## 📝 Command Reference & Syntax

### Communication Protocol

**Format:** All commands are plain text, space-separated, and terminated with newline (`\n`)

**Syntax Pattern:**
```
COMMAND arg1 arg2 arg3 ...
```

**Response Format:**
- `+OK` - Successful operation
- `+<value>` - String value response
- `:<number>` - Integer response
- `-ERR <message>` - Error response
- `(nil)` - Null/non-existent value

**Important Notes:**
- Commands are **case-insensitive** (SET = set = Set)
- Arguments are **space-separated**
- No quotes needed for single-word values
- Multi-word values should avoid spaces (use underscores: `hello_world`)

---

### 📦 String Commands

String commands operate on simple key-value pairs where values are strings.

#### SET
Set a key to hold a string value.

**Syntax:** `SET key value`

**Examples:**
```bash
SET name John
# Response: +OK

SET counter 42
# Response: +OK

SET message hello_world
# Response: +OK
```

**Response:** `+OK`

---

#### GET
Get the value of a key.

**Syntax:** `GET key`

**Examples:**
```bash
GET name
# Response: John

GET counter
# Response: 42

GET nonexistent
# Response: (nil)
```

**Response:** String value or `(nil)` if key doesn't exist

---

#### SETNX
Set a key only if it doesn't already exist.

**Syntax:** `SETNX key value`

**Examples:**
```bash
SETNX newkey value1
# Response: :1

SETNX newkey value2
# Response: :0 (key already exists)
```

**Response:** `:1` (set) or `:0` (not set)

---

#### MSET
Set multiple key-value pairs atomically.

**Syntax:** `MSET key1 value1 key2 value2 [key3 value3 ...]`

**Examples:**
```bash
MSET x 10 y 20 z 30
# Response: +OK

MSET name Alice age 25
# Response: +OK
```

**Response:** `+OK`

---

#### MGET
Get values of multiple keys.

**Syntax:** `MGET key1 key2 [key3 ...]`

**Examples:**
```bash
MSET x 10 y 20 z 30
MGET x y z
# Response: 10 20 30

MGET x missing y
# Response: 10 (nil) 20
```

**Response:** Space-separated values (nil for missing keys)

---

#### APPEND
Append a value to an existing key.

**Syntax:** `APPEND key value`

**Examples:**
```bash
SET msg hello
APPEND msg world
# Response: :10 (new length)

GET msg
# Response: helloworld
```

**Response:** `:length` (new string length)

---

#### STRLEN
Get the length of the value stored at key.

**Syntax:** `STRLEN key`

**Examples:**
```bash
SET mykey HelloWorld
STRLEN mykey
# Response: :10

STRLEN nonexistent
# Response: :0
```

**Response:** `:length`

---

#### INCR
Increment the integer value of a key by 1.

**Syntax:** `INCR key`

**Examples:**
```bash
SET counter 10
INCR counter
# Response: :11

INCR counter
# Response: :12

INCR newcounter
# Response: :1 (auto-initialized to 0, then incremented)
```

**Response:** `:new_value`

---

#### INCRBY
Increment the integer value by a specific amount.

**Syntax:** `INCRBY key amount`

**Examples:**
```bash
SET score 100
INCRBY score 50
# Response: :150

INCRBY score -20
# Response: :130
```

**Response:** `:new_value`

---

#### DECR
Decrement the integer value of a key by 1.

**Syntax:** `DECR key`

**Examples:**
```bash
SET counter 10
DECR counter
# Response: :9
```

**Response:** `:new_value`

---

#### DECRBY
Decrement the integer value by a specific amount.

**Syntax:** `DECRBY key amount`

**Examples:**
```bash
SET health 100
DECRBY health 25
# Response: :75
```

**Response:** `:new_value`

---

#### DEL
Delete a key.

**Syntax:** `DEL key`

**Examples:**
```bash
SET temp value
DEL temp
# Response: :1 (deleted)

DEL nonexistent
# Response: :0 (not found)
```

**Response:** `:1` (deleted) or `:0` (not found)

---

### 📋 List Commands

List commands operate on linked lists stored at keys.

#### LPUSH
Insert a value at the head (left) of the list.

**Syntax:** `LPUSH list value`

**Examples:**
```bash
LPUSH mylist first
# Response: :1

LPUSH mylist second
# Response: :2

# List is now: [second, first]
```

**Response:** `:length` (new list length)

---

#### RPUSH
Insert a value at the tail (right) of the list.

**Syntax:** `RPUSH list value`

**Examples:**
```bash
RPUSH mylist first
# Response: :1

RPUSH mylist second
# Response: :2

# List is now: [first, second]
```

**Response:** `:length` (new list length)

---

#### LPOP
Remove and return the first element from the list.

**Syntax:** `LPOP list`

**Examples:**
```bash
LPUSH mylist a b c
# List: [c, b, a]

LPOP mylist
# Response: c

LPOP mylist
# Response: b
```

**Response:** Popped value or `(nil)` if empty

---

#### RPOP
Remove and return the last element from the list.

**Syntax:** `RPOP list`

**Examples:**
```bash
RPUSH mylist a b c
# List: [a, b, c]

RPOP mylist
# Response: c

RPOP mylist
# Response: b
```

**Response:** Popped value or `(nil)` if empty

---

#### LLEN
Get the length of a list.

**Syntax:** `LLEN list`

**Examples:**
```bash
RPUSH mylist a b c
LLEN mylist
# Response: :3

LLEN nonexistent
# Response: :0
```

**Response:** `:length`

---

#### LINDEX
Get an element from a list by its index.

**Syntax:** `LINDEX list index`

**Examples:**
```bash
RPUSH mylist a b c
# List: [a, b, c]

LINDEX mylist 0
# Response: a

LINDEX mylist 2
# Response: c

LINDEX mylist -1
# Response: c (negative indices supported)

LINDEX mylist 10
# Response: (nil)
```

**Response:** Element value or `(nil)` if out of range

---

#### LSET
Set the value of an element in a list by its index.

**Syntax:** `LSET list index value`

**Examples:**
```bash
RPUSH mylist a b c
LSET mylist 1 X
# Response: +OK

LINDEX mylist 1
# Response: X
```

**Response:** `+OK` or `-ERR` if index out of range

---

#### LSORT
Sort the list in ascending or descending order using merge sort.

**Syntax:** `LSORT list order`  
**Order:** `asc` (ascending) or `desc` (descending)

**Examples:**
```bash
RPUSH numbers 5 2 8 1 9
LSORT numbers asc
# Response: +OK

LPRINT numbers
# Response: 1 2 5 8 9

LSORT numbers desc
LPRINT numbers
# Response: 9 8 5 2 1
```

**Response:** `+OK`

---

#### LPRINT
Print all elements in a list (space-separated).

**Syntax:** `LPRINT list`

**Examples:**
```bash
RPUSH mylist apple banana cherry
LPRINT mylist
# Response: apple banana cherry

LPRINT emptylist
# Response: (nil)
```

**Response:** Space-separated elements or `(nil)`

---

### 🎲 Set Commands

Set commands operate on unordered collections of unique strings.

#### SADD
Add one or more members to a set.

**Syntax:** `SADD key member [member ...]`

**Examples:**
```bash
SADD myset apple
# Response: :1 (added)

SADD myset banana cherry
# Response: :2 (2 new members added)

SADD myset apple
# Response: :0 (already exists)
```

**Response:** `:count` (number of new members added)

---

#### SREM
Remove one or more members from a set.

**Syntax:** `SREM key member [member ...]`

**Examples:**
```bash
SADD myset a b c d
SREM myset b
# Response: :1 (removed)

SREM myset x y
# Response: :0 (not found)
```

**Response:** `:count` (number removed)

---

#### SMEMBERS
Get all members of a set.

**Syntax:** `SMEMBERS key`

**Examples:**
```bash
SADD fruits apple banana cherry
SMEMBERS fruits
# Response: apple banana cherry

SMEMBERS nonexistent
# Response: (nil)
```

**Response:** Space-separated members or `(nil)`

---

#### SCARD
Get the number of members in a set.

**Syntax:** `SCARD key`

**Examples:**
```bash
SADD myset a b c
SCARD myset
# Response: :3

SCARD nonexistent
# Response: :0
```

**Response:** `:count`

---

#### SPOP
Remove and return a random member from a set.

**Syntax:** `SPOP key`

**Examples:**
```bash
SADD myset a b c
SPOP myset
# Response: b (random)

SCARD myset
# Response: :2
```

**Response:** Random member or `(nil)` if empty

---

#### SISMEMBER
Check if a value is a member of a set.

**Syntax:** `SISMEMBER key member`

**Examples:**
```bash
SADD myset apple banana
SISMEMBER myset apple
# Response: :1 (exists)

SISMEMBER myset cherry
# Response: :0 (doesn't exist)
```

**Response:** `:1` (exists) or `:0` (doesn't exist)

---

#### SUNION
Return the union of multiple sets.

**Syntax:** `SUNION key1 key2 [key...]`

**Examples:**
```bash
SADD set1 a b c
SADD set2 c d e
SUNION set1 set2
# Response: a b c d e
```

**Response:** Space-separated union members

---

#### SINTER
Return the intersection of multiple sets.

**Syntax:** `SINTER key1 key2 [key...]`

**Examples:**
```bash
SADD set1 a b c
SADD set2 b c d
SINTER set1 set2
# Response: b c
```

**Response:** Space-separated intersection members

---

#### SDIFF
Return the difference between the first set and all successive sets.

**Syntax:** `SDIFF key1 key2 [key...]`

**Examples:**
```bash
SADD set1 a b c d
SADD set2 b c
SDIFF set1 set2
# Response: a d
```

**Response:** Space-separated difference members

---

### 🗂️ Hash Map Commands

Hash commands operate on hash maps (field-value pairs within a key).

#### HSET
Set field in the hash stored at key.

**Syntax:** `HSET key field value [field value ...]`

**Examples:**
```bash
HSET user:1 name John
# Response: :1 (new field)

HSET user:1 age 30 city NYC
# Response: :2 (2 new fields)

HSET user:1 name Jane
# Response: :0 (field updated)
```

**Response:** `:count` (number of new fields added)

---

#### HGET
Get the value of a field in a hash.

**Syntax:** `HGET key field`

**Examples:**
```bash
HSET user:1 name Alice age 25
HGET user:1 name
# Response: Alice

HGET user:1 email
# Response: (nil)
```

**Response:** Field value or `(nil)`

---

#### HDEL
Delete one or more fields from a hash.

**Syntax:** `HDEL key field [field ...]`

**Examples:**
```bash
HSET user:1 name Bob age 30 city LA
HDEL user:1 age
# Response: :1 (deleted)

HDEL user:1 email phone
# Response: :0 (not found)
```

**Response:** `:count` (number deleted)

---

#### HEXISTS
Check if a field exists in a hash.

**Syntax:** `HEXISTS key field`

**Examples:**
```bash
HSET user:1 name Charlie
HEXISTS user:1 name
# Response: :1 (exists)

HEXISTS user:1 age
# Response: :0 (doesn't exist)
```

**Response:** `:1` (exists) or `:0` (doesn't exist)

---

#### HLEN
Get the number of fields in a hash.

**Syntax:** `HLEN key`

**Examples:**
```bash
HSET user:1 name Dan age 40 city SF
HLEN user:1
# Response: :3

HLEN nonexistent
# Response: :0
```

**Response:** `:count`

---

### ⏰ TTL (Time To Live) Commands

TTL commands manage key expiration.

#### EXPIRE
Set a timeout on a key (in seconds).

**Syntax:** `EXPIRE key seconds`

**Examples:**
```bash
SET session:abc token123
EXPIRE session:abc 60
# Response: :1 (expiry set)

EXPIRE nonexistent 10
# Response: :0 (key doesn't exist)
```

**Response:** `:1` (success) or `:0` (key not found)

---

#### TTL
Get the remaining time to live of a key (in seconds).

**Syntax:** `TTL key`

**Examples:**
```bash
SET mykey value
EXPIRE mykey 100
TTL mykey
# Response: :98 (approximate)

# After expiration:
TTL mykey
# Response: :-2 (key expired/doesn't exist)

SET persistent value
TTL persistent
# Response: :-1 (no expiry set)
```

**Response:**
- `:<seconds>` - Remaining TTL
- `:-1` - Key exists but has no expiry
- `:-2` - Key doesn't exist

---

## 🧪 Test Cases & Examples

Use the included Python client (`client.py`) to interactively test these commands.

### Test Case 1: Basic String Operations
```bash
SET x 10
# Response: +OK

GET x
# Response: 10

APPEND x 20
# Response: :4

GET x
# Response: 1020

STRLEN x
# Response: :4

DEL x
# Response: :1
```

---

### Test Case 2: String Increment/Decrement
```bash
SET counter 0
# Response: +OK

INCR counter
# Response: :1

INCRBY counter 10
# Response: :11

DECR counter
# Response: :10

DECRBY counter 5
# Response: :5
```

---

### Test Case 3: Multiple String Operations
```bash
MSET name Alice age 30 city NYC
# Response: +OK

MGET name age city
# Response: Alice 30 NYC

SETNX name Bob
# Response: :0 (already exists)

SETNX email alice@example.com
# Response: :1
```

---

### Test Case 4: List Operations
```bash
RPUSH tasks task1 task2 task3
# Response: :3

LPUSH tasks urgent
# Response: :4

LLEN tasks
# Response: :4

LINDEX tasks 0
# Response: urgent

LPOP tasks
# Response: urgent

RPOP tasks
# Response: task3

LPRINT tasks
# Response: task1 task2
```

---

### Test Case 5: List Sorting
```bash
RPUSH numbers 5 2 8 1 9 3
# Response: :6

LSORT numbers asc
# Response: +OK

LPRINT numbers
# Response: 1 2 3 5 8 9

LSORT numbers desc
# Response: +OK

LPRINT numbers
# Response: 9 8 5 3 2 1
```

---

### Test Case 6: Set Operations
```bash
SADD colors red green blue
# Response: :3

SCARD colors
# Response: :3

SISMEMBER colors red
# Response: :1

SISMEMBER colors yellow
# Response: :0

SPOP colors
# Response: green (random)

SMEMBERS colors
# Response: red blue
```

---

### Test Case 7: Set Operations - Union, Intersection, Difference
```bash
SADD set1 a b c d
SADD set2 c d e f

SUNION set1 set2
# Response: a b c d e f

SINTER set1 set2
# Response: c d

SDIFF set1 set2
# Response: a b
```

---

### Test Case 8: Hash Map Operations
```bash
HSET user:1 username john_doe
# Response: :1

HSET user:1 email john@example.com age 28
# Response: :2

HGET user:1 username
# Response: john_doe

HEXISTS user:1 age
# Response: :1

HLEN user:1
# Response: :3

HDEL user:1 age
# Response: :1

HLEN user:1
# Response: :2
```

---

### Test Case 9: TTL and Expiration
```bash
SET session token123
# Response: +OK

EXPIRE session 5
# Response: :1

TTL session
# Response: :4 (approximate)

GET session
# Response: token123

# Wait 6 seconds...

TTL session
# Response: :-2 (expired)

GET session
# Response: (nil)
```

---

### Test Case 10: TTL Edge Cases
```bash
SET permanent data
# Response: +OK

TTL permanent
# Response: :-1 (no expiry)

TTL nonexistent
# Response: :-2 (doesn't exist)

EXPIRE nonexistent 10
# Response: :0 (can't set expiry on missing key)
```

---

### Test Case 11: Mixed Data Types
```bash
SET mystring value
LPUSH mylist item1
SADD myset member1
HSET myhash field1 value1

GET mystring
# Response: value

LLEN mylist
# Response: :1

SCARD myset
# Response: :1

HLEN myhash
# Response: :1
```

---

### Test Case 12: Error Handling
```bash
GET nonexistent
# Response: (nil)

INVALIDCMD arg1
# Response: -ERR unknown command

SET
# Response: -ERR wrong number of arguments for SET

INCR notanumber
# Response: -ERR value is not an integer
```

---

### Test Case 13: Complex Workflow
```bash
# User session management
HSET session:abc user_id 12345
HSET session:abc username alice
HSET session:abc login_time 1640000000
EXPIRE session:abc 3600

# Shopping cart
SADD cart:user123 item_a item_b item_c
SCARD cart:user123
# Response: :3

# Activity log
LPUSH log:user123 login
LPUSH log:user123 view_product
LPUSH log:user123 add_to_cart
LLEN log:user123
# Response: :3

# Stats counter
SET visits:today 0
INCR visits:today
INCR visits:today
GET visits:today
# Response: 2
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
│   ├── parser/
│   │   └── Parser.hpp
│   ├── server/
│   │   └── Server.hpp
│   └── storage/
│       ├── RedisHashMap.hpp
│       ├── RedisObject.hpp
│       ├── StringStore.hpp
│       ├── ListStore.hpp
│       ├── SetStore.hpp
│       ├── HashMapStore.hpp
│       ├── TTLManager.hpp
│       └── murmurhash/
│           └── murmurhash3.hpp
│
├── src/
│   ├── parser/
│   │   └── Parser.cpp
│   ├── server/
│   │   └── Server.cpp
│   ├── storage/
│   │   ├── RedisHashMap.cpp
│   │   ├── RedisObject.cpp
│   │   ├── StringStore.cpp
│   │   ├── ListStore.cpp
│   │   ├── SetStore.cpp
│   │   ├── HashMapStore.cpp
│   │   ├── TTLManager.cpp
│   │   └── murmurhash/
│   │       └── murmurhash3.cpp
│   │
│   └── main.cpp
│
├── CMakeLists.txt
├── client.py
└── README.md

```

## 🤝 Contributing

This is an academic project. For educational purposes, feel free to fork and experiment!
If you find it useful, please consider giving it a star.

## 📄 License

This project is part of an academic submission. Please respect academic integrity policies if using this code for educational purposes.

## 🙏 Acknowledgments

- Inspired by Redis architecture
- MurmurHash3 algorithm by Austin Appleby
- Course instructors and teaching assistants

---

**Repository**: [github.com/murtaza335/in-memory-cache-db-cpp](https://github.com/murtaza335/in-memory-cache-db-cpp)

Built with ❤️ for CS-250 | Data Structures & Algorithms
