/**
 * @file   : main.cpp
 * @brief  : Main file for running the genetic algorithm optimization.
 * @details: This file contains the main function that runs the genetic algorithm optimization
 *           for a given circuit. It includes the standard genetic algorithm and the age-layered
 *           population structure (ALPS) genetic algorithm.
 */

#include <iostream>
#include <vector>
#include <iomanip>

#include "CUnit.h"
#include "CCircuit.h"
#include "CSimulator.h"
#include "Genetic_Algorithm.h"
#include "Genetic_Algorithm_ALPS.h" // ALPS support

// Include the updated config loader
#include "config.h"

// Function declarations
/**
 * @brief Run the standard genetic algorithm optimization.
 * @details This function runs the GA for one or more sets of parameters loaded from config.json.
 */
void runStandardGA(); // Standard GA runner
/**
 * @brief Run the age-layered population structure (ALPS) genetic algorithm optimization.
 * @details This function sets up the hyperparameters for the ALPS genetic algorithm and runs the optimization.
 */
void runALPSGA();     // ALPS GA runner
/**
 * @brief Run a hybrid genetic algorithm optimization (topology + beta).
 * @details This function demonstrates a hybrid GA that optimizes both discrete topology and continuous beta parameters.
 */
void runHybridGA();  // Hybrid GA runner

/**
 * @brief Demo fitness function for hybrid GA (for demonstration purposes).
 * @param topo_len Length of the topology vector.
 * @param topo Pointer to the topology vector.
 * @param n_units Number of units (length of beta vector).
 * @param beta Pointer to the beta vector.
 * @return Fitness value.
 */
double demo_hybrid_fitness(int topo_len, int* topo,
                    int n_units, double* beta);


// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
/**
 * @brief Main entry point for the genetic algorithm optimization program.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char* argv[])
{
    //runStandardGA();
    //runALPSGA();
    //runHybridGA();
}
                    
/**
 * @brief Run the standard genetic algorithm optimization.
 * @details This function runs the GA for one or more sets of parameters loaded from config.json.
 */
void runStandardGA()
{
    std::cout << "Genetic Algorithm Optimization" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    // NEW: Load all configurations from config.json (could be a single object or array)
    std::vector<Algorithm_Parameters> configs = load_all_configs("../src/config.json");

    // OLD: Hardcoded/single-config loading
    // Algorithm_Parameters params = load_config("../src/config.json");

    // OLD: Hardcoded parameter block
    /*
    Algorithm_Parameters params = {
        .max_iterations = 1000,
        .stall_iterations = 50,
        .cross_prob = 0.7,
        .mutate_prob = 0.2,
        .pop_size = 100,
        .number_of_units = 4,
        .tournament_size = 10
    };
    */

    for (size_t i = 0; i < configs.size(); ++i)
    {
        const Algorithm_Parameters& params = configs[i];

        // Display params for current config
        std::cout << "\n[Config " << i + 1 << "] Running with parameters:" << std::endl;
        std::cout << "  max_iterations    = " << params.max_iterations << std::endl;
        std::cout << "  stall_iterations  = " << params.stall_iterations << std::endl;
        std::cout << "  cross_prob        = " << params.cross_prob << std::endl;
        std::cout << "  mutate_prob       = " << params.mutate_prob << std::endl;
        std::cout << "  pop_size          = " << params.pop_size << std::endl;
        std::cout << "  number_of_units   = " << params.number_of_units << std::endl;
        std::cout << "  tournament_size   = " << params.tournament_size << std::endl;

        GeneticAlgorithm ga;
        int genome_length = 2 * params.number_of_units + 1; // 2n + 1
        Individual best;

        // Run the optimisation for this config
        ga.optimize(genome_length, best, circuit_performance, Circuit::check_validity, params);

        // Display final results for this config
        std::cout << "\nFINAL RESULT (Config " << i + 1 << "):" << std::endl;
        std::cout << "Best Fitness: " << best.fitness << std::endl;
        std::cout << "Best Vector: ";
        for (int gene : best.vector)
            std::cout << gene << " ";

        // Recompute & show fitness for safety
        std::cout << "\nBest Fitness: " << circuit_performance(genome_length, best.vector.data()) << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
    }
}

