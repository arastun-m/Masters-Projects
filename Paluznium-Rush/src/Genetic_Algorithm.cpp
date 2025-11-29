/**
 * @file Genetic_Algorithm.cpp
 * @brief Implementation of the Genetic Algorithm.
 */

#include <stdio.h>
#include <cmath>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <set>

#include "Genetic_Algorithm.h"
#include "CCircuit.h"
#include "CSimulator.h"
#include "performance_utilities.h"
#include <omp.h>

using namespace std;

/**
 * @brief Check validity of both integer and real vectors.
 * @param int_vector_size Size of integer vector.
 * @param int_vector Integer vector.
 * @param real_vector_size Size of real vector.
 * @param real_vector Real vector.
 * @return True if all values are valid.
 */
bool all_true(int int_vector_size, int *int_vector, int real_vector_size, double *real_vector)
{
    return true;
}

/**
 * @brief Check validity of integer vector.
 * @param int_vector_size Size of integer vector.
 * @param int_vector Integer vector.
 * @return True if all values are valid.
 */
bool all_true_ints(int int_vector_size, int *int_vector)
{
    return true;
}

/**
 * @brief Check validity of real vector.
 * @param real_vector_size Size of real vector.
 * @param real_vector Real vector.
 * @return True if all values are valid.
 */
bool all_true_reals(int real_vector_size, double *real_vector)
{
    return true;
}

/**
 * @brief Default constructor.
 */
GeneticAlgorithm::GeneticAlgorithm()
    : params_{},
      rng_(std::random_device{}()),
      uni_dist_(0.0,1.0),
      gauss_(0.0, params_.real_sigma) {}

GeneticAlgorithm::~GeneticAlgorithm() = default;

/* ---------------- POPULATION INITIALISATION ---------------- */
/**
 * @brief Initialize the population.
 * @param func Fitness function.
 * @param validity Validity check function.
 */
