#pragma once

#include <string>
#include <vector>
#include <random>
#include "Node.h"

class Graph {
    std::size_t _size;
    std::vector<Node> _nodes;
    std::vector<std::vector<double>> _matrix;

public:
    explicit Graph(const std::vector<Node> &points);

    std::string to_string() const;

    std::size_t size() const;

    double getEdgeWeight(int from, int to) const;

    std::vector<int> getRandomPath(std::mt19937 &eng) const;

    std::vector<int> getRandomPathNn(std::mt19937 &eng) const;
    std::vector<int> getRandomPathFi(std::mt19937 &eng) const;

    explicit operator std::vector<std::vector<double>>&();

    explicit operator const std::vector<std::vector<double>>&() const;

};
