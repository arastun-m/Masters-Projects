/**
 * @file test_hybrid_mutate_crossover.cpp
 * @brief Unit tests for hybrid mutation and crossover operations in the genetic algorithm.
 *
 * This file tests the mutate_hybrid and crossover_hybrid functions of the GeneticAlgorithm class,
 * demonstrating their effects on both discrete (topology) and continuous (beta) chromosomes.
 *
 * Example build and run:
 * g++ -std=c++17 \
 *     -Iinclude \
 *     src/Genetic_Algorithm.cpp \
 *     tests/test_hybrid_mutate_crossover.cpp \
 *     -o tests/test_hybrid_mutate_crossover
 * ./tests/test_hybrid_mutate_crossover
 */

#include <iostream>
#include <vector>
#include <iomanip>

// ─── Expose private data for quick testing (not for production!) ───────────────
#define private public
#define protected public
#include "Genetic_Algorithm.h"
#undef private
#undef protected
// ───────────────────────────────────────────────────────────────────────────────

/**
 * @brief Pretty-print a vector of integers (topology chromosome).
 * @param v Vector of integers to print.
 */
void print_topo(const std::vector<int>& v)
{
    for (int g : v) std::cout << g << ' ';
}

/**
 * @brief Pretty-print a vector of doubles (beta chromosome).
 * @param b Vector of doubles to print.
 */
void print_beta(const std::vector<double>& b)
{
    std::cout << std::fixed << std::setprecision(3);
    for (double x : b) std::cout << x << ' ';
}

/**
 * @brief Main function to test hybrid mutation and crossover operations.
 * @return Exit code.
 */
int main()
{
    std::cout << "=== Hybrid GA: mutate & crossover demo ===\n\n";

    // ── GA engine + parameter tuning just for the test ────────────────────────
    GeneticAlgorithm ga;
    ga.params_.number_of_units  = 4;   // n
    ga.params_.cross_prob       = 1.0; // force crossover
    ga.params_.mutate_prob      = 0; // force discrete mutation
    ga.params_.mutate_prob_real = 1.0; // force beta mutation
    ga.params_.real_sigma       = 0.2; // visible Gaussian noise

    int n   = ga.params_.number_of_units;
    int len = 2*n + 1;                 // discrete chromosome length

    // ── Construct two parents ────────────────────────────────────────────────
    Individual p1, p2;
    p1.vector.resize(len);
    p2.vector.resize(len);
    p1.beta.resize(n);
    p2.beta.resize(n);

    // Parent 1 : topo = 0..len-1  ,  beta = 0.1,0.2,0.3,0.4
    // Parent 2 : topo = len-1..0  ,  beta = 0.9,0.8,0.7,0.6
    for (int i = 0; i < len; ++i) {
        p1.vector[i] = i;
        p2.vector[i] = len - 1 - i;
    }
    for (int i = 0; i < n; ++i) {
        p1.beta[i] = 0.1 * (i + 1);
        p2.beta[i] = 1.0 - 0.1 * (i + 1);
    }

    // ── Show parents ─────────────────────────────────────────────────────────
    std::cout << "Parent 1 topo: "; print_topo(p1.vector); std::cout << "\n";
    std::cout << "Parent 1 beta: "; print_beta(p1.beta);   std::cout << "\n\n";

    std::cout << "Parent 2 topo: "; print_topo(p2.vector); std::cout << "\n";
    std::cout << "Parent 2 beta: "; print_beta(p2.beta);   std::cout << "\n\n";

    // ── Mutate Parent 1 ──────────────────────────────────────────────────────
    Individual mut = ga.mutate_hybrid(p1);
    std::cout << "After mutate_hybrid(p1):\n";
    std::cout << "  topo: "; print_topo(mut.vector); std::cout << "\n";
    std::cout << "  beta: "; print_beta(mut.beta);   std::cout << "\n\n";

    // ── Crossover P1 × P2 ───────────────────────────────────────────────────
    auto offspring = ga.crossover_hybrid(p1, p2);
    const Individual& c1 = offspring.first;
    const Individual& c2 = offspring.second;

    std::cout << "Offspring from crossover_hybrid:\n";
    std::cout << "  Child 1 topo: "; print_topo(c1.vector); std::cout << "\n";
    std::cout << "  Child 1 beta: "; print_beta(c1.beta);   std::cout << "\n";
    std::cout << "  Child 2 topo: "; print_topo(c2.vector); std::cout << "\n";
    std::cout << "  Child 2 beta: "; print_beta(c2.beta);   std::cout << "\n";
}