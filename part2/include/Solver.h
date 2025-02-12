#ifndef SOLVER_H
#define SOLVER_H

#include <algorithm>
#include <cassert>
#include <functional>
#include <list>
#include <random>
#include <unordered_set>
#include <vector>
#include "Tour.h"
#include "FitnessEvaluator.h"

// Abstract class for Crossover Operator
class CrossoverOperator {
protected:
/**
 * @brief Generates a random integer within a specified range.
 *
 * This function generates a random integer value,
 * uniformly distributed on the closed interval [min,max].
 *
 * @param min The minimum value of the range (inclusive).
 * @param max The maximum value of the range (inclusive).
 * @return A random integer between min and max, inclusive.
 *
 * @note This function is static and uses static variables to maintain
 *       the random number generator state across multiple calls.
 */
    static int dice(const int min, const int max) {
        static std::uniform_int_distribution<> distr{min, max};
        static std::random_device device;
        static std::mt19937 engine{device()};
        return distr(engine);
    }

public:
    virtual Tour crossover(const Tour& parent1, const Tour& parent2) const {
        int a = dice(0, parent1.size());
        int b = dice(0, parent1.size());
        if (a > b) {
            std::swap(a, b);
        }
        return applyCrossover(parent1, parent2, a, b);
    }
    virtual ~CrossoverOperator() = default;
    virtual std::string describe() const = 0;

protected:
    virtual Tour applyCrossover(const Tour &parent1, const Tour &parent2, int a, int b) const = 0;

};

class PMXCrossover : public CrossoverOperator {
protected:
    Tour applyCrossover(const Tour &parent1, const Tour &parent2, int a, int b) const override;

public:
    std::string describe() const override {
        return "PM crossover";
    }

};

class OrderCrossover : public CrossoverOperator {
protected:
    Tour applyCrossover(const Tour &parent1, const Tour &parent2, int a, int b) const override;
public:
    std::string describe() const override {
        return "Order crossover";
    }

};

class TestOrderCrossoverOperator : public OrderCrossover {
    public:
    Tour crossover(const Tour& parent1, const Tour& parent2) const override {
        std::cout << "TestOrderCrossoverOperator::crossover" << std::endl;
        std::cout << "parent1: " << parent1 << std::endl;
        std::cout << "parent2: " << parent2 << std::endl;
        const Tour result = applyCrossover(parent1, parent2, 2, 4);
        assert(parent1[2] == parent2[2] && parent1[3] == parent2[3]);
        return result;
    }
};





// Abstract class for Mutation Operator
class MutationOperator {
public:
    virtual void mutate(Tour& tour) const = 0;
    virtual std::string describe() const = 0;
    virtual ~MutationOperator() = default;

    explicit MutationOperator(const double mut_rate=0.01): _mutation_rate(mut_rate){}

protected:
    static int dice(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    static std::mt19937& get_rand_engine() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
    double _mutation_rate;
};

class PSMutation : public MutationOperator {
public:
    void mutate(Tour& tour) const override {
        if (tour.size() < 2) return;

        int a = dice(0, tour.size());
        int b = dice(0, tour.size());
        if (a > b) {
            std::swap(a, b);
        }
        std::uniform_real_distribution<> prob_dist(0.0, 1.0);
        std::mt19937 g = get_rand_engine();
        if (prob_dist(g) < _mutation_rate && a<b) {
            tour.swap_indexes(a, b);
        }
    }
    std::string describe() const override {
        return "PSM mutation";
    }

