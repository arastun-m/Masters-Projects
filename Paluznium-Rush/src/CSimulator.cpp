/**
 * @file CSimulator.cpp
 * @brief Implements simulation and evaluation functions for mineral processing circuits.
 */

#include "CUnit.h"
#include "CCircuit.h"
#include "CSimulator.h"

#include "simulate_flow.h" // required for simulate_flow() and product arrays

#include <cmath>

/**
 * @brief Default simulator parameters.
 */
struct Simulator_Parameters default_simulator_parameters = {1e-10, 100};

/**
 * @brief Example answer vector for dummy performance calculation.
 */
int fake_answer_vector[] = {1, 2, 3, 0, 3, 4, 3, 0, 6};

// -------------------- Profit Calc Function --------------------

// Profit calculation based on pricing, penalties, and operating cost
/**
 * @brief Compute the profit from output streams.
 *
 * Calculates profit based on product pricing, penalties, and operating cost.
 *
 * @param out OutputStreams structure containing product and waste quantities.
 * @return Calculated profit.
 */
double compute_profit(const OutputStreams &out)
{
  // Product Stream 1 (Palusznium)
  double value1 = (+120.0 * out.palusznium1) + (-20.0 * out.gormanium1) + (-300.0 * out.waste1);

  // Product Stream 2 (Gormanium)
  double value2 = (+80.0 * out.gormanium2) + (-25.0 * out.waste2);

  // cost calculation
  double cost = 5.0 * std::pow(out.total_volume, 2.0 / 3.0);
  if (out.total_volume >= 150.0)
  {
    cost += 1000.0 * std::pow(out.total_volume - 150.0, 2.0);
  }

  double profit = value1 + value2 - cost;
  return profit;
}
// ---------------------

/**
 * @brief Evaluate the performance of a circuit.
 *
 * This function simulates the circuit described by the input vector and unit parameters,
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
                           struct Simulator_Parameters simulator_parameters)
{
  // This function takes a circuit vector and returns a performance value.
  // The current version of the function is a dummy function that returns
  //  a performance value based on how close the circuit vector is to a predetermined answer vector.

  //   double performance = 0.0;
  //   for (int i=0;i<vector_size;i++) {
  //     //dummy_answer_vector is a predetermined answer vector (same size as circuit_vector)
  //     performance += (20-std::abs(circuit_vector[i]-fake_answer_vector[i]))*100.0;
  //   }

// =============== SIMULATOR INTEGRATION ===============
    int num_units = (vector_size - 1) / 2;
    int feed_index = circuit_vector[0];  // Feed unit index

    // Volume range as per the specification
    const double V_min = 2.5;
    const double V_max = 20.0;

    // Convert vector into Circuit structure
    Circuit circ(num_units);
    circ.Parse_vector(std::vector<int>(circuit_vector, circuit_vector + vector_size));

    // Initialize simulator units
    std::vector<Unit> sim_units;
    sim_units.reserve(circ.units.size());

    double total_volume = 0.0;

    // For each unit, calculate its volume based on the beta parameter
    for (int i = 0; i < num_units; ++i) {
        double volume = 10.0;  // Default

        if (unit_parameters != nullptr && i < unit_parameters_size) {
            double beta = std::clamp(unit_parameters[i], 0.0, 1.0);
            volume = V_min + (V_max - V_min) * beta;
            // std::cout << "Unit " << i << ": beta = " << beta << ", volume = " << volume << std::endl;
        } else {
            // std::cout << "Unit " << i << ": using default volume = " << volume << std::endl;
        }

        total_volume += volume;

        // Set volume directly into CUnit
        circ.units[i].setVolume(volume);

        sim_units.emplace_back(circ.units[i].conc_num,
                            circ.units[i].tails_num,
                            circ.units[i]);
    }

    for (int i = num_units; i < num_units + 3; ++i) {
        sim_units.emplace_back(circ.units[i].conc_num,
                            circ.units[i].tails_num,
                            circ.units[i]);
    }

    // Run the simulation with parsed units and parameters
    OutputStreams out = {};

    out.total_volume = num_units * 10.0; 
  
    simulate_flow(sim_units,
                feed_index,
                out,
                simulator_parameters.max_iterations,
                simulator_parameters.tolerance,
                false);

  // =============DEBUG PRINT=====================================
  // std::cout << "\n=== Final Product Streams ===\n";
  // std::cout << "Palusznium: "
  //           << palusznium_product[0] << " pal, "
  //           << palusznium_product[1] << " gor, "
  //           << palusznium_product[2] << " waste\n";
  //=============DEBUG PRINT=====================================
  // std::cout << "\n=== Final Product Streams ===\n";
  // std::cout << "Palusznium: "
  //           << palusznium_product[0] << " pal, "
  //           << palusznium_product[1] << " gor, "
  //           << palusznium_product[2] << " waste\n";

  // std::cout << "Gormanium:  "
  //           << gormanium_product[0] << " pal, "
  //           << gormanium_product[1] << " gor, "
  //           << gormanium_product[2] << " waste\n";

  // std::cout << "Tailings:   "
  //           << tailings_product[0]  << " pal, "
  //           << tailings_product[1]  << " gor, "
  //           << tailings_product[2]  << " waste\n";

double profit = compute_profit(out);
// if (simulator_parameters.export_data) {
//     std::cout << "Exporting unit data\n";
//     export_unit_data(unit_feeds, unit_concentrates, unit_tailings);
//     std::cout << "Exporting performance data\n";
//     export_performance_data(num_units, circuit_vector, 
//                             simulator_parameters.circuit_feed, profit, palusznium_product, 
//                             gormanium_product,tailings_product);              
// }

  //Calculate final profit using actual simulation output
  return profit;
  // =============================================================
}

// overloads (delete if not needed)
/**
 * @brief Overload: Evaluate circuit performance with default simulator parameters.
 *
 * @param vector_size Size of the circuit vector.
 * @param circuit_vector Pointer to the circuit vector.
 * @param unit_parameters_size Size of the unit parameters array.
 * @param unit_parameters Pointer to the unit parameters array.
 * @return Performance value (profit).
 */
double circuit_performance(int vector_size, int *circuit_vector,
                           int unit_parameters_size, double *unit_parameters)
{
  return circuit_performance(vector_size, circuit_vector,
                             unit_parameters_size, unit_parameters,
                             default_simulator_parameters);
};

/**
 * @brief Overload: Evaluate circuit performance with only the circuit vector.
 *
 * @param vector_size Size of the circuit vector.
 * @param circuit_vector Pointer to the circuit vector.
 * @return Performance value (profit).
 */
double circuit_performance(int vector_size, int* circuit_vector) {
    return circuit_performance(vector_size, circuit_vector, 
                            0, nullptr,
                            default_simulator_parameters);
}
// Other functions and variables to evaluate a real circuit.