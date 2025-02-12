# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -O2 -std=c++14
LDFLAGS := 

# Include directories
INCLUDE_DIRS := -I. -I./include -I./include/graph

# Source files
SRCS := main.cpp include/Solver.cpp include/ArgParser.cpp include/graph/Graph.cpp include/graph/Tour.cpp
OBJS := $(SRCS:.cpp=.o)

# Target executable
TARGET := assignment2

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)