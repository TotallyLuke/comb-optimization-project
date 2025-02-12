#include "Tour.h"


const int * Path::cbegin() const {
    return _route.data();
}

const int * Path::cend() const {
    return _route.data()+_route.size();
}
