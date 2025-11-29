/**
 * @file Genetic_Algorithm_ALPS.cpp
 * @brief Implementation of the Genetic Algorithm with Age-Layered Population Structure (ALPS).
 */

#include <stdio.h>
#include <cmath>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <set>

#include "Genetic_Algorithm_ALPS.h"
#include "CCircuit.h"
#include "CSimulator.h"
#include "performance_utilities.h"

using namespace std;

/**
 * @brief Default constructor for the GeneticAlgorithmALPS class.
 * Initializes the random number generator and uniform distribution.
 */
GeneticAlgorithmALPS::GeneticAlgorithmALPS()
    : params_{},
      rng_(std::random_device{}()),
      uni_dist_(0.0, 1.0)
{}

/**
 * @brief Constructor for the GeneticAlgorithmALPS class with parameters.
 * Initializes the random number generator, uniform distribution, and age layers.
 * 
 * @param params Algorithm parameters including population size and number of layers.
 */
GeneticAlgorithmALPS::GeneticAlgorithmALPS(Algorithm_Parameters & params)
    : params_(params),
      rng_(std::random_device{}()),
      uni_dist_(0.0, 1.0)
{
    age_gap = 40;
    age_increment = age_gap / 20; // increment for individuals with has_reproduced == true
    for (int i = 0; i < params_.number_of_layers; ++i) {
        layer_max_ages.push_back((i + 1) * age_gap);
    }
}

GeneticAlgorithmALPS::~GeneticAlgorithmALPS() = default;

/* ---------------- INITIALISATIONS & INJECTIONS---------------- 
* Initialises a random population of individuals/circuits (old_pop & new_pop).
*
* Hyperparameters used: pop_size (population size), number_of_units (number of units per circuit)
*
* Rules for valid individual (circuit) initialisation, n => number of units per circuit:
* 1. length should be 2n+1
* 2. feed_to (first element) should be valid (0 <= feed_to < n)
* 3. other elements can include conc and tails but not feed (1 <= element <= n+2)
* 4. conc = self or tails = self is not valid (e.g., not valid: [0, 1, 2, 3, 0, 4, 5])
* 5. two outputs of a unit should be different (e.g., not valid: [0, 1, 2, 3, 3, 4, 4])
* 6. remaining rules managed by Circuit::check_validity() through iterative checks
*/

/**
 * @brief Initialize the population.
 * @param func Fitness function.
 * @param validity Validity check function.
 */
void GeneticAlgorithmALPS::init_population(double (&func)(int, int *), bool (&validity)(int, int *))
{
    const int pop_size = params_.pop_size;
    const int number_of_units = params_.number_of_units;
    const int circuit_size = number_of_units * 2 + 1; // 2n+1

    old_pop.clear();
    old_pop.reserve(pop_size);

    std::uniform_int_distribution<int> feed_to_dist(0, number_of_units - 1);           // for vector[0]
    std::uniform_int_distribution<int> elem_dist(1, number_of_units + 2);              // for vector[j], j+1

    for (int i = 0; i < pop_size; ++i) {
        Individual_ALPS ind;

        do {
            ind.vector.clear();
            ind.vector.resize(circuit_size);

            ind.vector[0] = feed_to_dist(rng_); // 0 <= feed_to < n

            for (int j = 1; j < circuit_size; j += 2) {
                int elem1, elem2;

                do {
                    elem1 = elem_dist(rng_);
                    elem2 = elem_dist(rng_);
                } while (elem1 == elem2 || elem1 == (j - 1) / 2 || elem2 == j / 2);

                ind.vector[j] = elem1;
                ind.vector[j + 1] = elem2;
            }
        } while (!validity((int)ind.vector.size(), ind.vector.data())); // Ensure validity

        ind.fitness = func((int)ind.vector.size(), ind.vector.data());
        
        /* ----- ALPS ONLY ----- */
        ind.age = 0; // initial age
        ind.has_reproduced = false; // initial reproduction status
        old_pop.push_back(ind);
    }

    new_pop = old_pop; // deep copy
}

/*
 * Injects new individuals by replacing the lowest layer.
 * Operation is done in the old_pop.
 * Number of individuals to inject is determined by the number of individuals in the lowest layer.
 * 
 * 1. Remove all individuals in the lowest layer (age <= layer_max_ages[0]).
 * 2. Inject new individuals into the population.
 * 3. Ensure that the new individuals are valid and have a fitness score, with age set to 0.
 * 
 */

