#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include "Graph.h"

#include <cassert>
#include <unordered_set>

Graph::Graph(const std::vector<Node> &points) {
    _size = points.size();
    _matrix = std::vector<std::vector<double>>(_size, std::vector<double>(_size));
    _nodes = points;
    for (int i = 0; i < _size; i++)
        for (int j = i; j < _size; j++)
            _matrix[i][j] = _matrix[j][i] = points[i].distance(points[j]);
}

std::string Graph::to_string() const {
    std::stringstream ss;
    ss << _size << "\n";
    for (const auto &v: _matrix) {
        for (double x: v) ss << x << " ";
        ss << "\n";
    }
    return ss.str();
}

std::vector<int> Graph::getRandomPath(std::mt19937& eng) const {
    std::vector<int> permutation(_size);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::shuffle(permutation.begin(), permutation.end(), eng);
    return permutation;
}


std::vector<int> Graph::getRandomPathFi(std::mt19937 &eng) const {
    std::uniform_int_distribution<> distr(0, _size - 1);
    //
    const int x = {distr(eng)};
    std::vector<int> tour = {x};
    std::vector<bool> visited(_size, false);
    visited[x] = true;

    int farthestPoint = 0;
    double maxDistance = 0.0;

    for (int i = 0; i < _size; ++i) {
        double distance = _matrix[x][i];
        if (distance > maxDistance && i!=x) {
            maxDistance = distance;
            farthestPoint = i;
        }
    }

    tour.push_back(farthestPoint);
    visited[farthestPoint] = true;

    // while there are unvisited points
    while (tour.size() < _size) {
        double maxDistance = -1.0;
        int nextPoint = -1;
        int bestPosition = -1;

        // find the farthest unvisited point
        for (int i = 0; i < _size; ++i) {
            if (!visited[i]) {
                // find the best position to insert this point
                for (int j = 0; j < tour.size(); ++j) {
                    int current = tour[j];
                    int next = tour[(j + 1) % tour.size()]; // wrap around to the start

                    double distance = _matrix[i][current] +
                                      _matrix[i][next] -
                                      _matrix[current][next];

                    if (distance > maxDistance) {
                        maxDistance = distance;
                        nextPoint = i;
                        bestPosition = j + 1; // insert after current
                    }
                }
            }
        }

        // insert the farthest point at the best position
        tour.insert(tour.begin() + bestPosition, nextPoint);
        visited[nextPoint] = true;
    }
    const std::unordered_set<int> part_path_set(tour.cbegin(), tour.cend());
    assert(part_path_set.size() == _size);
    assert(part_path_set.size() == _size);
    return tour;
}
    std::vector<int> Graph::getRandomPathNn(std::mt19937 &eng) const {
    std::uniform_int_distribution<> distr(0, _size - 1);

    const int x = {distr(eng)};
    std::vector<int> partialPath{x};
    partialPath.reserve(_size);
    int y = x;
    std::vector<bool> visited(_size, false);
    visited[y] = true;

    while (partialPath.size() != _size) {
        const std::vector<double>& r = _matrix[y];
        double minDistance = std::numeric_limits<double>::max();
        int argminDistance = y;
        for (int i = 0; i < _size; ++i) {
            if (i != y && r[i] < minDistance && !visited[i]) {
                minDistance = r[i];
                argminDistance = i;
            }
        }
        assert(argminDistance != y);
        partialPath.push_back(argminDistance);
        visited[argminDistance] = true;
        y=argminDistance;
    }
    const std::unordered_set<int> part_path_set(partialPath.cbegin(), partialPath.cend());
    assert(part_path_set.size() == _size);
    return partialPath;
}

std::size_t Graph::size() const {
    return _size;
}

double Graph::getEdgeWeight(int from, int to) const {
    if (from < 0 || from >= _size || to < 0 || to >= _size) {
        throw std::invalid_argument("Edge out of bounds");
    }
    return _matrix[from][to];
}

Graph::operator std::vector<std::vector<double>>&() {
    return _matrix;
}

Graph::operator const std::vector<std::vector<double>>&() const {
    return _matrix;
}
