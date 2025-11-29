/**
 * @file performance_utilities.h
 * 
 * In utility.h
 * 
 * 1. Function to write (e.g., .csv file) of fitness values of all individuals per generation (for all generations) into a file
 * 2. Function to write population diversity (unique individuals count) per generation into a file
 * 3. Function to write the meta data (hyper-parameter values) into a file
 * 4. Function to write the best individual progression (best fitness per generation) into a file (for standard and ALPS)
 * 5. Function to create the directory if it does not exist
 * 6. Function to write the best individual progression (best fitness per generation) into a file
 * 7. Function to write the age layer distribution of the individuals (ALPS only) into a file
 * 8. Function to write the best fitness of each layer per generation (ALPS only) into a file
 */

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Genetic_Algorithm.h"
#include "Genetic_Algorithm_ALPS.h"


/**
 * * @brief Utility class for genetic algorithm
 * Static methods for writing fitness values, population diversity, and meta data to files.
 */
class GeneticAlgorithmUtils {
    // standard directory and filenames
    static std::string directory;                        ///< Output directory for result files
    static std::string meta_data_filename;               ///< Meta data file name
    static std::string fitness_values_filename;          ///< Fitness values file name
    static std::string population_diversity_filename;    ///< Population diversity file name
    static std::string best_individual_progression_filename; ///< Best individual progression file name
    static std::string age_layer_distribution_filename;  ///< Age layer distribution file name (ALPS)
    static std::string best_fitness_per_layer_filename;  ///< Best fitness per layer file name (ALPS)

public:
    // writes fitness values of all individuals per generation (for all generations) into a file
    /**
     * @brief Write fitness values of all individuals per generation to a file.
     * @param fitnessValues 2D vector of fitness values [generation][individual].
     */
    static void writeFitnessValues(const std::vector<std::vector<double>> &fitnessValues);
    
    // writes population diversity (unique individuals count) per generation into a file
    /**
     * @brief Write population diversity (unique individuals count) per generation to a file.
     * @param populationDiversity Vector of diversity values per generation.
     */
    static void writePopulationDiversity(const std::vector<double> &populationDiversity);
    
    // writes the best individual progression (best fitness per generation) into a file
    /**
     * @brief Write the best individual progression (best fitness per generation) to a file.
     * @param bestIndividuals Vector of best Individual objects per generation.
     */
    static void writeBestIndividualProgression(const std::vector<Individual> &bestIndividuals);
    
    /**
     * @brief Write the best individual progression (best fitness per generation) to a file (ALPS version).
     * @param bestIndividuals Vector of best Individual_ALPS objects per generation.
     */    
    static void writeBestIndividualProgression(const std::vector<Individual_ALPS> &bestIndividuals);

    // writes the meta data (hyper-parameter values) into a file
    /**
     * @brief Write the meta data (hyper-parameter values) to a file.
     * @param filename Output file name.
     * @param metaData Vector of meta data strings.
     */
    static void writeMetaData(const std::string &filename, const std::vector<std::string> &metaData);
    
    /**
     * @brief Write the meta data (hyper-parameter values) to a file from Algorithm_Parameters.
     * @param params Algorithm_Parameters object.
     */
    static void writeMetaData(Algorithm_Parameters &params);

    // create the directory if it does not exist
    /**
     * @brief Create the output directory if it does not exist.
     */
    static void createDirectory();

     /* ----- ALPS ONLY ----- */
    // write age layer distribution of the individuals (ALPS only) into a file
    /**
     * @brief Write age layer distribution of the individuals (ALPS only) to a file.
     * @param ageLayerDistribution 2D vector of age layer distribution [generation][layer].
     */
    static void writeAgeLayerDistribution(const std::vector<std::vector<double>> &ageLayerDistribution);
    
    // write the best fitness of each layer per generation (ALPS only) into a file
    /**
     * @brief Write the best fitness of each layer per generation (ALPS only) to a file.
     * @param bestFitnessPerLayer 2D vector of best fitness values [generation][layer].
     */
    static void writeBestFitnessPerLayer(const std::vector<std::vector<double>> &bestFitnessPerLayer);
};
