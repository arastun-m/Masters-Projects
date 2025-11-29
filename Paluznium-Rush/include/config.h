/**
 * @file config.h
 * @brief Declarations for loading algorithm configuration parameters from JSON files.
 */
#pragma once
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Genetic_Algorithm.h"

// NEW: load several configurations instead of just one
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
std::vector<Algorithm_Parameters> load_all_configs(const std::string& filename);
// OLD: (single config only):
/*
inline Algorithm_Parameters load_config(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }

    nlohmann::json j;
    file >> j;

    Algorithm_Parameters params;
    params.max_iterations    = j.value("max_iterations", 1000);
    params.stall_iterations  = j.value("stall_iterations", 30);
    params.cross_prob        = j.value("cross_prob", 0.5);
    params.mutate_prob       = j.value("mutate_prob", 0.05);
    params.pop_size          = j.value("pop_size", 100);
    params.number_of_units   = j.value("number_of_units", 4);
    params.tournament_size   = j.value("tournament_size", 3);
    return params;
}
*/
