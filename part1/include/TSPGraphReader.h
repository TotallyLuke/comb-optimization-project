#pragma once
#include "Node.h"
#include <sstream>
#include <fstream>

#include <stdexcept>
#include <memory>

class TSPGraphReader  {
public:
    std::vector<Node> read(const std::string &filepath) const  {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filepath);
        }

        std::string line;
        std::vector<Node> nodes;
        bool nodeSection = false;

        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty()) {
                continue;
            }
            if (line == "NODE_COORD_SECTION") {
                nodeSection = true;
                continue;
            }

            if (line == "EOF" || (nodeSection && line.empty())) {
                break;
            }

            if (nodeSection) {
                std::istringstream iss(line);
                int id;
                double x, y;
                if (!(iss >> id >> x >> y)) {
                    throw std::runtime_error("Invalid format in NODE_COORD_SECTION");
                }
                nodes.emplace_back(x, y);
            }
        }

        file.close();

        if (nodes.empty()) {
            throw std::runtime_error("No nodes found in the file.");
        }
        return nodes;
    }

private:
    static std::string trim(const std::string &str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, last - first + 1);
    }
};
