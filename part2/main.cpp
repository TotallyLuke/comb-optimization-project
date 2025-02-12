#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

#include <memory>
#include "TSPGraphReader.h"

#include "Graph.h"
#include "FitnessEvaluator.h"
#include "Solver.h"
#include "ArgParser.h"

#include "Tour.h"




std::vector<Tour> get_population(const Graph *g, const int population_size) {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::vector<Tour> population;

    // std::cout << "generateRouteNN(g, distr(eng)).score() "<< std::endl;
    // std::cout << Tour{g->getRandomPathNn(eng)}.score() << std::endl;
    while (population.size() < population_size) {
        population.push_back(g->getRandomPathFi(eng));
        for (int i = 0; i < 20 && population.size() < population_size; ++i) {
            population.emplace_back(g->getRandomPath(eng));
        }
        std::shuffle(population.begin(), population.end(), eng);
    }
    return population;
}

void invoke_solvers(const Graph *g, const double mutation_rate, const int POP_SIZE, const int lambda,
                    const double crossover_rate, const double elitism_rate, const int timeout_ms,
                    const int max_gen_no_improvement, const int max_gen, const int k, const bool best) {
    const FitnessEvaluator fitEval{g};
    Tour::_fitnessEvaluator = fitEval;

    const CrossoverOperator *o = new OrderCrossover(),
            *p = new PMXCrossover();
    const MutationOperator *psm = new PSMutation(mutation_rate),
            *rsm = new RSMutation(mutation_rate);
    const SelectionOperator *ro = new RankingSelection(lambda),
            *to = new TournamentSelection(lambda, k);

    std::vector<Tour> population{get_population(g, POP_SIZE)};
    std::cout << "initial_best " << (std::min_element(population.begin(), population.end(),
                                                      [&](const Tour &a, const Tour &b) {
                                                          return a.score() < b.score();
                                                      }))->score() << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    if (!best) {
        for (const SelectionOperator *sel: {ro, to})
            for (const MutationOperator *mut: {psm, rsm})
                for (const CrossoverOperator *xover: {o, p}) {
                    Solver s = Solver(fitEval, xover, mut, sel);
                    const Tour t = s.solve(population, crossover_rate, timeout_ms, max_gen_no_improvement, max_gen, elitism_rate);
                    std::cout << xover->describe() << ", " << mut->describe() << ", " << sel->describe() << std::endl;
                    std::cout << "cost: " << t.score() << std::endl;
                }
    } else {
        Solver s = Solver(fitEval, o, rsm, ro);
        const Tour t = s.solve(population, crossover_rate, timeout_ms, max_gen_no_improvement, max_gen, elitism_rate);
        std::cout << o->describe() << ", " << rsm->describe() << ", " << ro->describe() << std::endl;
        std::cout << "cost: " << t.score() << std::endl;
    }

        const std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
        std::cout << "Wall clock time s1-s8: " << duration.count() << " seconds" << std::endl;

        delete o, delete p, delete psm, delete rsm, delete ro, delete to;
    }

    int main(int argc, char *argv[]) {
        CommandLineOptions cli{parse_arguments(argc, argv)};
        try {
            const TSPGraphReader tspReader{};
            std::unique_ptr<Graph> g{new Graph(tspReader.read(cli.filename))};

            invoke_solvers(g.get(), cli.mutation_probability, cli.mu, cli.lambda, cli.crossover_rate,
                cli.elitism_rate, cli.timeout_ms, cli.max_gen_no_improvement, cli.max_gen, cli.k, cli.best);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }

        return 0;
    }

