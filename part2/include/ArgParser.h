#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>

struct CommandLineOptions {
    std::string filename;
    int timeout_ms;
    double mutation_probability;
    double crossover_rate;
    double elitism_rate;
    int mu;
    int lambda;
    int k;
    int max_gen_no_improvement;
    int max_gen;
    bool best;
};
CommandLineOptions parse_arguments(int argc, char* argv[]);


#endif // ARG_PARSER_H