/**
 * @brief Run the age-layered population structure (ALPS) genetic algorithm optimization.
 * @details This function sets up the hyperparameters for the ALPS genetic algorithm and runs the optimization.
 */
void runALPSGA()
{
    std::cout << "Genetic Algorithm Optimization (ALPS)" << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    Algorithm_Parameters params = {
        .max_iterations = 500,
        .stall_iterations = 500, // ALPS disables stall termination
        .cross_prob = 0.9,
        .mutate_prob = 0.2,
        .pop_size = 200,
        .number_of_units = 10,
        .tournament_size = 50,
        .tournament_rate = 2.0,
        .number_of_layers = 5
    };

    int genome_length = 2 * params.number_of_units + 1; // 2n+1
    Individual_ALPS best;

    // Run the ALPS GA
    GeneticAlgorithmALPS ga(params);
    ga.optimize(genome_length, best, circuit_performance, Circuit::check_validity, params, false, false);

    std::cout << "\nBest Fitness: " << circuit_performance(genome_length, best.vector.data()) << std::endl;
    std::cout << std::endl;
}


// -----------------------------------------------------------------------------
// Fitness:  sum(beta)  –  0.05 * sum(abs(discrete_gene))
// (pure demo — replace with your real simulator)
// -----------------------------------------------------------------------------
double demo_hybrid_fitness(int topo_len, int* topo,
                    int n_units, double* beta)
{
    double sum_beta = 0.0;
    for(int i = 0; i < n_units; ++i) sum_beta += beta[i];

    int sum_discrete = 0;
    for(int i = 0; i < topo_len; ++i) sum_discrete += std::abs(topo[i]);

    return sum_beta - 0.05 * sum_discrete;
}

// Pretty-print helpers
/**
 * @brief Pretty-print a vector of integers.
 * @param v Vector to print.
 */
void print_vec(const std::vector<int>& v)
{ for(int x : v) std::cout << x << ' '; }

/**
 * @brief Pretty-print a vector of doubles (beta values).
 * @param b Vector to print.
 */
void print_beta(const std::vector<double>& b)
{ std::cout<<std::fixed<<std::setprecision(3);
  for(double x : b) std::cout << x << ' '; }

/**
 * @brief Run a hybrid genetic algorithm optimization (topology + beta).
 * @details This function demonstrates a hybrid GA that optimizes both discrete topology and continuous beta parameters.
 */
void runHybridGA() {
    std::cout << "=== Hybrid (topology + beta) GA demo ===\n\n";

    // Construct the GA engine
    GeneticAlgorithm ga;

    // -------------------------------------------------------
    // Set GA hyperparameters
    // -------------------------------------------------------
    Algorithm_Parameters params{};
    params.max_iterations   = 500;
    params.stall_iterations = 100;
    params.cross_prob       = 0.7;   // SBX crossover prob
    params.mutate_prob_real = 0.2;   // Gaussian mutation prob
    params.pop_size         = 100;
    params.number_of_units  = 10;     // n = number of β genes
    params.tournament_size  = 5;
    params.real_sigma       = 0.1;   // σ for Gaussian mutation

    int n = params.number_of_units;
    int len = 2*n + 1;            // length of discrete chromosome

    // -------------------------------------------------------
    // 6) Prepare the Individual: fixed topo + empty β vector
    // -------------------------------------------------------
    Individual indy;
    indy.vector.resize(len);
    indy.beta.resize(n, 0.0);         // will be optimized

    ga.optimize_hybrid(
        len,                     // topo length
        indy,                  // Individual to hold result
        circuit_performance,  // fitness callback
        // Circuit::check_validity,             // validity callback
        Circuit::check_validity,             // validity callback
        params                     // GA hyperparameters
    );

    std::cout << "\n\n=== Final result Hybrid ===\n";
    std::cout << "\nBest topology: ";
    print_vec(indy.vector);
    std::cout << "\nBest beta:     ";
    print_beta(indy.beta);
    std::cout << "\nBest fitness = " << indy.fitness << "\n";
}