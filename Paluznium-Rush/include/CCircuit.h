/**
 * @file CCircuit.h
 * @brief Header for the Circuit class and its associated functions.
 *
 * This header defines the Circuit class, which represents a mineral processing circuit,
 * and provides methods for parsing, validation, and internal traversal.
 */

#pragma once

#include "CUnit.h"

#include <vector>

/**
 * @class Circuit
 * @brief Represents a mineral processing circuit and provides utilities for validation and parsing.
 */
class Circuit {
public:
    /**
     * @brief Construct a new Circuit object.
     * @param num_units Number of processing units in the circuit.
     */
    Circuit(int num_units);
    /**
     * @brief Parse a vector representation of the circuit and initialize units.
     * @param circuit_vector The vector encoding the circuit structure.
     */
    void Parse_vector(const std::vector<int>& circuit_vector);
    /**
     * @brief Check the validity of a circuit vector.
     * @param vector_size Size of the circuit vector.
     * @param vec Pointer to the circuit vector.
     * @return true if the circuit is valid, false otherwise.
     */
    static bool check_validity(int vector_size, int *);
    /**
     * @brief Check the validity of a circuit vector and unit parameters.
     * @param vector_size Size of the circuit vector.
     * @param vec Pointer to the circuit vector.
     * @param unit_parameters_size Size of the unit parameters array.
     * @param unit_parameters Pointer to the unit parameters array.
     * @return true if the circuit and parameters are valid, false otherwise.
     */
    static bool check_validity(int vector_size, int *,
                               int unit_parameters_size, double *unit_parameters);
    /**
     * @brief Vector of CUnit objects representing the units in the circuit.
     */                               
    std::vector<CUnit> units;
    // accept overload for vector
    /**
     * @brief Overload: Check validity using a std::vector<int>.
     * @param vec Circuit vector.
     * @return true if valid, false otherwise.
     */
    static bool check_validity(const std::vector<int>& vec) {
        return check_validity((int)vec.size(), const_cast<int*>(vec.data()));
    }
    /**
     * @brief Overload: Check validity using std::vector<int> and std::vector<double>.
     * @param vec Circuit vector.
     * @param params Unit parameter vector.
     * @return true if valid, false otherwise.
     */
    static bool check_validity(const std::vector<int>& vec,
        const std::vector<double>& params) {
        return check_validity((int)vec.size(),
            const_cast<int*>(vec.data()),
            (int)params.size(),
            const_cast<double*>(params.data()));
    }
  private:
    /**
     * @brief Mark all reachable units from a starting unit (used internally).
     * @param unit_num Index of the starting unit.
     */  
    void mark_units(int unit_num);
    /**
     * @brief Indicates if an exit node has been reached during traversal.
     */
    // newly added
    bool exit_reached;
    /**
     * @brief Number of processing units in the circuit.
     */
    int num_units;
};

