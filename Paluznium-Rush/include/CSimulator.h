/**
 * @file CSimulator.h
 * @brief Header file for the circuit simulator.
 *
 * This header file defines the data structures and functions used to evaluate
 * the performance of mineral processing circuits.
 */

#pragma once

// added to reuse in runs
/**
 * @struct OutputStreams
 * @brief Stores the output streams for each product and waste from the circuit.
 */
struct OutputStreams {
    double palusznium1, gormanium1, waste1;
    double palusznium2, gormanium2, waste2;
    double tailings_pal, tailings_gor, tailings_waste;
    double total_volume;
};

/**
 * @struct Simulator_Parameters
 * @brief Parameters controlling simulation accuracy and iteration.
 */
struct Simulator_Parameters{
    double tolerance;
    int max_iterations;
    bool export_data;
    // other parameters for your circuit simulator       
};

/**
 * @brief Evaluate the performance of a circuit.
 *
 * Simulates the circuit described by the input vector and unit parameters,
 * then computes a performance value (profit) based on the simulation output.
 *
 * @param vector_size Size of the circuit vector.
 * @param circuit_vector Pointer to the circuit vector.
 * @param unit_parameters_size Size of the unit parameters array.
 * @param unit_parameters Pointer to the unit parameters array (beta values).
 * @param simulator_parameters Parameters controlling simulation accuracy and iteration.
 * @return Performance value (profit) for the given circuit and parameters.
 */
double circuit_performance(int vector_size, int *circuit_vector,
    int unit_parameters_size, double *unit_parameters,
    struct Simulator_Parameters simulator_parameters);

/**
 * @brief Overload: Evaluate circuit performance with default unit parameters.
 * @param vector_size Size of the circuit vector.
 * @param circuit_vector Pointer to the circuit vector.
 * @param simulator_parameters Parameters controlling simulation accuracy and iteration.
 * @return Performance value (profit).
 */
double circuit_performance(int vector_size, int *circuit_vector, struct Simulator_Parameters simulator_parameters);

/**
 * @brief Overload: Evaluate circuit performance with default simulator parameters.
 * @param vector_size Size of the circuit vector.
 * @param circuit_vector Pointer to the circuit vector.
 * @param unit_parameters_size Size of the unit parameters array.
 * @param unit_parameters Pointer to the unit parameters array.
 * @return Performance value (profit).
 */
double circuit_performance(int vector_size, int *circuit_vector,
    int unit_parameters_size, double *unit_parameters);

/**
 * @brief Overload: Evaluate circuit performance with only the circuit vector.
 * @param vector_size Size of the circuit vector.
 * @param circuit_vector Pointer to the circuit vector.
 * @return Performance value (profit).
 */
double circuit_performance(int vector_size, int *circuit_vector);