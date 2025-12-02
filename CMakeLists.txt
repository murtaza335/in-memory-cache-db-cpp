cmake_minimum_required(VERSION 3.10)
project(DSA)

# 1. Tell CMake where the header files (.hpp) are
include_directories(include)

# 2. Create the executable using main.cpp AND the murmurhash implementation
add_executable(main 
    src/main.cpp 
    src/storage/murmurhash/murmurhash3.cpp
    src/server/server.cpp
)

if (WIN32)
    target_link_libraries(main PRIVATE ws2_32)
endif()
