#ifndef FITNESSEVALUATOR_H
#define FITNESSEVALUATOR_H


#include <iostream>
#include <stdexcept>
#include "Graph.h"

class FitnessEvaluator {
    const Graph* _g;

public:
    explicit FitnessEvaluator(const Graph* graph)
        : _g(graph) {}

    /**
     * @brief Calculates the fitness value of the tour.
     *
     * The fitness value is the total weight of the edges traversed in the order
     * specified by the path.
     *
     * @param path A constant reference to a Tour
     *
     * @return The total fitness value of the Tour
     *
     * @throws std::invalid_argument If the size of the path does not match the
     *                                size of a tour in the graph.
    */
    double getFitness(const std::vector<int>& path) const {
        if (path.size() != _g->size()) {
            throw std::invalid_argument("Path size must match the size of the adjacency matrix.");
        }

        double res = 0.0;

        for (std::size_t i = 0; i < _g->size() - 1; ++i) {
            res += _g->getEdgeWeight(path[i], path[i + 1]);
        }
        return _g->getEdgeWeight(path[_g->size() - 1], path[0]) + res;
    }
};



#endif