#pragma once
struct Node {
    double x, y;
    Node(const double x, const double y): x(x), y(y) {}
    double distance(const Node &n) const {
        return std::sqrt(
                (x - n.x) * (x - n.x) +
                (y - n.y) * (y - n.y)
        );
    }
};
