/**
 * @file CCircuit.cpp
 * @brief Implementation of the Circuit class and related utilities for circuit validation and parsing.
 * @details
 * This file implements the Circuit class which represents a processing circuit with multiple units.
 * It includes functionality for:
 * - Circuit construction and initialization
 * - Vector parsing and validation
 * - Unit marking and accessibility checking
 * - Circuit validity verification
 */

#include <vector>
 
#include <stdio.h>
#include <CUnit.h>
#include <CCircuit.h>

#include <stack>
#include <algorithm>
#include <queue>
#include <bitset> 
#include <iostream>

/**
 * @brief Construct a new Circuit object.
 * @param num_units Number of processing units in the circuit.
 * @details
 * Initializes a new circuit with the specified number of units.
 * Reserves space for the units vector including terminal nodes.
 */
Circuit::Circuit(int num_units)
  : num_units(num_units),
    exit_reached(false)
{
    units.clear();
    units.reserve(num_units + 3);
}

/**
 * @brief Parse a vector representation of the circuit and initialize units.
 * @param circuit_vector The vector encoding the circuit structure.
 * @details
 * Processes the input vector to:
 * - Initialize processing units with their connections
 * - Set up terminal nodes (Palusznium, Gormanium, Waste)
 * - Configure unit properties and connections
 */
void Circuit::Parse_vector(const std::vector<int>& circuit_vector) {
    int vector_size = circuit_vector.size();
   
    // Initialize each unit based on the input vector
    for (int i = 0; i < num_units; i++) {
        // Extract concentrate and tailings destinations
        int conc_dest = circuit_vector[2 * i + 1];
        int tail_dest = circuit_vector[2 * i + 2];
       
        // Initialize unit
        units.emplace_back();
        units[i].conc_num = conc_dest;
        units[i].tails_num = tail_dest;
        // Reset visited flag
        units[i].mark = false;  
    }
   
    // Initialize terminal nodes
    // Palusznium
    units.emplace_back();  
    units[num_units].conc_num = -1;
    units[num_units].tails_num = -1;
    units[num_units].mark = false;
   
    // Gormanium
    units.emplace_back();  
    units[num_units + 1].conc_num = -1;
    units[num_units + 1].tails_num = -1;
    units[num_units + 1].mark = false;
   
    // Tailings
    units.emplace_back();
    units[num_units + 2].conc_num = -1;
    units[num_units + 2].tails_num = -1;
    units[num_units + 2].mark = false;
}

// -- constructor --------
// construct the required size directly to avoid additional operations
// Circuit::Circuit(int n) : units(n), exit_reached(false) {}

// -- utilities --------
// expected vector size

/**
 * @brief Compute the expected size of the circuit vector for a given number of units.
 * @param n_units Number of processing units.
 * @return Expected vector size.
 * @details
 * Calculates the required vector size based on the number of units:
 * - Each unit requires 2 entries (concentrate and tailings destinations)
 * - Plus 1 entry for the feed unit
 */
static inline int expected_vector_size(int n_units) {
    return 1 + 2 * n_units;
}

// clear mark

/**
 * @brief Reset the mark flag for all units in the circuit.
 * @param units Vector of CUnit objects to reset.
 * @details
 * Iterates through all units and resets their mark flags to false.
 * Used in circuit validation to track unit accessibility.
 */
static void reset_marks(std::vector<CUnit>& units) {
    for (auto& u : units) u.resetMark();
}

// ennum type

/**
 * @enum Outlet
 * @brief Enumeration for outlet types in the circuit.
 * @details
 * Defines the three possible outlet types:
 * - PALU: Palusznium outlet
 * - GORM: Gormanium outlet
 * - WASTE: Waste outlet
 */
enum Outlet : int { PALU = 0, GORM = 1, WASTE = 2 };


// DFS
/**
 * @brief Depth-first search to mark all reachable units from a starting unit.
 * @param unit_num Index of the starting unit.
 * @details
 * Recursively marks all units that can be reached from the starting unit.
 * Updates exit_reached flag if any path leads to an exit.
 */
