cmake_minimum_required(VERSION 3.10)
project(assignment2)

set(CMAKE_CXX_STANDARD 14)
include_directories( . ./include/graph/ ./include)
add_executable(assignment2 main.cpp include/Solver.cpp include/ArgParser.cpp include/graph/Graph.cpp
        include/graph/Tour.cpp)