/**
 * @brief Inject new individuals into the lowest layer.
 * @param func Fitness function.
 * @param validity Validity check function.
 */
void GeneticAlgorithmALPS::inject_population(double (&func)(int, int *), bool (&validity)(int, int *)) {
    int injection_count = 0;
    auto it = std::remove_if(old_pop.begin(), old_pop.end(),
        [&](const Individual_ALPS &ind) {
            return ind.age <= layer_max_ages[0];
        });
    injection_count = std::distance(it, old_pop.end());
    old_pop.erase(it, old_pop.end());

    const int pop_size = params_.pop_size;
    const int number_of_units = params_.number_of_units;
    const int circuit_size = number_of_units * 2 + 1; // 2n+1
    std::uniform_int_distribution<int> feed_to_dist(0, number_of_units - 1);           // for vector[0]
    std::uniform_int_distribution<int> elem_dist(1, number_of_units + 2);              // for vector[j], j+1

    for (int i = 0; i < injection_count; ++i) {
        Individual_ALPS ind;

        do {
            ind.vector.clear();
            ind.vector.resize(circuit_size);

            ind.vector[0] = feed_to_dist(rng_); // 0 <= feed_to < n

            for (int j = 1; j < circuit_size; j += 2) {
                int elem1, elem2;

                // ensure elem1 != elem2 and both are not self-referencing
                do {
                    elem1 = elem_dist(rng_);
                    elem2 = elem_dist(rng_);
                } while (elem1 == elem2 || elem1 == (j - 1) / 2 || elem2 == j / 2);

                ind.vector[j] = elem1;
                ind.vector[j + 1] = elem2;
            }
        } while (!validity((int)ind.vector.size(), ind.vector.data())); // Ensure validity

        // set the initial values for injected individuals
        ind.fitness = func((int)ind.vector.size(), ind.vector.data());
        ind.age = 0; // initial age
        ind.has_reproduced = false; // initial reproduction status
        old_pop.push_back(ind);
    }
}

/* ---------------- SELECTION METHODS ----------------
 * Selects two parents from the population (old_pop).
 * Selection methods:
 * 1. Tournament selection (simple, fast, parallelizable)
 * 2. Aged tournament selection (age restriction)
 *
 */

/**
 * @brief Select parents for breeding.
 * @return std::pair<Individual, Individual> - selected parents
 */
std::pair<Individual_ALPS, Individual_ALPS> GeneticAlgorithmALPS::select_parents()
{
    return select_tournament();
}

/**
 * @brief Select parents using tournament selection.
 * @return Pair of selected parent individuals.
 */
std::pair<Individual_ALPS, Individual_ALPS> GeneticAlgorithmALPS::select_tournament() {
    std::uniform_int_distribution<size_t> index_dist(0, old_pop.size() - 1);

    auto tournament_select = [&](void) -> Individual_ALPS {
        Individual_ALPS best = old_pop[index_dist(rng_)];
        for (int i = 1; i < params_.tournament_size; ++i) {
            Individual_ALPS candidate = old_pop[index_dist(rng_)];
            if (candidate.fitness > best.fitness) {
                best = candidate;
            }
        }
        return best;
    };

    Individual_ALPS parent1 = tournament_select();
    Individual_ALPS parent2 = tournament_select();
    return {parent1, parent2};
}

/*
 * Implements a tournament selection method with age restriction.
 * Breeding is restricted to individuals within a layer and the layer below.
 * The current layer for breeding is determined randomly at each call.
 * 
 * @return std::pair<Individual_ALPS&, Individual_ALPS&> - references to selected parents
 */

/**
 * @brief Select parents using age-restricted tournament selection.
 * @return Pair of selected parent individuals.
 */
