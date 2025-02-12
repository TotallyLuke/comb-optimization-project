#ifndef TOUR_H
#define TOUR_H

#include <algorithm>
#include "FitnessEvaluator.h"

class Tour {
    std::vector<int> _route;
    double _score;

public:
    double score() const {
        return _score;
    }

    static FitnessEvaluator _fitnessEvaluator;

    Tour() : _score(0) {}
    Tour (const std::vector<int>& v) : _route(v) {
//    assert(_fitnessEvaluator );
        _score = _fitnessEvaluator.getFitness(_route);
    }
    // Tour& operator=(const Tour& tour) = delete;
    const int& operator[](std::size_t index) const {
        return _route[index];
    }
    const int* cbegin() const;
    const int* cend() const;


    std::size_t size() const {
        return _route.size();
    }

    void swap_indexes(int i, int j) {
        std::swap(_route[i], _route[j]);
        _score = _fitnessEvaluator.getFitness(_route);
    }

    void reverse_sub_path(int from, int to) {
        std::reverse(_route.begin() + from, _route.begin() + to);
        _score = _fitnessEvaluator.getFitness(_route);
    };


    friend std::ostream& operator<<(std::ostream& os, const Tour & route) {
        os << "[";
        const std::size_t length = route.size()-1;
        for (size_t i = 0; i < length; ++i) {
            os << route[i];
            os << ", ";
        }

        os << route[length] << "]";
        return os;
    }

    explicit operator const std::vector<int>&() const {
        return _route;
    }

};

#endif