cmake_minimum_required(VERSION 3.10)
project(DSA)

# 1. Tell CMake where the header files (.hpp) are
include_directories(include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# 2. Create the executable using main.cpp AND the murmurhash implementation
add_executable(main 
    src/main.cpp 
    src/storage/murmurhash/murmurhash3.cpp
    src/storage/liststore.cpp
    src/storage/stringstore.cpp
    src/storage/RedisHashMap.cpp
    src/storage/RedisObject.cpp
    src/server/server.cpp
    src/parser/parser.cpp
    src/storage/RedisDictionaryHashMap.cpp
    src/storage/RedisSets.cpp
)

if (WIN32)
    target_link_libraries(main PRIVATE ws2_32)
endif()
