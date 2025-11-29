/**
 * @file config.cpp
 * @brief Implementation for loading algorithm configuration parameters from a JSON file.
 */

#include "config.h"

/**
 * @brief Load multiple algorithm configurations from a JSON file.
 *
 * This function reads a JSON file containing one or more configuration objects
 * and returns a vector of Algorithm_Parameters. If the file contains a single
 * object, it is wrapped in an array for consistency.
 *
 * @param filename Path to the JSON configuration file.
 * @return std::vector<Algorithm_Parameters> Vector of loaded configuration parameters.
 * @throws std::runtime_error If the file cannot be opened.
 */

// NEW: load several configurations instead of just one
std::vector<Algorithm_Parameters> load_all_configs(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }

    nlohmann::json j;
    file >> j;

    std::vector<Algorithm_Parameters> configs;

    // NEW: If single object is passed, wrap in array for consistency
    if (j.is_object()) {
        j = nlohmann::json::array({j});
    }

    for (const auto& entry : j) {
        Algorithm_Parameters params;

        // fallback default vals in case left empty 
        params.max_iterations    = entry.value("max_iterations", 1000);
        params.stall_iterations  = entry.value("stall_iterations", 30);
        params.cross_prob        = entry.value("cross_prob", 0.5);
        params.mutate_prob       = entry.value("mutate_prob", 0.05);
        params.pop_size          = entry.value("pop_size", 100);
        params.number_of_units   = entry.value("number_of_units", 4);
        params.tournament_size   = entry.value("tournament_size", 3);

        configs.push_back(params);
    }

    return configs;
}
