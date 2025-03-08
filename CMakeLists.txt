# CMake minimum version
cmake_minimum_required(VERSION 3.12)

# Project name
project(BinaryReader)

# Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cc
    src/BinaryReader.cc
)

# Header files
set(HEADERS
    include/BinaryReader.h
)

add_definitions(-DEXTENDED)
set(CMAKE_BUILD_TYPE Debug)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE EXTENDED)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    # Add additional include directories if needed
)

# Set include directories

# Find and link the threading library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -W  -pthread"
)

