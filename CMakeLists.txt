cmake_minimum_required(VERSION 3.10)
project(DSA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect all .cpp files recursively
file(GLOB_RECURSE SOURCES
    src/**/*.cpp   # use ** to include subfolders
    main.cpp
)

add_executable(DSA ${SOURCES})
