#include "Tour.h"

FitnessEvaluator Tour::_fitnessEvaluator(nullptr);

const int * Tour::cbegin() const {
    return _route.data();
}

const int * Tour::cend() const {
    return _route.data()+_route.size();
}
