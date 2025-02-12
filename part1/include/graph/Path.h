#ifndef PATH_H
#define PATH_H

#include <algorithm>
#include <iostream>
#include <vector>

class Path {
    std::vector<int> _route;

public:


    const int& operator[](std::size_t index) const {
        return _route[index];
    }

    Path(const std::vector<std::vector<int>>& pairs): _route{pairs[0]} {
        _route.reserve(pairs.size());

        while (_route.size() < pairs.size()) {
            const int last = _route.back();
            std::vector<std::vector<int>>::const_iterator it = std::find_if(
                pairs.cbegin(), pairs.cend(), [last](const std::vector<int>& pair) {
                return !pair.empty() && pair[0] == last;
            });
            _route.push_back((*it)[1]);
        }
    }

    const int* cbegin() const;
    const int* cend() const;


    std::size_t size() const {
        return _route.size();
    }


    friend std::ostream& operator<<(std::ostream& os, const Path & route) {
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