    explicit PSMutation(const double mut_rate=0.01): MutationOperator(mut_rate){}

};
class RSMutation : public MutationOperator {
public:
    void mutate(Tour& tour) const override {
        if (tour.size() < 2) return;

        int a = dice(0, tour.size());
        int b = dice(0, tour.size());
        if (a > b) {
            std::swap(a, b);
        }
        std::uniform_real_distribution<> prob_dist(0.0, 1.0);
        std::mt19937 g = get_rand_engine();
        if (prob_dist(g) < _mutation_rate && a<b) {
            tour.reverse_sub_path(a, b);
        }
    }
    std::string describe() const override {
        return "RSM mutation";
    }
    explicit RSMutation(const double mut_rate=0.01): MutationOperator(mut_rate){}
};

class SelectionOperator {
protected:
    int _num_selections;
public:
    virtual std::vector<Tour *> select(std::vector<Tour>& population, const FitnessEvaluator& f) const  = 0;
    explicit SelectionOperator(const int num_selection): _num_selections(num_selection) {}
    virtual ~SelectionOperator() = default;
    virtual std::string describe() const = 0;

};

class TournamentSelection : public SelectionOperator {
public:
    explicit TournamentSelection(const int num_selection, const int tournament_size) : SelectionOperator(num_selection) ,_tournament_size(tournament_size) {}

    std::vector<Tour *> select(std::vector<Tour>& population, const FitnessEvaluator& f) const override {
        std::vector<Tour *> selectedParents;
        selectedParents.reserve(_num_selections);
        // random number generator setup
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, population.size() - 1);

        // perform selection
        while (selectedParents.size() < _num_selections) {
            std::vector<Tour*> tournamentGroup;
            tournamentGroup.reserve(_tournament_size);

            while (tournamentGroup.size() < _tournament_size) {
                const int randIndex = dist(gen);
                tournamentGroup.push_back(population.data() + randIndex);
            }

            auto bestIt = std::min_element(
                tournamentGroup.begin(), tournamentGroup.end(),
                [](const Tour* a, const Tour* b) -> bool {
                    return a->score() < b->score();
                });
                assert(tournamentGroup[0]->score() == f.getFitness(std::vector<int>{*tournamentGroup[0]}));
            selectedParents.push_back(*bestIt);
        }

        return selectedParents;
    }
    std::string describe() const override {
        return "Tournament selection";
    }
private:
    int _tournament_size;
};


class RankingSelection : public SelectionOperator {
public:
    explicit RankingSelection(const int num_selections) : SelectionOperator(num_selections) {}

    std::vector<Tour *> select(std::vector<Tour>& population,
                              const FitnessEvaluator& f) const override {

        // sort individuals by fitness (descending order)
        std::sort(population.begin(), population.end(),
                  [](const Tour& a, const Tour& b) -> bool {
                      return a.score() < b.score();
                  });
        // assert(population.cbegin()->score() >= (*(population.cend()-1)).score());
        std::vector<Tour *> selectedParents;
        selectedParents.reserve(_num_selections);

        std::vector<double> probabilities;
        probabilities.reserve(population.size());

        const double totalRankSum = population.size() * (population.size()+1) / 2.0;
        for (int i = 0; i < population.size(); ++i) {
            assert((population.size() - i) != 0);
            probabilities.push_back((population.size() - i) / totalRankSum);
        }
        assert(population[0].score() <= population[population.size()-1].score()
            && probabilities[0] >= probabilities[population.size()-1]
        );

        std::random_device rd;
        std::mt19937 gen(rd());

        while (selectedParents.size() < _num_selections) {
            std::uniform_real_distribution<> prob_dist(0.0, 1.0);
            const double r = prob_dist(gen);
            double cumulativeProbability = 0.0;

            for (int i = 0; i < population.size(); ++i) {

                cumulativeProbability += probabilities[i];
                if (r <= cumulativeProbability) {
                    selectedParents.push_back(population.data()+i);
                    break;
                }
                assert(cumulativeProbability <= 1.0);
            }
        }
        return selectedParents;
    }
    std::string describe() const override {
        return "Ranking selection";
    }
};


