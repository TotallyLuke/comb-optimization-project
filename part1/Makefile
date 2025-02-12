# Compiler and flags
CXX := g++
CXXFLAGS := -m64 -Wall -O2 -fPIC -Wno-format-truncation -Wno-sign-compare -std=c++14
LDFLAGS := -g -Wall -O2
LIBS := -lcplex -lm -lpthread -ldl

# Paths
CPLEX_DIR := /opt/ibm/ILOG/CPLEX_Studio2211/cplex
CPLEX_INCLUDE_DIR := $(CPLEX_DIR)/include
CPLEX_LIB_DIR := $(CPLEX_DIR)/lib/x86-64_linux/static_pic
INCLUDE_DIRS := -I$(CPLEX_INCLUDE_DIR) -I./include -I./include/graph
LIB_DIRS := -L$(CPLEX_LIB_DIR)

# Source files
SRCS := main.cpp ./include/graph/Graph.cpp
OBJS := $(SRCS:.cpp=.o)

# Target executable
TARGET := assignment1

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)