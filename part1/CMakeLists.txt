
# minimum cmake version
cmake_minimum_required(VERSION 3.10)

# project name and language
project(part1 LANGUAGES CXX)

# set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# define paths for CPLEX
set(CPLEX_DIR /opt/ibm/ILOG/CPLEX_Studio2211/cplex)
set(CPLEX_INCLUDE_DIR ${CPLEX_DIR}/include)
set(CPLEX_LIB_DIR ${CPLEX_DIR}/lib/x86-64_linux/static_pic)

# include directories
include_directories(${CPLEX_INCLUDE_DIR} ./include ./include/graph)

# source file
set(SOURCE_FILES main.cpp)

# target executable
add_executable(assignment1 ${SOURCE_FILES}  ./include/graph/Graph.cpp )

# link directories and libraries
target_link_directories(assignment1 PRIVATE ${CPLEX_LIB_DIR})
target_link_libraries(assignment1 PRIVATE cplex m pthread dl)

# compiler flags
target_compile_options(assignment1 PRIVATE -m64 -Wall -O2 -fPIC -Wno-format-truncation -Wno-sign-compare)
target_link_options(assignment1 PRIVATE -g -Wall -O2)

# add a custom clean target
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "main.o;main")
