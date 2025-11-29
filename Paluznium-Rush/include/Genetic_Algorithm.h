/**
 * @file Genetic_Algorithm.h
 * @brief Header for the Genetic Algorithm library
 */

#pragma once

#include <vector>
#include <map>
#include <random>

/**
 * @brief Algorithm parameters structure.
 */
struct Algorithm_Parameters{
    // genetic algorithm hyperparameters
    int    max_iterations   = 1000;
    int    stall_iterations = 30;   // stall termination criteria
    double cross_prob       = 0.75;  // crossover probability
    double mutate_prob      = 0.05; // mutation probability
    double mutate_prob_real = 0.15; // mutation probability for real-valued individuals
    int    pop_size         = 100;  // population size
    int    number_of_units  = 10;   // number of units per circuit
    int    tournament_size  = 10;    // selection tournament size (higher = more selective & less diversity)
    double real_sigma       = 0.1;  // mutation sigma for real-valued individuals
    double tournament_rate  = 0.5;  // dynamic tournament rate (<= 1.0: faster start rate, >= 1.0: slower start, faster end)
    int    number_of_layers = 5;    // number of layers in the ALPS
};

#define DEFAULT_ALGORITHM_PARAMETERS Algorithm_Parameters{}

/**
 * @brief Individual structure for genetic algorithm.
 */
struct Individual
{
    std::vector<int> vector;
    std::vector<double> beta; // for real-valued individuals
    double fitness;
};
typedef std::vector<Individual> Population;

/**
 * @brief Check validity of both integer and real vectors.
 * @param int_vector_size Size of integer vector.
 * @param int_vector Integer vector.
 * @param real_vector_size Size of real vector.
 * @param real_vector Real vector.
 * @return True if all values are valid.
 */
bool all_true(int int_vector_size, int *int_vector,
              int real_vector_size, double *real_vector);

/**
 * @brief Check validity of integer vector.
 * @param int_vector_size Size of integer vector.
 * @param vector Integer vector.
 * @return True if all values are valid.
 */
bool all_true_ints(int int_vector_size, int *vector);

/**
 * @brief Check validity of real vector.
 * @param real_vector_size Size of real vector.
 * @param vector Real vector.
 * @return True if all values are valid.
 */
bool all_true_reals(int real_vector_size, double *vector);

/**
 * @brief Genetic Algorithm class.
 */
class GeneticAlgorithm
{

private:
    Algorithm_Parameters params_;
    std::mt19937 rng_;                                // Mersenne Twister random number engine for stochastic operations
    std::uniform_real_distribution<double> uni_dist_; // Uniform real distribution (0.0–1.0) for generating random doubles
    std::normal_distribution<double> gauss_;


public:
    Population old_pop, new_pop;

    /**
     * @brief Default constructor.
     */
    GeneticAlgorithm();  // constructor
    /**
     * @brief Destructor.
     */
    ~GeneticAlgorithm(); // destructor

    /* ---------------- HELPER FUNCTIONS ---------------- */
    /**
     * @brief Initialize the population.
     * @param func Fitness function.
     * @param validity Validity check function.
     */
    void init_population(double (&func)(int, int *), bool (&validity)(int, int *) = all_true_ints);
    /**
     * @brief Select parents for breeding.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual, Individual> select_parents(); // returns best (randomly) selected parents
    /**
     * @brief Select parents using tournament selection.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual, Individual> select_tournament(); // returns 2 parents selected by tournament selection
    /**
     * @brief Select parents using dynamic tournament selection.
     * @param gen Current generation number.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual, Individual> select_dynamic_tournament(int gen); // returns 2 parents selected by dynamic tournament selection
    /**
     * @brief Select parents using rank-based selection.
     * @param select_probs Selection probabilities.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual, Individual> select_rank_based(std::vector<double> & select_probs); // returns 2 parents selected by rank-based selection

    /**
     * @brief Perform crossover between two parents.
     * @param parent1 First parent individual.
     * @param parent2 Second parent individual.
     * @return Pair of offspring individuals.
     */
    std::pair<Individual, Individual> crossover(const Individual &parent1,
                                                const Individual &parent2);
    /**
     * @brief Mutate an individual.
     * @param offspring Individual to mutate.
     * @return Mutated individual.
     */
    Individual mutate(const Individual &offpsring);

