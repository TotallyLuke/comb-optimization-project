#include "../Solver.h"

Tour PMXCrossover::applyCrossover(const Tour &parent1, const Tour &parent2, int a, int b) const {
    std::vector<int> result(parent1.size(), -1);
    std::copy(parent1.cbegin()+a, parent1.cbegin()+b, result.begin()+a);

    // assumes Tour contains a Vector
    for (int i = a; i < b; ++i) {
        if (std::find(parent1.cbegin()+a, parent1.cbegin()+b, parent2[i]) != parent1.cbegin()+b) {
            continue;   // skip the insertion if node to insert in result is already in result
        }

        std::ptrdiff_t pd = 0;
        int el = parent1[i];
        do {
            const int* it = std::find(parent2.cbegin(), parent2.cend(), el);
            pd = std::distance(parent2.cbegin(), it);
            el = parent1[pd];
        } while (pd >= a && pd < b);
        result[pd] = parent2[i];
        assert(pd < result.size());
    }

    for (int i = 0; i < result.size(); ++i) {
        if (result[i] == -1) {
            result[i] = parent2[i];
        }
    }
    assert(std::find(result.cbegin(), result.cend(), -1) == result.cend());

    return Tour{result};
}



Tour OrderCrossover::applyCrossover(const Tour &parent1, const Tour &parent2, const int a, const int b) const {

    std::function<bool(int)> exists_in_range_ab = [parent1,a,b](const int x) -> bool {
        return std::find(parent1.cbegin() + a, parent1.cbegin() + b, x) != parent1.cbegin() + b;
    };

    std::list<int> parent2_l{parent2.cbegin(), parent2.cend()};
    parent2_l.remove_if(exists_in_range_ab);

    std::vector<int> result(parent1.size(), -1);
    std::copy(parent1.cbegin()+a, parent1.cbegin()+b, result.begin()+a);
    for (int& elem : result) {
        if (elem == -1) {
            elem = parent2_l.front();
            parent2_l.pop_front();
        }
    }

    // Assert that parent2_l is empty and result does not contain -1
    assert(parent2_l.empty());
    assert(std::find(result.cbegin(), result.cend(), -1) == result.cend());

    return Tour{result};
}