std::pair<Individual_ALPS&, Individual_ALPS&> GeneticAlgorithmALPS::select_aged_tournament() {
    std::vector<Individual_ALPS*> filtered_refs;
    while (filtered_refs.size() < 2) { // have at least 2 individuals in selected age range
        std::uniform_int_distribution<size_t> index_dist(0, old_pop.size() - 1);
        int layer_index = index_dist(rng_) % layer_max_ages.size();

        int min_age = layer_index > 1 ? layer_max_ages[layer_index - 2] : 0;
        int max_age = layer_max_ages[layer_index];

        for (auto& ind : old_pop) {
            if (ind.age >= min_age && ind.age <= max_age) {
                filtered_refs.push_back(&ind);
            }
        }
    }

    // apply the tournament selection in the determined age layer/range
    int tournament_size = std::max(1, static_cast<int>(filtered_refs.size()) / 2);
    std::uniform_int_distribution<size_t> index_dist_filtered(0, filtered_refs.size() - 1);

    auto aged_tournament_select = [&]() -> Individual_ALPS& {
        Individual_ALPS* best = filtered_refs[index_dist_filtered(rng_)];
        for (int i = 1; i < tournament_size; ++i) {
            Individual_ALPS* candidate = filtered_refs[index_dist_filtered(rng_)];
            if (candidate->fitness > best->fitness) {
                best = candidate;
            }
        }
        return *best;
    };

    Individual_ALPS& parent1 = aged_tournament_select();
    Individual_ALPS& parent2 = aged_tournament_select();

    return {parent1, parent2};
}

/* ---------------- CROSSOVER & MUTATION ----------------
* Mutates an offspring individual by randomly changing its genes.
* The mutation is done with a certain probability (params_.mutate_prob).
* 
* @param offspring - the individual to be mutated
* @return Individual_ALPS - the mutated individual
*/
    
/**
 * @brief Mutate an individual.
 * @param offspring Individual to mutate.
 * @return Individual_ALPS - the mutated individual.
 */
Individual_ALPS GeneticAlgorithmALPS::mutate(const Individual_ALPS &offspring)
{
    Individual_ALPS out = offspring;
    int n = params_.number_of_units;
    int len = 2 * n + 1;

    // distribution for random sampling
    std::uniform_int_distribution<int> hdr_dist(0, n - 1);
    std::uniform_int_distribution<int> gene_dist(0, n + 2);

    // mutate based on the mutation probability
    for (int i = 0; i < len; ++i)
    {
        if (uni_dist_(rng_) < params_.mutate_prob)
        {
            out.vector[i] = (i == 0)
                                ? hdr_dist(rng_)
                                : gene_dist(rng_);
        }
    }

    // define a lambda function to sample a gene that is not equal to the two forbidden values
    auto sample_not = [&](int forbid1, int forbid2)
    {
        int v;
        do
        {
            v = gene_dist(rng_);
        } while (v == forbid1 || v == forbid2);
        return v;
    };

    // avoid duplicates in the offspring
    for (int unit = 0; unit < n; ++unit)
    {
        int l = 2 * unit + 1;
        int r = l + 1;

        // if the left slot equals unit, sample a new value
        if (out.vector[l] == unit)
        {
            out.vector[l] = sample_not(unit, -1);
        }

        // if the right slot equals unit or the left slot, sample a new value
        if (out.vector[r] == unit || out.vector[r] == out.vector[l])
        {
            out.vector[r] = sample_not(unit, out.vector[l]);
        }
    }

    return out;
}

/**
 * @brief Perform crossover between two parents to create two offspring and is done with a certain probability (params_.cross_prob)..
 * @param parent1 First parent individual.
 * @param parent2 Second parent individual.
 * @return std::pair<Individual_ALPS, Individual_ALPS> - the two offspring individuals
 */
std::pair<Individual_ALPS, Individual_ALPS>
GeneticAlgorithmALPS::crossover(const Individual_ALPS &parent1,
                            const Individual_ALPS &parent2)
{
    int len = static_cast<int>(parent1.vector.size());
    Individual_ALPS child1 = parent1;
    Individual_ALPS child2 = parent2;

    if (uni_dist_(rng_) < params_.cross_prob)
    {
        std::uniform_int_distribution<int> cut_dist(1, len - 1);
        int cut = cut_dist(rng_);

        // switch the genes between the two parents
        std::copy(parent2.vector.begin(),
                  parent2.vector.begin() + cut,
                  child1.vector.begin());
        std::copy(parent1.vector.begin(),
                  parent1.vector.begin() + cut,
                  child2.vector.begin());
    }

    return {child1, child2};
}


