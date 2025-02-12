#include "ArgParser.h"
#include <iostream>
#include <cstdlib> // for std::atoi and std::atof

CommandLineOptions parse_arguments(int argc, char* argv[]) {
    CommandLineOptions options;
    options.timeout_ms = 1800000;
    options.mutation_probability = 0.01;
    options.crossover_rate = 0.7;
    options.elitism_rate = 0.0;
    options.mu = 100;
    options.lambda = 300;
    options.k = 8;
    options.max_gen_no_improvement = 100;
    options.max_gen = 400;
    options.best = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-f" || arg == "--filename") {
            if (i + 1 < argc) {
                options.filename = argv[++i];
            } else {
                std::cerr << "Error: No filename provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-t" || arg == "--timeout-ms") {
            if (i + 1 < argc) {
                options.timeout_ms = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No timeout provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-m" || arg == "--mutation-probability") {
            if (i + 1 < argc) {
                options.mutation_probability = std::atof(argv[++i]);
            } else {
                std::cerr << "Error: No mutation probability provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-c" || arg == "--crossover-rate") {
            if (i + 1 < argc) {
                options.crossover_rate = std::atof(argv[++i]);
            } else {
                std::cerr << "Error: No crossover rate provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-e" || arg == "--elitism-rate") {
            if (i + 1 < argc) {
                options.elitism_rate = std::atof(argv[++i]);
            } else {
                std::cerr << "Error: No elitism rate provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "--mu" || arg == "--population-size") {
            if (i + 1 < argc) {
                options.mu = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No population size provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "--lambda") {
            if (i + 1 < argc) {
                options.lambda = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No offspring size provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-k" || arg == "--tournament-size") {
            if (i + 1 < argc) {
                options.k = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No tournament size provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-N" || arg == "--max-gen-no-improvement") {
            if (i + 1 < argc) {
                options.max_gen_no_improvement = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No max generations without improvement provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-M" || arg == "--max-gen") {
            if (i + 1 < argc) {
                options.max_gen = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No max generations provided after " << arg << std::endl;
                exit(1);
            }
        }
        else if (arg == "-b" || arg == "--best") {
            if (i + 1 < argc) {
                if (std::string(argv[++i]) == "true") {
                    options.best = true;
                } else if (std::string(argv[i]) == "false") {
                    options.best = false;
                } else {
                    std::cerr << "Error: Invalid value for " << arg << ". Expected 'true' or 'false', but got " << argv[i] << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: No value provided after " << arg << std::endl;
                exit(1);
            }
        }
        else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -f, --filename [FILE]                Name of the input TSP instance file\n"
                      << "  -t, --timeout-ms [TIMEOUT]           Timeout expressed in milliseconds\n"
                      << "  -m, --mutation-probability [PROBABILITY]  Probability that a mutation occurs in the genetic algorithm\n"
                      << "  -c, --crossover-rate [RATE]          Probability that two selected solutions are mated to create a new offspring\n"
                      << "  -e, --elitism-rate [RATE]           Elitism rate\n"
                      << "  --mu [SIZE]                           Size of the population pool\n"
                      << "  --lambda [SIZE]                       Size of the offspring pool before being pruned\n"
                      << "  -k, --tournament-size [SIZE]          Size of the tournament selection\n"
                      << "  -N, --max-gen-no-improvement [NUMBER] Maximum number of generations without solution improvement\n"
                      << "  -M, --max-gen [NUMBER]                Maximum number of generations\n"
                      << "  -b, --best [BOOL]                     If true, runs the algorithm once with the best combination of genetic operators; if false, runs it 8 times with different combinations.\n";

            exit(0);
        }
    }
    if (options.filename.empty()) {
        std::cerr << "Error: A filename must be specified using -f or --filename\n";
        exit(1);
    }
    return options;
}