void Circuit::mark_units(int unit_num)
{
    if (units[unit_num].mark) return;
    units[unit_num].mark = true;

    int c = units[unit_num].conc_num;
    if (c >= 0 && c < num_units) {
        mark_units(c);
    } else {
        exit_reached = true;
    }

    int t = units[unit_num].tails_num;
    if (t >= 0 && t < num_units) {
        mark_units(t);
    } else {
        exit_reached = true;
    }
}


// check validity
/**
 * @brief Check the validity of a circuit vector.
 * @param vec_size Size of the circuit vector.
 * @param vec Pointer to the circuit vector.
 * @return true if the circuit is valid, false otherwise.
 * @details
 * Performs comprehensive validation of the circuit:
 * - Checks vector size and format
 * - Verifies unit connections
 * - Ensures no self-circulation
 * - Validates pathway accessibility
 * - Confirms multiple exit paths
 */
bool Circuit::check_validity(int vec_size, int* vec)
{
    if (vec_size < 3 || (vec_size - 1) % 2 != 0) return false;
    int n_units = (vec_size - 1) / 2;
    if (vec_size != expected_vector_size(n_units)) return false;

    int feed = vec[0];
    if (feed < 0 || feed >= n_units) return false;

    // check number is illegal
    for (int i = 1; i < vec_size; ++i) {
        if (vec[i] < 0 || vec[i] > n_units + 2) {
            return false;
        }
    }

    // tmp.units
    std::vector<int> v(vec, vec + vec_size);
    Circuit tmp(n_units);
    tmp.Parse_vector(v);  
    // now tmp.units.size() == n_units + 3
    
     // conc==i / tails==i / conc==tails 
    for (int i = 0; i < n_units; ++i) {
        int conc  = tmp.units[i].conc_num;
        int tails = tmp.units[i].tails_num;
        // prohibit self-circulation
        if (conc == i || tails == i) return false;
        // prohibit the same target
        if (conc == tails)        return false;
    }

    // accessibility of pathways
    reset_marks(tmp.units);
    tmp.exit_reached = false;
    tmp.mark_units(feed);
    // not a single road leading to any exit
    if (!tmp.exit_reached) 
        return false;
    for (int i = 0; i < n_units; ++i) {
        if (!tmp.units[i].mark) return false;
    }

    // at least two different exits
    // bitset[0]=Palu, [1]=Gorm, [2]=Waste
    std::vector<std::bitset<3>> reach(n_units);
    // two direct ways out for each unit
    for (int i = 0; i < n_units; ++i) {
        int c = tmp.units[i].conc_num;
        int t = tmp.units[i].tails_num;
        if (c == n_units) reach[i].set(0);
        else if (c == n_units + 1) reach[i].set(1);
        else if (c == n_units + 2) reach[i].set(2);
        if (t == n_units) reach[i].set(0);
        else if (t == n_units + 1) reach[i].set(1);
        else if (t == n_units + 2) reach[i].set(2);
    }
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < n_units; ++i) {
            auto before = reach[i];
            int c = tmp.units[i].conc_num;
            int t = tmp.units[i].tails_num;
            if (c >= 0 && c < n_units) reach[i] |= reach[c];
            if (t >= 0 && t < n_units) reach[i] |= reach[t];
            if (reach[i] != before) changed = true;
        }
    } while (changed);
    // each unit is connected to at least two different exits
    for (int i = 0; i < n_units; ++i) {
        if (reach[i].count() < 2) return false;
    }
    return true;
}


// with parameter

/**
 * @brief Check the validity of a circuit vector with additional parameters.
 * @param vec_size Size of the circuit vector.
 * @param vec Pointer to the circuit vector.
 * @param param_size Size of the parameters array.
 * @param parameters Pointer to the parameters array.
 * @return true if the circuit and parameters are valid, false otherwise.
 * @details
 * Extends basic circuit validation to include parameter validation:
 * - Verifies parameter count matches unit count
 * - Ensures all parameters are within valid range [0,1]
 */
bool Circuit::check_validity(int vec_size, int* vec,
    int param_size, double* parameters)
{
    if (!check_validity(vec_size, vec)) return false;
    
    int n_units = (vec_size - 1) / 2;
    if (param_size != n_units) return false;

    for (int i = 0; i < param_size; ++i) {
        double b = parameters[i];
        if (b < 0.0 || b > 1.0) return false;
    }
    return true;
}