/* ---------------- MAIN LOOP ---------------- */
/* =============== STORE PERFORMANCE DATA (if store_peformance set to true) =============== */
/**
 * @brief Main optimization loop.
 * @param int_vector_size Size of the vector.
 * @param best_individual Reference to the best individual found.
 * @param func Fitness function.
 * @param validity Validity check function.
 * @param params Algorithm parameters.
 * @param debug Debug flag.
 * @param store_performance Performance tracking flag.
 * @return Status code (0 for success).
 */
int GeneticAlgorithmALPS::optimize(int int_vector_size, Individual_ALPS &best_individual,
                               double (&func)(int, int *),
                               bool (&validity)(int, int *),
                               Algorithm_Parameters params, bool debug, bool store_performance)
{
    std::vector<std::vector<double>> fitness_values;
    std::vector<double> population_diversity;
    std::vector<std::string> meta_data;
    std::vector<Individual_ALPS> best_individuals;
    std::vector<std::vector<double>> age_layer_distribution;
    std::vector<std::vector<double>> best_fitness_layer_distribution;

    if (store_performance) {
        // create the directory if it does not exist
        GeneticAlgorithmUtils::createDirectory();
        // write meta data
        GeneticAlgorithmUtils::writeMetaData(params);

        fitness_values.reserve(params.max_iterations);
        age_layer_distribution.reserve(params.max_iterations);
        best_fitness_layer_distribution.reserve(params.max_iterations);
    }

    params_ = params;
    init_population(func, validity);

    best_individual = *std::max_element(old_pop.begin(), old_pop.end(),
                                        [](const Individual_ALPS &a, const Individual_ALPS &b)
                                        {
                                            return a.fitness < b.fitness;
                                        });

    int stall_counter = 0;

    // Main loop
    for (int gen = 0; gen < params_.max_iterations; ++gen)
    {
        if (debug) std::cout << "[Gen " << gen << "] Best fitness: " << best_individual.fitness << std::endl;
        new_pop.clear();
        new_pop.push_back(best_individual); // Elitism

        //std::vector<double> select_probs = this->calc_selection_probabilities();

        // Generate new individuals until the population size is reached
        while ((int)new_pop.size() < params_.pop_size)
        {
            // 1. Select parents
            /* ----- ALPS ----- */
            auto [p1, p2] = select_aged_tournament();

            // 2. Crossover
            auto [child1, child2] = crossover(p1, p2);

            // 3. Mutation
            if ((double)rand() / RAND_MAX < params.mutate_prob)
            {
                child1 = mutate(child1);
            }

            if ((double)rand() / RAND_MAX < params.mutate_prob)
            {
                child2 = mutate(child2);
            }
            
            // 4. Check validity
            if (validity(int_vector_size, child1.vector.data()))
            {
                child1.fitness = func(int_vector_size, child1.vector.data());
                new_pop.push_back(child1);
            }

            if ((int)new_pop.size() >= params_.pop_size)
                break;

            if (validity(int_vector_size, child2.vector.data()))
            {
                child2.fitness = func(int_vector_size, child2.vector.data());
                // set reproduce to true
                p1.has_reproduced = true;
                p2.has_reproduced = true;
                new_pop.push_back(child2);
            }
        }

        old_pop = new_pop;

        /* ----- ALPS ONLY ----- */
        // inject and age the old_pop (now replaced with new_pop)
        if (gen % age_gap == 0) // every age_gap generation inject new population to lowest age layer
        {
            if (debug) std::cout << "[Gen " << gen << "] Injecting new population..." << std::endl;
            inject_population(func, validity);
        }

        age_population();
        //get_age_summary(); // print the portion of the population in each age layer

        // 5. Evaluate fitness
        auto current_best = *std::max_element(old_pop.begin(), old_pop.end(),
                                              [](const Individual_ALPS &a, const Individual_ALPS &b)
                                              {
                                                  return a.fitness < b.fitness;
                                              });

        if (current_best.fitness > best_individual.fitness)
        {
            best_individual = current_best;
            stall_counter = 0;
        }
        else
        {
            stall_counter++;
        }

        if (store_performance)
        {
            // store population diversity
            population_diversity.push_back(get_population_diversity());
            // store all fitness values
            fitness_values.push_back(std::vector<double>(params_.pop_size));
            for (const auto &ind : old_pop) fitness_values[gen].push_back(ind.fitness);

            // store best individual
            best_individuals.push_back(best_individual);

            // store the age layer distribution of this generation
            std::vector<double> age_dist = getLayerDistribution();
            age_layer_distribution.push_back(age_dist);
            std::vector<double> best_fitness_values = getBestFitnessPerLayer();
            best_fitness_layer_distribution.push_back(best_fitness_values);
        }

        if (debug) std::cout << "[Gen " << gen << "] Best fitness: " << best_individual.fitness << std::endl;
        if (stall_counter >= params_.stall_iterations)
            break;
    }

    // Store performance data
    if (store_performance)
    {
        GeneticAlgorithmUtils::writeFitnessValues(fitness_values);
        GeneticAlgorithmUtils::writePopulationDiversity(population_diversity);
        GeneticAlgorithmUtils::writeBestIndividualProgression(best_individuals);
        GeneticAlgorithmUtils::writeAgeLayerDistribution(age_layer_distribution);
        GeneticAlgorithmUtils::writeBestFitnessPerLayer(best_fitness_layer_distribution);
    }

    return 0;
}

