/**
 * @file test_mutate_crossover.cpp
 * @brief Unit tests for mutation and crossover operations in the genetic algorithm.
 *
 * This file tests the mutate and crossover functions of the GeneticAlgorithm class.
 * It prints the results before and after mutation, and shows the offspring from crossover.
 */

/*
g++ -std=c++11 \             
    -Iinclude \
    src/Genetic_Algorithm.cpp \
    tests/test_mutate_crossover.cpp \
    -o tests/test_mutate_crossover
./tests/test_mutate_crossover
*/

#include <iostream>
#include <vector>

// expose private/protected members for testing
#define private public
#define protected public
#include "Genetic_Algorithm.h"
#undef private
#undef protected

// print a vector of integers
/**
 * @brief Print a vector of integers to standard output.
 * @param v Vector of integers to print.
 */
void print_vector(const std::vector<int>& v) {
    for (auto x : v) std::cout << x << ' ';
    std::cout << '\n';
}

/**
 * @brief Main function to test mutation and crossover operations.
 * @return Exit code.
 */
int main() {
    GeneticAlgorithm ga;

    // set up the parameters
    int n = 4;                      // unit number
    ga.params_.circuit_size = n;
    ga.params_.mutate_prob  = 0;
    ga.params_.cross_prob   = 1.0;

    int len = 2*n + 1;

    // --- test mutate ---
    Individual ind;
    ind.vector.resize(len);
    // generate a simple individual
    for (int i = 0; i < len; ++i) ind.vector[i] = i;

    std::cout << "Before mutate: ";
    print_vector(ind.vector);

    Individual mutated = ga.mutate(ind);
    std::cout << " After mutate: ";
    print_vector(mutated.vector);

    // --- test crossover ---
    Individual p1, p2;
    p1.vector.resize(len);
    p2.vector.resize(len);
    // p1: 0,1,2,...  p2: len,len-1,...
    for (int i = 0; i < len; ++i) {
        p1.vector[i] = i;
        p2.vector[i] = len - i;
    }

    std::cout << "\nParent1:       ";
    print_vector(p1.vector);
    std::cout << "Parent2:       ";
    print_vector(p2.vector);

    auto offspring = ga.crossover(p1, p2);
    std::cout << "Child1 (X+M):  ";
    print_vector(offspring.first.vector);
    std::cout << "Child2 (X+M):  ";
    print_vector(offspring.second.vector);

    return 0;
}