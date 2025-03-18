cmake_minimum_required(VERSION 3.12)
project(binary_reader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python (only if Python module is built)
option(BUILD_PYTHON_MODULE "Build Python module" OFF)

if(BUILD_PYTHON_MODULE)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    find_package(pybind11 REQUIRED)
endif()

# Common source files
set(SOURCES
    src/BinaryReader.cc
)

set(HEADERS
    include/BinaryReader.h
    include/ParticleData.h
)

# ========== Build Standalone Executable ==========
add_executable(binary_reader_exec src/main.cc ${SOURCES})
target_include_directories(binary_reader_exec PRIVATE include)

find_package(Threads REQUIRED)
target_link_libraries(binary_reader_exec PRIVATE Threads::Threads)

# ========== Optional: Build Python Module ==========
if(BUILD_PYTHON_MODULE)
    pybind11_add_module(binary_reader src/pybinds.cc ${SOURCES})
    target_include_directories(binary_reader PRIVATE include)
    target_link_libraries(binary_reader PRIVATE Python3::Python)
endif()