/* ---------------- OTHER UTILITY FUNCTIONS ---------------- */
/**
 * @brief Calculates the population diversity by counting unique individuals.
 * @return Diversity measure.
 */
double GeneticAlgorithmALPS::get_population_diversity()
{
    std::set<std::vector<int>> unique_individuals;
    for (const auto &ind : old_pop)
    {
        unique_individuals.insert(ind.vector);
    }
    return static_cast<double>(unique_individuals.size()) / old_pop.size();
}

/**
 * @brief Age the population.
 */
void GeneticAlgorithmALPS::age_population()
{
    for (auto &ind : old_pop)
    {
        if (ind.has_reproduced) {
            ind.age += age_increment; // increment age
            ind.has_reproduced = false; // reset reproduction status
        }
    }
}

/**
 * @brief Print age distribution summary.
 */
void GeneticAlgorithmALPS::get_age_summary()
{
    std::vector<int> age_count(layer_max_ages.size(), 0);
    for (const auto &ind : old_pop)
    {
        for (size_t i = 0; i < layer_max_ages.size(); ++i)
        {
            if (ind.age <= layer_max_ages[i])
            {
                age_count[i]++;
                break;
            }
        }
    }

    std::cout << "Age distribution: ";
    for (size_t i = 0; i < age_count.size(); ++i)
    {
        std::cout << "Layer " << i << ": " << static_cast<double>(age_count[i]) / old_pop.size() * 100 << "%, ";
    }
    std::cout << std::endl;
}

/**
 * @brief Get layer distribution.
 * @return std::vector<double> - distribution of individuals in each age layer in %
 */
std::vector<double> GeneticAlgorithmALPS::getLayerDistribution() {
    std::vector<double> age_count(layer_max_ages.size(), 0);
    for (const auto &ind : old_pop)
    {
        for (size_t i = 0; i < layer_max_ages.size(); ++i)
        {
            if (ind.age <= layer_max_ages[i])
            {
                age_count[i]++;
                break;
            }
        }
    }
    // convert to percentage
    for (size_t i = 0; i < age_count.size(); ++i)
    {
        age_count[i] = static_cast<double>(age_count[i]) / old_pop.size() * 100;
    }
    return age_count;
}

/**
 * @brief Get best fitness per layer.
 * @return std::vector<double> - best fitness value for each age layer
 */
std::vector<double> GeneticAlgorithmALPS::getBestFitnessPerLayer() {
    std::vector<double> best_fitness(layer_max_ages.size(), 0);
    for (const auto &ind : old_pop)
    {
        for (size_t i = 0; i < layer_max_ages.size(); ++i)
        {
            if (ind.age <= layer_max_ages[i])
            {
                best_fitness[i] = std::max(best_fitness[i], ind.fitness);
                break;
            }
        }
    }
    return best_fitness;
}

/**
 * @brief Sort population in-place in descending order of fitness.
 */
void GeneticAlgorithmALPS::sort_population()
{
    std::sort(old_pop.begin(), old_pop.end(),
              [](const Individual_ALPS &a, const Individual_ALPS &b)
              {
                  return a.fitness > b.fitness;
              });
}
