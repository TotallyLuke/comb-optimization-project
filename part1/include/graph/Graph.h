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

    static int getSingleCoord(int num_vertex, int i, int j) {
        // assert(i!=j);
        return i * num_vertex + j - (i + (j > i));
    }

    static void getDoubleCoord(const int num_vertex, const int x, int &i, int &j) {
        // assert(x<num_vertex*(num_vertex-1));
        // int count = 0;
        // for (int h = 0; h < num_vertex; ++h) {
        //     for (int v = 0; v < num_vertex; ++v) {
        //         if (h != v) {
        //             if (count == x) {
        //                 i = h;
        //                 j = v;
        //                 return;
        //             }
        //             ++count;
        //         }
        //     }
        // }
        i = static_cast<int>(x/(num_vertex-1));
        j=static_cast<int>(x%(num_vertex-1) + (x%(num_vertex-1) >= i));


    }
};
