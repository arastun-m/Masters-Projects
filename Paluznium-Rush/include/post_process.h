/**
 * @file post_process.h
 * @brief Declarations for functions that export simulation results and performance metrics to JSON files.
 */

#include "simulate_flow.h"
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Function to export unit flow data to JSON
/**
 * @brief Export unit flow data to a JSON file for visualization.
 *
 * Writes the feed, concentrate, and tailings streams for each processing unit
 * (excluding the last three dummy units) to a JSON file at "plotting/data/unit_data.json".
 *
 * @param unit_feeds Vector of feed streams for each unit.
 * @param unit_concentrates Vector of concentrate streams for each unit.
 * @param unit_tailings Vector of tailings streams for each unit.
 */
void export_unit_data(std::vector<Stream>& unit_feeds, 
        std::vector<Stream>& unit_concentrates, 
        std::vector<Stream>& unit_tailings);

// Function to export performance data to JSON
/**
 * @brief Export circuit performance data and metrics to a JSON file.
 *
 * Writes the circuit vector, feed composition, profit, recoveries, and grades
 * to a JSON file at "plotting/data/performance_data.json" for further analysis or plotting.
 *
 * @param num_units Number of processing units in the circuit.
 * @param circuit_vector Pointer to the circuit vector.
 * @param feed Feed stream composition.
 * @param profit Calculated profit for the circuit.
 * @param palusznium_product Array containing Palusznium product stream values.
 * @param gormanium_product Array containing Gormanium product stream values.
 * @param tailings_product Array containing tailings stream values.
 */
void export_performance_data(int num_units,
        int* circuit_vector,
        Stream feed,
        double profit,
        double* palusznium_product,
        double* gormanium_product, 
        double* tailings_product);