void GeneticAlgorithm::init_population(double (&func)(int, int *), bool (&validity)(int, int *))
{
    /**
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

    const int pop_size = params_.pop_size;
    const int number_of_units = params_.number_of_units;
    const int circuit_size = number_of_units * 2 + 1; // 2n+1
 
    //New: use RNG instead of rand()
    std::uniform_int_distribution<int> feed_dist(0, number_of_units - 1); // Replaces: rand() % number_of_units
    std::uniform_int_distribution<int> dest_dist(1, number_of_units + 2); // Replaces: 1 + rand() % elem_range

    old_pop.clear();
    old_pop.reserve(pop_size);

    for (int i = 0; i < pop_size; ++i)
    {
        Individual ind;
        bool valid = false;

        // do
        while (!valid)
        {
            ind.vector.clear();
            ind.vector.resize(circuit_size);
 
            // ind.vector[0] = rand() % number_of_units; // 0 <= feed_to < n
            ind.vector[0] = feed_dist(rng_); // Feed destination: [0, n-1]
            for (int j = 1; j < circuit_size; j += 2)
            {
                int unit_id = (j - 1) / 2;
                int out1, out2;

                // Ensure: no duplicate outputs, no self-loops
                do {
                    out1 = dest_dist(rng_);
                    out2 = dest_dist(rng_);
                } while (out1 == out2 || out1 == unit_id || out2 == unit_id);

                ind.vector[j] = out1;
                ind.vector[j + 1] = out2;
            }

            // Validation step (unchanged)
            valid = validity((int)ind.vector.size(), ind.vector.data());
        }

        ind.fitness = func((int)ind.vector.size(), ind.vector.data());
        old_pop.push_back(ind);
    }

    new_pop = old_pop; // deep copy
}


/* ---------------- SELECTION METHODS ---------------- */
/**
 * @brief Select parents for breeding.
 * @return Pair of selected parent individuals.
 */
std::pair<Individual, Individual> GeneticAlgorithm::select_parents()
{
    /**
     * Selects two parents from the population (old_pop).
     * Selection methods:
     * 1. Tournament selection (simple, fast, parallelizable)
     *
     */

    return select_tournament(); // TODO: implement other selection methods
}

/**
 * @brief Select parents using tournament selection.
 * @return Pair of selected parent individuals.
 */
std::pair<Individual, Individual> GeneticAlgorithm::select_tournament() {
    /**
     * Implements a standard tournament selection method.
     * Uses the params_.tournament_size.
     * 
     */

    std::uniform_int_distribution<size_t> index_dist(0, old_pop.size() - 1);

    auto tournament_select = [&](void) -> Individual {
        Individual best = old_pop[index_dist(rng_)];
        for (int i = 1; i < params_.tournament_size; ++i) {
            Individual candidate = old_pop[index_dist(rng_)];
            if (candidate.fitness > best.fitness) {
                best = candidate;
            }
        }
        return best;
    };

    Individual parent1 = tournament_select();
    Individual parent2 = tournament_select();
    return {parent1, parent2};
}

/**
 * @brief Select parents using dynamic tournament selection.
 * @param gen Current generation number.
 * @return std::pair<Individual, Individual> - selected parents.
 */
std::pair<Individual, Individual> GeneticAlgorithm::select_dynamic_tournament(int gen) {
    /**
     * Implements a dynamic tournament selection method.
     * Vary tournament size over generations (e.g., start small, grow as generations advance).
     * Uses the params_.pop_size and params_.max_iterations.
     * 
     * rate < 1.0: Faster initial growth
     * rate = 1.0: Linear growth
     * rate > 1.0: Slower start, faster end (like a learning curve)
     * 
     */

    // set the dynamic tournament size
    const int max_tournament_size = params_.pop_size / 2;
    const int min_tournament_size = params_.pop_size / 50;
    int dy_tournament_size = min_tournament_size + \
        static_cast<int>((max_tournament_size - min_tournament_size) * \
        std::pow(static_cast<double>(gen) / params_.max_iterations, params_.tournament_rate));

    std::uniform_int_distribution<size_t> index_dist(0, old_pop.size() - 1);

    auto tournament_select = [&](void) -> Individual {
        Individual best = old_pop[index_dist(rng_)];
        for (int i = 1; i < dy_tournament_size; ++i) {
            Individual candidate = old_pop[index_dist(rng_)];
            if (candidate.fitness > best.fitness) {
                best = candidate;
            }
        }
        return best;
    };

    Individual parent1 = tournament_select();
    Individual parent2 = tournament_select();
    return {parent1, parent2};
}

/**
 * @brief Select parents using rank-based selection.
 * @param select_probs Selection probabilities.
 * @return std::pair<Individual, Individual> - selected parents.
 */
std::pair<Individual, Individual> GeneticAlgorithm::select_rank_based(std::vector<double> &select_probs) {
    /**
     * Implements a rank-based selection method.
     * Assume input pouplation is sorted by fitness.
     * Select two parent individuals from the population using the calculated selection probabilities.
     * 
     */

    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    double rand1 = prob_dist(rng_);
    double rand2 = prob_dist(rng_);
    double cum_prob1 = 0.0;
    double cum_prob2 = 0.0;
    Individual parent1, parent2;

    for (size_t i = 0; i < old_pop.size(); ++i) {
        cum_prob1 += select_probs[i];
        if (rand1 <= cum_prob1) {
            parent1 = old_pop[i];
            break;
        }
    }
    for (size_t i = 0; i < old_pop.size(); ++i) {
        cum_prob2 += select_probs[i];
        if (rand2 <= cum_prob2) {
            parent2 = old_pop[i];
            break;
        }
    }
    return {parent1, parent2};
}

/* ---------------- Mutation ---------------- */
/**
 * @brief Mutate an individual.
 * @param offspring Individual to mutate.
 * @return Mutated individual.
 */
Individual GeneticAlgorithm::mutate(const Individual &offspring)
{
    // Copy and basic setup
    Individual out = offspring;
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

// ---------------- Crossover ----------------
/**
 * @brief Perform crossover between two parents.
 * @param parent1 First parent individual.
 * @param parent2 Second parent individual.
 * @return Pair of offspring individuals.
 */
std::pair<Individual, Individual>
GeneticAlgorithm::crossover(const Individual &parent1,
                            const Individual &parent2)
{
    int len = static_cast<int>(parent1.vector.size());
    Individual child1 = parent1;
    Individual child2 = parent2;

    if (uni_dist_(rng_) < params_.cross_prob)
    {
        int num_units = (len - 1) / 2;

        for (int unit = 0; unit < num_units; ++unit)
        {
            if (rand() % 2)
            {
                int l = 2 * unit + 1;
                int r = l + 1;
                std::swap(child1.vector[l], child2.vector[l]);
                std::swap(child1.vector[r], child2.vector[r]);
            }
        }

        // feed_to (index 0) is untouched
    }

    return {child1, child2};
}

/* ---------------- MAIN LOOP---------------- */
/**
 * @brief Main optimization loop.
 * @param int_vector_size Size of the vector.
 * @param best_individual Reference to the best individual found.
 * @param func Fitness function.
 * @param validity Validity check function.
 * @param params Algorithm parameters.
 * @return Status code (0 for success).
 */
int GeneticAlgorithm::optimize(int int_vector_size, Individual &best_individual,
                               double (&func)(int, int *),
                               bool (&validity)(int, int *),
                               Algorithm_Parameters params)
{
    /* =============== DEBUG FLAG TO STORE PERFORMANCE DATA =============== */
    bool store_performance_data = false;
    std::vector<std::vector<double>> fitness_values;
    std::vector<double> population_diversity;
    std::vector<std::string> meta_data;
    std::vector<Individual> best_individuals;
    if (store_performance_data) {
        // create the directory if it does not exist
        GeneticAlgorithmUtils::createDirectory();
        // write meta data
        GeneticAlgorithmUtils::writeMetaData(params);

        fitness_values.reserve(params.max_iterations);
    }

    /* =============== MAIN LOOP =============== */
    params_ = params;
    init_population(func, validity);

    best_individual = *std::max_element(old_pop.begin(), old_pop.end(),
                                        [](const Individual &a, const Individual &b)
                                        {
                                            return a.fitness < b.fitness;
                                        });

    int stall_counter = 0;

    // Main loop
    for (int gen = 0; gen < params_.max_iterations; ++gen)
    {
        std::cout << "[Gen " << gen << "] Best fitness: " << best_individual.fitness << std::endl;
        new_pop.clear();
        new_pop.push_back(best_individual); // Elitism

        //std::vector<double> select_probs = this->calc_selection_probabilities();

        // Generate new individuals until the population size is reached
        while ((int)new_pop.size() < params_.pop_size)
        {

            // 1. Select parents
            auto [p1, p2] = select_parents();

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
                new_pop.push_back(child1);
            }

            if ((int)new_pop.size() >= params_.pop_size)
                break;

            if (validity(int_vector_size, child2.vector.data()))
            {
                new_pop.push_back(child2);
            }
        }

// 5. Evaluate fitness
#pragma omp parallel for
        for (auto &ind : new_pop)
        {
            ind.fitness = func(int_vector_size, ind.vector.data());
        }

        old_pop = new_pop;

        // 5. Evaluate fitness
        auto current_best = *std::max_element(old_pop.begin(), old_pop.end(),
                                              [](const Individual &a, const Individual &b)
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

        if (store_performance_data)
        {
            // store population diversity
            population_diversity.push_back(get_population_diversity());
            // store all fitness values
            fitness_values.push_back(std::vector<double>(params_.pop_size));
            for (const auto &ind : old_pop) fitness_values[gen].push_back(ind.fitness);
            // store best individual
            best_individuals.push_back(best_individual);
        }

        std::cout << "[Gen " << gen << "] Best fitness: " << best_individual.fitness << std::endl;
        if (stall_counter >= params_.stall_iterations)
            break;
    }

    // Store performance data
    if (store_performance_data)
    {
        GeneticAlgorithmUtils::writeFitnessValues(fitness_values);
        GeneticAlgorithmUtils::writePopulationDiversity(population_diversity);
        GeneticAlgorithmUtils::writeBestIndividualProgression(best_individuals);
    }

    return 0;
}


/* --------------- Beta --------------- */ 
/* ---------------- POPULATION INITIALISATION ---------------- */
/**
 * @brief Initialize population for beta optimization.
 * @param fixed_topology Fixed circuit topology.
 * @param fit Fitness function.
 * @param val Validity check function.
 */
void GeneticAlgorithm::init_population_beta(
    const std::vector<int>& fixed_topology,
    double (&fit)(int,int*,int,double*),
    bool   (&val)(int,int*,int,double*))
{
    int   n         = (int)(fixed_topology.size() - 1)/2;  // if topo length = 2*n+1
    int   P         = params_.pop_size;
    int   topo_len  = (int)fixed_topology.size();
    std::uniform_real_distribution<double> uni01(0.0,1.0);

    old_pop.clear();
    old_pop.reserve(P);

    for (int i = 0; i < P; ++i) {
        Individual ind;
        ind.vector = fixed_topology;      // copy fixed circuit
        ind.beta.resize(n);

        // Random β until validity satisfied
        do {
            for (double &b : ind.beta) {
                b = uni01(rng_);
            }
        } while (!val(topo_len, ind.vector.data(),
                      n,        ind.beta.data()));

        ind.fitness = fit(topo_len, ind.vector.data(),
                          n,        ind.beta.data());
        old_pop.push_back(std::move(ind));
    }

    new_pop = old_pop;  // deep copy (or use swap if you prefer)
}
/* ---------------- SELECTION METHODS ---------------- */
// use previous selection methods
/* ---------------- Crossover ---------------- */
/**
 * @brief Perform simulated binary crossover (SBX) on the real-valued part (beta) of two parents.
 *
 * @param p1   The first parent individual (must have p1.beta populated).
 * @param p2   The second parent individual (must have p2.beta populated).
 * @returns    A pair of offspring individuals (child1, child2) with their beta vectors recombined.
 */
std::pair<Individual, Individual>
GeneticAlgorithm::crossover_beta(const Individual &p1,
                                 const Individual &p2)
{
    // Number of real-valued genes per individual
    int n = static_cast<int>(p1.beta.size());

    // Initialize offspring as copies of the parents.
    // If crossover does not occur, we will return these unmodified.
    Individual child1 = p1;
    Individual child2 = p2;

    // Decide whether to perform crossover at all.
    // Draw a uniform random number in [0,1). If it is >= cross_prob, skip crossover.
    if (uni_dist_(rng_) >= params_.cross_prob) {
        // No crossover: return the cloned parents.
        return { child1, child2 };
    }

    // SBX distribution index (η). Controls the spread of offspring around parents.
    // Typical values range from 2 to 5.
    const double η = 2.0;

    // For each gene position, perform SBX independently.
    for (int i = 0; i < n; ++i) {
        // Sample a uniform random number u in [0,1)
        double u = uni_dist_(rng_);

        // Compute the SBX factor 'beta_factor' based on u:
        // If u <= 0.5:    beta_factor = (2u)^(1/(η+1))
        // Else:           beta_factor = [2(1-u)]^(1/(η+1))
        // This yields a value in [0,1] with a distribution controlled by η.
        double beta_factor;
        if (u <= 0.5) {
            beta_factor = std::pow(2.0 * u, 1.0 / (η + 1.0));
        } else {
            beta_factor = std::pow(2.0 * (1.0 - u), 1.0 / (η + 1.0));
        }

        // Retrieve parent gene values
        double x1 = p1.beta[i];
        double x2 = p2.beta[i];

        // Compute offspring gene values using the SBX formulas:
        //   c1 = 0.5 * [ (1 + β) * x1 + (1 - β) * x2 ]
        //   c2 = 0.5 * [ (1 - β) * x1 + (1 + β) * x2 ]
        child1.beta[i] = 0.5 * ((1.0 + beta_factor) * x1 + (1.0 - beta_factor) * x2);
        child2.beta[i] = 0.5 * ((1.0 - beta_factor) * x1 + (1.0 + beta_factor) * x2);

        // Clamp the results to the valid range [0,1], in case of numerical drift
        child1.beta[i] = std::clamp(child1.beta[i], 0.0, 1.0);
        child2.beta[i] = std::clamp(child2.beta[i], 0.0, 1.0);
    }

    // Return the two newly created offspring
    return { child1, child2 };
}
/* ---------------- Mutation ---------------- */
/**
 * @brief Mutate individual for beta optimization.
 * @param in Individual to mutate.
 * @return Mutated individual.
 */
Individual GeneticAlgorithm::mutate_beta(const Individual &in)
{
    Individual out = in;
    for(double &b : out.beta) {
        if (uni_dist_(rng_) < params_.mutate_prob_real) {
            b = std::clamp(b + gauss_(rng_), 0.0, 1.0);
        }
    }
    return out;
}

/* ---------------- MAIN LOOP---------------- */
/**
 * @brief Optimize beta for a given circuit.
 * @param beta_size Size of beta vector.
 * @param beta_vector Initial beta vector.
 * @param func Fitness function.
 * @param validity Validity check function.
 * @param algorithm_parameters Algorithm parameters.
 * @return Status code (0 for success).
 */
int GeneticAlgorithm::optimize_beta(int beta_size, Individual &beta_vector,
                 double (&func)(int, int *, int, double *),
                 bool (&validity)(int, int *, int, double *),
                 struct Algorithm_Parameters algorithm_parameters)
{
      // 1) Backup GA parameters
    params_ = algorithm_parameters;

    // 2) Initialize the population with a fixed circuit (topology)
    init_population_beta(beta_vector.vector, func, validity);

    // 3) Find the best
    Individual best = *std::max_element(
        old_pop.begin(), old_pop.end(),
        [](auto &a, auto &b){ return a.fitness < b.fitness; });

    int stall = 0;
    // 4) GA main loop
    for(int gen = 0; gen < params_.max_iterations && stall < params_.stall_iterations; ++gen){
        new_pop.clear();
        new_pop.push_back(best);  // elitism

        while((int)new_pop.size() < params_.pop_size){
            // 4.1 select parents
            auto parents = select_tournament();
            auto p1 = parents.first;
            auto p2 = parents.second;

            // 4.2 crossover
            auto children = crossover_beta(p1, p2);
            auto c1 = children.first;
            auto c2 = children.second;

            // 4.3 mutation
            c1 = mutate_beta(c1);
            c2 = mutate_beta(c2);

            // 4.4 check validity
            if(validity((int)beta_vector.vector.size(),
                        beta_vector.vector.data(),
                        beta_size, c1.beta.data()))
            {
                new_pop.push_back(c1);
            }
            if((int)new_pop.size() < params_.pop_size &&
               validity((int)beta_vector.vector.size(),
                        beta_vector.vector.data(),
                        beta_size, c2.beta.data()))
            {
                new_pop.push_back(c2);
            }
        }

        #pragma omp parallel for
        for (int i = 0; i < (int)new_pop.size(); ++i)
        {
            new_pop[i].fitness = func(
                (int)beta_vector.vector.size(),
                beta_vector.vector.data(),
                beta_size,
                new_pop[i].beta.data());
        }

        // 4.5 Updating the population & finding the best of the generation
        old_pop.swap(new_pop);
        auto current_best = *std::max_element(
            old_pop.begin(), old_pop.end(),
            [](auto &a, auto &b){ return a.fitness < b.fitness; });

        if(current_best.fitness > best.fitness){
            best = current_best;
            stall = 0;
        } else {
            ++stall;
        }
        std::cout << "[Gen " << gen << "] best = " << best.fitness << "\n";
    }

    // 5) Write the best β back to the caller
    beta_vector.beta    = best.beta;
    beta_vector.fitness = best.fitness;
    return 0;
}

/* -------------- hybrid -------------- */ 
/* ---------------- POPULATION INITIALISATION ---------------- */
/**
 * @brief Initialize population for hybrid optimization.
 * @param fit Fitness function.
 * @param val Validity check function.
 */
void GeneticAlgorithm::init_population_hybrid(
        double (&fit)(int,int*,int,double*),
        bool   (&val)(int,int*,int,double*))
{
    int n_units = params_.number_of_units;
    int topo_len = 2*n_units + 1;
    int P        = params_.pop_size;
    std::uniform_real_distribution<double> uni01(0.0,1.0);
    std::uniform_int_distribution<int> gene_dist(0, n_units+2);
    std::uniform_int_distribution<int> feed_dist(0, n_units-1);

    old_pop.clear(); old_pop.reserve(P);

    for(int i=0;i<P;++i)
    {
        Individual ind;
        ind.vector.resize(topo_len);
        ind.beta.resize(n_units);

        // ---------- random DISCRETE part ----------
        ind.vector[0] = feed_dist(rng_);
        for(int u=0; u<n_units; ++u){
            int l = 2*u+1, r = l+1;
            do{
                ind.vector[l] = gene_dist(rng_);
                ind.vector[r] = gene_dist(rng_);
            }while(ind.vector[l]==ind.vector[r]||   // conc!=tails
                   ind.vector[l]==u || ind.vector[r]==u); // no self-feed
        }

        // ---------- random CONTINUOUS part ----------
        for(double& b : ind.beta) b = uni01(rng_);

        // ---------- validity + fitness ----------
        std::cout << "topo_len = " << topo_len << "\nvector = [ ";
        for (int k = 0; k < topo_len; ++k) {
            std::cout << ind.vector[k];
            if (k + 1 < topo_len) std::cout << ", ";
        }
        std::cout << " ]\n";
        if(!val(topo_len, ind.vector.data(), n_units, ind.beta.data())){
            --i; continue;        // draw again
        }
        ind.fitness = fit(topo_len, ind.vector.data(), n_units, ind.beta.data());
        old_pop.push_back(std::move(ind));
    }
    new_pop = old_pop;
}

/* ---------------- SELECTION METHODS ---------------- */
// use previous selection methods
/* ---------------- Mutation ---------------- */
/**
 * @brief Mutate individual for hybrid optimization.
 * @param in Individual to mutate.
 * @return Mutated individual.
 */
Individual GeneticAlgorithm::mutate_hybrid(const Individual& in)
{
    Individual out = mutate(in);      // mutate discrete genes
    Individual tmp = mutate_beta(out);// mutate β
    return tmp;
}

/* ---------------- Crossover ---------------- */
/**
 * @brief Perform crossover for hybrid optimization.
 * @param p1 First parent individual.
 * @param p2 Second parent individual.
 * @return Pair of offspring individuals.
 */
std::pair<Individual,Individual>
GeneticAlgorithm::crossover_hybrid(const Individual& p1,
                                   const Individual& p2)
{
    // 1) discrete part
    auto topo_children = crossover(p1, p2);      // single-point
    Individual c1 = topo_children.first;
    Individual c2 = topo_children.second;

    // 2) continuous part
    auto beta_children = crossover_beta(p1, p2); // SBX
    c1.beta = beta_children.first.beta;
    c2.beta = beta_children.second.beta;

    return {c1, c2};
}

/* ---------------- MAIN LOOP---------------- */
// int GeneticAlgorithm::optimize_hybrid(int vector_size, Individual &hybrid_vector,
//                  double (&func)(int, int*, int, double *),
//                  bool (&validity)(int, int *, int, double *),
//                  struct Algorithm_Parameters algorithm_parameters)
// {return 0;}
/**
 * @brief Optimize circuit and beta value.
 * @param topo_len Length of topology vector.
 * @param best_individual Reference to the best individual found.
 * @param fit Fitness function.
 * @param valid Validity check function.
 * @param p Algorithm parameters.
 * @return Status code (0 for success).
 */
int GeneticAlgorithm::optimize_hybrid(
        int                 topo_len,          // length 2 n + 1
        Individual&         best_individual,   // result holder
        double (&fit   )(int,int*,int,double*),// fitness(topo,β)
        bool   (&valid  )(int,int*,int,double*),// validity(topo,β)
        Algorithm_Parameters p)                // GA hyper-params
{
    // -----------------------------------------------------------------
    // 0.  Sanity check on vector length
    // -----------------------------------------------------------------
    if (topo_len < 3 || (topo_len & 1) == 0) {
        std::cerr << "optimize_hybrid: topo_len must be 2*n+1\n";
        return -1;
    }
    int n_units = (topo_len - 1) / 2;

    // -----------------------------------------------------------------
    // 1.  Store parameters & build initial population
    // -----------------------------------------------------------------
    params_ = p;
    init_population_hybrid(fit, valid);            // fills old_pop


    // best_individual = elite of generation 0
    best_individual =
        *std::max_element(old_pop.begin(), old_pop.end(),
                          [](const Individual& a, const Individual& b)
                          { return a.fitness < b.fitness; });

    int stall = 0;

    // -----------------------------------------------------------------
    // 2.  Evolution loop
    // -----------------------------------------------------------------
    for (int gen = 0;
         gen < params_.max_iterations && stall < params_.stall_iterations;
         ++gen)
    {
        new_pop.clear();
        new_pop.push_back(best_individual); // elitism

        // ── create rest of new_pop ─────────────────────────────────
        while ((int)new_pop.size() < params_.pop_size)
        {
            // 2.1 parent selection
            auto parents  = select_tournament();
            const Individual& p1 = parents.first;
            const Individual& p2 = parents.second;

            // 2.2 crossover (both parts)
            auto children = crossover_hybrid(p1, p2);
            Individual c1 = children.first;
            Individual c2 = children.second;

            // 2.3 mutation (both parts)
            c1 = mutate_hybrid(c1);
            c2 = mutate_hybrid(c2);

            // 2.4 validity + fitness
            if (valid(topo_len, c1.vector.data(), n_units, c1.beta.data())) {
                new_pop.push_back(c1);
            }
            if (new_pop.size() < params_.pop_size &&
                valid(topo_len, c2.vector.data(), n_units, c2.beta.data()))
            {
                new_pop.push_back(c2);
            }
        } // while(population not full)

        // ── evaluate fitness of new population ──────────────────────
        #pragma omp parallel for
        for (int i = 0; i < (int)new_pop.size(); ++i) {
            new_pop[i].fitness = fit(topo_len, new_pop[i].vector.data(), n_units, new_pop[i].beta.data());
        }

        // -----------------------------------------------------------------
        // 3.  Replace population & update best / stall counter
        // -----------------------------------------------------------------
        old_pop.swap(new_pop);

        const Individual& gen_best =
            *std::max_element(old_pop.begin(), old_pop.end(),
                              [](const Individual& a, const Individual& b)
                              { return a.fitness < b.fitness; });

        if (gen_best.fitness > best_individual.fitness) {
            best_individual = gen_best;
            stall = 0;
        } else {
            ++stall;
        }

        std::cout << "[Gen " << gen << "] best = "
                  << best_individual.fitness << '\n';
    } // for gen

    return 0;   // success
}

/**
 * @brief Calculate population diversity.
 * @return Diversity measure.
 */
double GeneticAlgorithm::get_population_diversity()
{
    // Calculate population diversity
    std::set<std::vector<int>> unique_individuals;
    for (const auto &ind : old_pop)
    {
        unique_individuals.insert(ind.vector);
    }
    return static_cast<double>(unique_individuals.size()) / old_pop.size();
}

/**
 * @brief Sort population by fitness.
 */
void GeneticAlgorithm::sort_population()
{
    // Sort the population in-place by fitness
    std::sort(old_pop.begin(), old_pop.end(),
              [](const Individual &a, const Individual &b)
              {
                  return a.fitness > b.fitness;
              });
}

/**
 * @brief Calculate selection probabilities.
 * @return Vector of selection probabilities.
 */
std::vector<double> GeneticAlgorithm::calc_selection_probabilities()
{
    // Calculate the selection probabilities for each individual (rank-based selection)
    this->sort_population();
    std::vector<double> selection_probs(old_pop.size());
    double total_prob = 0.0;
    for (size_t i = 0; i < old_pop.size(); ++i)
    {
        selection_probs[i] = static_cast<double>(i + 1) / (old_pop.size() * (old_pop.size() + 1) / 2);
        total_prob += selection_probs[i];
    }
    return selection_probs;
}