class Solver {
    const FitnessEvaluator& _fitness_evaluator;
    const CrossoverOperator* _crossover_op;
    const MutationOperator* _mutation_op;
    const SelectionOperator* _selection_op;
    int _max_iterations;
    int _generation_no_impr{10};
    int _generation_index{0};
    int _stagnation_counter{0};


protected:
    static void test_value(const Tour& r) {
        const int *first = r.cbegin(), *last = r.cend();
        const std::unordered_set<int> us(first, last);
        for (int i = 0; i < r.size(); ++first, ++i) {
            assert(us.find(i) != us.cend()); // check if the element is in the set
            assert(*first < r.size() && *first >= 0);
        }
        assert(us.size() == r.size());
    }

public:
    Solver(const FitnessEvaluator& fitness_evaluator,
           const CrossoverOperator* crossover_operator,
           const MutationOperator* mutation_operator,
           const SelectionOperator* selection_operator
    ):
    _fitness_evaluator(fitness_evaluator),
    _crossover_op(crossover_operator),
    _mutation_op(mutation_operator),
    _selection_op(selection_operator)
    {}

    /**
     * @brief Solves the TSP using a genetic algorithm
     * Assume that user has already provided fitness function,
     *  and xover, mutation, selection operators
     *
     * @return an (approximate) solution to the Travelling Salesperson Problem
     */
    Tour solve(const std::vector<Tour> &_population,
        const double crossover_rate,
        const int timeout_ms,
        const int max_gen_no_improvement,
        const int max_gen,
        const double elitism_rate = 0.05) {
        assert(_selection_op != nullptr);

        Tour current_best_tour = *std::min_element(_population.begin(), _population.end(),
        [&](const Tour& a, const Tour& b) -> bool { return a.score() < b.score(); }
        );


        const int mu = _population.size();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> prob_dist(0.0, 1.0);

        std::vector<Tour> parents = _population;
        for (int i = 0; i < max_gen; ++i) {
            std::vector<Tour *> pop = _selection_op->select(parents, _fitness_evaluator);


            std::vector<Tour> offspring;

            for (int j = 0; j < pop.size()-1; j+=2) {  // pop.size()-1 to make resilient to odd sizes
                if (prob_dist(gen) < crossover_rate) {
                    offspring.push_back(_crossover_op->crossover(*pop[j], *pop[j+1]));
                } else {
                    offspring.push_back(*pop[j]);
                }
                if (prob_dist(gen) < crossover_rate) {
                    offspring.push_back(_crossover_op->crossover(*pop[j+1], *pop[j]));
                } else {
                    offspring.push_back(*pop[j+1]);
                }
            }

            assert(_mutation_op != nullptr);
            for (Tour& r : offspring) {
                _mutation_op->mutate(r);
            }
            const int elitism_counter = static_cast<int>(elitism_rate*mu);
            std::partial_sort(parents.begin(), parents.begin() + elitism_counter, parents.end(),
                              [&](const Tour& a, const Tour& b) -> bool {
                                  return a.score() < b.score();
                              });

            std::partial_sort(offspring.begin(), offspring.begin() + mu - elitism_counter, offspring.end(),
                              [&](const Tour& a, const Tour& b) -> bool {
                                  return a.score() < b.score();
                              });
            offspring.resize(mu - elitism_counter);
            for (int j = 0; j < elitism_counter; ++j) {
                offspring.push_back(parents[j]);
            }
            offspring.shrink_to_fit();
            // std::cout << "generation " << i << " min " << offspring[0].score() << std::endl;
            assert(offspring[0].score() <= offspring[mu-elitism_counter-1].score());
            parents = offspring;

            // check improvement
            if (offspring[0].score() < current_best_tour.score()) {
                current_best_tour = offspring[0];
                _stagnation_counter = 0;
            } else if (++_stagnation_counter >= max_gen_no_improvement) {
                std::cout << "Stopping due to stagnation ";
                std::cout << "at iteration: " << i << std::endl;
                break;
            }
        }
        return current_best_tour;
        auto min_element_it = std::min_element(parents.begin(), parents.end(), [&](const Tour& a, const Tour& b) {
            return a.score() < b.score();
        });

        return *min_element_it;
    }
};



#endif //SOLVER_H
