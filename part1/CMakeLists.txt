
# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(part1 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define paths for CPLEX
set(CPLEX_DIR /opt/ibm/ILOG/CPLEX_Studio2211/cplex)
set(CPLEX_INCLUDE_DIR ${CPLEX_DIR}/include)
set(CPLEX_LIB_DIR ${CPLEX_DIR}/lib/x86-64_linux/static_pic)

# Include directories
include_directories(${CPLEX_INCLUDE_DIR} ./include)

# Source file
set(SOURCE_FILES main.cpp)

# Target executable
add_executable(main ${SOURCE_FILES})

# Link directories and libraries
target_link_directories(main PRIVATE ${CPLEX_LIB_DIR})
target_link_libraries(main PRIVATE cplex m pthread dl)

# Compiler flags
target_compile_options(main PRIVATE -m64 -Wall -O2 -fPIC)
target_link_options(main PRIVATE -g -Wall -O2)

# Add a custom clean target
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "main.o;main")
