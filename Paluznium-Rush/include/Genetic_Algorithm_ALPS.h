/**
* @file Genetic_Algorithm_ALPS.h
* @brief Age-Layered Population Structure (ALPS) Genetic Algorithm for Premature Convergence. 
  See: https://www.researchgate.net/publication/216300808_ALPS_The_age-layered_population_structure_for_reducing_the_problem_of_premature_convergence

* @details
* Population layering
* - Age layers with age layer gap (e.g., 20)
    1. std::vector<double> layer_max_ages
* - Each layer has an age limit
* - Each individual has an age (that associates them with their age layer)
* - Each individual has_reproduced flag (to track aging)

Each Generation
* New individuals have age 0
* Increment age for individuals with has_reproduced == true

Breeding Rules (Selection)
* Breeding is restricted to individuals within a layer (defined by that layer’s max age)

Initialisation & Injection
* New individuals are injected every age gap iteration into the lowest layer

Elitism
* global elitism only for the 

Some of the new parameters:
1. int age_gap
2. int num_layers
3. std::vector<double> layer_max_ages

max_age = pop_size
injection_rate = 1 (all individuals in lowest layer)
*/

#pragma once

#include <vector>
#include <map>
#include <random>
#include <Genetic_Algorithm.h>

/**
 * @brief Individual structure for the ALPS genetic algorithm.
 * Extends the base Individual structure with additional fields for age and reproduction status.
 */
struct Individual_ALPS
{
    std::vector<int> vector;
    double fitness;
    int age; // age of the individual
    bool has_reproduced; // flag to track if the individual has reproduced (hence we age it)
};
typedef std::vector<Individual_ALPS> Population_ALPS;

/**
 * @brief Genetic Algorithm class for the ALPS (Age-Layered Population Structure) approach.
 * 
 * This class implements the ALPS genetic algorithm for optimization problems, with a focus on reducing premature convergence.
 * It includes methods for population initialization, injections, selection, crossover, mutation, and aging of individuals.
 */
class GeneticAlgorithmALPS
{
private:
    // randomization engine and distribution
    Algorithm_Parameters params_;
    std::mt19937 rng_;                                // Mersenne Twister random number engine for stochastic operations
    std::uniform_real_distribution<double> uni_dist_; // Uniform real distribution (0.0–1.0) for generating random doubles

public:
    Population_ALPS old_pop, new_pop;

    /* ----- ALPS ADDITIONS ----- */
    std::vector<double> layer_max_ages; // max ages for each layer
    int age_gap; // age gap between layers (e.g., 20)
    int age_increment; // increment for individuals with has_reproduced == true
    
    /**
     * @brief Default constructor.
     */
    GeneticAlgorithmALPS();  // constructor

    /**
     * @brief Constructor with parameters.
     * @param params Algorithm parameters for initialization.
     */
    GeneticAlgorithmALPS(Algorithm_Parameters & params); // constructor with parameters (initialiases age layers)

    /**
     * @brief Destructor.
     */
    ~GeneticAlgorithmALPS(); // destructor

    /* ---------------- INITIALISATIONS & INJECTIONS (only to lowest/first age layer) ---------------- */
    /**
     * @brief Initialize the population.
     * @param func Fitness function.
     * @param validity Validity check function.
     */
    void init_population(double (&func)(int, int *), bool (&validity)(int, int *) = all_true_ints);

    /**
     * @brief Inject new individuals into the lowest layer.
     * @param func Fitness function.
     * @param validity Validity check function.
     */
    void inject_population(double (&func)(int, int *), bool (&validity)(int, int *) = all_true_ints); // injects new individuals by replacing the lowest layer

    /* ---------------- SELECTIONS ---------------- */
    /**
     * @brief Select parents for breeding.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual_ALPS, Individual_ALPS> select_parents(); // returns best (randomly) selected parents

    /**
     * @brief Select parents using tournament selection.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual_ALPS, Individual_ALPS> select_tournament(); // returns 2 parents selected by tournament selection

    /* ---------------- SELECTIONS FOR ALPS ---------------- */
    /**
     * @brief Select parents using age-restricted tournament selection.
     * @return Pair of selected parent individuals.
     */
    std::pair<Individual_ALPS&, Individual_ALPS&> select_aged_tournament(); // breeding restricted to individuals within a layer and the layer below

    /* ---------------- CROSSOVER & MUTATION ---------------- */
    /**
     * @brief Perform crossover between two parents.
     * @param parent1 First parent individual.
     * @param parent2 Second parent individual.
     * @return Pair of offspring individuals.
     */
    std::pair<Individual_ALPS, Individual_ALPS> crossover(const Individual_ALPS &parent1,
                                                const Individual_ALPS &parent2);

    /**
     * @brief Mutate an individual.
     * @param offspring Individual to mutate.
     * @return Mutated individual.
     */
    Individual_ALPS mutate(const Individual_ALPS &offpsring);

    /* ---------------- MAIN LOOP ---------------- */
    /**
     * @brief Main optimization loop.
     * @param int_vector_size Size of the vector.
     * @param int_vector Initial vector.
     * @param func Fitness function.
     * @param validity Validity check function.
     * @param algorithm_parameters Algorithm parameters.
     * @param debug Debug flag.
     * @param store_performance Performance tracking flag.
     * @return Optimization result.
     */
    int optimize(int int_vector_size, Individual_ALPS &int_vector,
                 double (&func)(int, int *),
                 bool (&validity)(int, int *) = all_true_ints,
                 struct Algorithm_Parameters algorithm_parameters = DEFAULT_ALGORITHM_PARAMETERS,
                 bool debug = false, bool store_performance = false);

    /* ---------------- OTHER UTILITY FUNCTIONS ---------------- */
    /**
     * @brief Calculate population diversity.
     * @return Diversity measure.
     */
    double get_population_diversity(); // population diversity per generation

    /**
     * @brief Age the population.
     */
    void age_population(); // ages the population after every generation


    /**
     * @brief Print age distribution summary.
     */
    void get_age_summary(); // prints the portion of the population in each age layer

    /**
     * @brief Get layer distribution.
     * @return Vector of layer distributions.
     */
    std::vector<double> getLayerDistribution(); // returns the distribution of individuals in each layer (for performance analysis)

    /**
     * @brief Get best fitness per layer.
     * @return Vector of best fitness values.
     */
    std::vector<double> getBestFitnessPerLayer(); // returns the best fitness in each layer (for performance analysis)

    /**
     * @brief Sort population by fitness.
     */
    void sort_population(); // sorts the population in-place by fitness

    /**
     * @brief Get old population (getters).
     * @return Old population.
     */
    Population_ALPS get_old_pop() { return old_pop; };

    /**
     * @brief Get new population (setters).
     * @return New population.
     */
    Population_ALPS get_new_pop() { return new_pop; };
};