    /* ---------------- MAIN LOOP ---------------- */
    /**
     * @brief Main optimization loop.
     * @param int_vector_size Size of the vector.
     * @param int_vector Initial vector.
     * @param func Fitness function.
     * @param validity Validity check function.
     * @param algorithm_parameters Algorithm parameters.
     * @return Optimization result.
     */
    int optimize(int int_vector_size, Individual &int_vector,
                 double (&func)(int, int *),
                 bool (&validity)(int, int *) = all_true_ints,
                 struct Algorithm_Parameters algorithm_parameters = DEFAULT_ALGORITHM_PARAMETERS);

    /* ---------------- EXTENSIONS ---------------- */
    /**
     * @brief Optimize beta for a given circuit.
     * @param beta_size Size of beta vector.
     * @param beta_vector Initial beta vector.
     * @param func Fitness function.
     * @param validity Validity check function.
     * @param algorithm_parameters Algorithm parameters.
     * @return Optimization result.
     */
    int optimize_beta(int beta_size, Individual &beta_vector,
                 double (&func)(int, int *, int, double *),
                 bool (&validity)(int, int *, int, double *) = all_true,
                 struct Algorithm_Parameters algorithm_parameters = DEFAULT_ALGORITHM_PARAMETERS);

    /**
     * @brief Optimize circuit and beta value.
     * @param vector_size Size of the vector.
     * @param hybrid_vector Initial hybrid vector.
     * @param func Fitness function.
     * @param validity Validity check function.
     * @param algorithm_parameters Algorithm parameters.
     * @return Optimization result.
     */
    int optimize_hybrid(int vector_size, Individual &hybrid_vector,
                 double (&func)(int, int*, int, double *),
                 bool (&validity)(int, int *, int, double *) = all_true,
                 struct Algorithm_Parameters algorithm_parameters = DEFAULT_ALGORITHM_PARAMETERS);

    /* ---------------- HELPER FUNCTIONS for beta ---------------- */
    /**
     * @brief Initialize population for beta optimization.
     * @param fixed_topology Fixed circuit topology.
     * @param func Fitness function.
     * @param validity Validity check function.
     */
    void init_population_beta(const std::vector<int>& fixed_topology, double (&func)(int, int *, int, double *), bool (&validity)(int, int *, int, double *) = all_true);
    /**
     * @brief Perform crossover for beta optimization.
     * @param parent1 First parent individual.
     * @param parent2 Second parent individual.
     * @return Pair of offspring individuals.
     */
    std::pair<Individual, Individual> crossover_beta(const Individual &parent1,
                                                const Individual &parent2);
    /**
     * @brief Mutate individual for beta optimization.
     * @param offspring Individual to mutate.
     * @return Mutated individual.
     */
    Individual mutate_beta(const Individual &offpsring);
    
    /* ---------------- HELPER FUNCTIONS for hybrid---------------- */
    /**
     * @brief Initialize population for hybrid optimization.
     * @param func Fitness function.
     * @param validity Validity check function.
     */
    void init_population_hybrid(double (&func)(int, int *, int, double *), bool (&validity)(int, int *, int, double *) = all_true);
    /**
     * @brief Perform crossover for hybrid optimization.
     * @param parent1 First parent individual.
     * @param parent2 Second parent individual.
     * @return Pair of offspring individuals.
     */
    std::pair<Individual, Individual> crossover_hybrid(const Individual &parent1,
                                                const Individual &parent2);
    /**
     * @brief Mutate individual for hybrid optimization.
     * @param offspring Individual to mutate.
     * @return Mutated individual.
     */
    Individual mutate_hybrid(const Individual &offpsring);

    // getters and setters
    /**
     * @brief Get old population.
     * @return Old population.
     */
    Population get_old_pop() { return old_pop; };
    /**
     * @brief Get new population.
     * @return New population.
     */
    Population get_new_pop() { return new_pop; };
    // other utility methods
    /**
     * @brief Calculate population diversity.
     * @return Diversity measure.
     */
    double get_population_diversity();
    /**
     * @brief Sort population by fitness.
     */
    void sort_population(); // sorts the population in-place by fitness
    /**
     * @brief Calculate selection probabilities.
     * @return Vector of selection probabilities.
     */
    std::vector<double> calc_selection_probabilities(); // calculates the selection probabilities for each individual (rank-based selection)
};
