/**
 * @file Genetic_Algorithm_ALPS.cpp
 * @brief Implementation of the Genetic Algorithm with Age-Layered Population Structure (ALPS).
 */

#include "performance_utilities.h"


// standard directory and filenames
std::string GeneticAlgorithmUtils::directory = "performance_data";
std::string GeneticAlgorithmUtils::meta_data_filename = directory + "/meta_data.csv";
std::string GeneticAlgorithmUtils::fitness_values_filename = directory + "/fitness_values.csv";
std::string GeneticAlgorithmUtils::population_diversity_filename = directory + "/population_diversity.csv";
std::string GeneticAlgorithmUtils::best_individual_progression_filename = directory + "/best_individual_progression.csv";
std::string GeneticAlgorithmUtils::age_layer_distribution_filename = directory + "/age_layer_distribution.csv";
std::string GeneticAlgorithmUtils::best_fitness_per_layer_filename = directory + "/fitness_layer_distribution.csv";

/**
 * @brief Write fitness values of all individuals per generation to a file. Each line represents a generation.
 * @param fitnessValues 2D vector of fitness values [generation][individual].
 */
void GeneticAlgorithmUtils::writeFitnessValues(const std::vector<std::vector<double>> &fitnessValues)
{
    std::ofstream file(fitness_values_filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << fitness_values_filename << std::endl;
        return;
    }

    for (const auto &generation : fitnessValues)
    {
        for (const auto &fitness : generation)
        {
            file << fitness << ",";
        }
        file << "\n";
    }
    file.close();
}

/**
 * @brief Write population diversity (unique individuals count) per generation to a file.
 * @param populationDiversity Vector of diversity values per generation.
 */
void GeneticAlgorithmUtils::writePopulationDiversity(const std::vector<double> &populationDiversity)
{
    std::ofstream file(population_diversity_filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << population_diversity_filename << std::endl;
        return;
    }
    for (const auto &diversity : populationDiversity)
    {
        file << diversity << "\n";
    }
    file.close();
}

/**
 * @brief Write the best individual progression (best fitness per generation) to a file. Each line represents a generation.
 * @param bestIndividuals Vector of best Individual objects per generation.
 */
void GeneticAlgorithmUtils::writeBestIndividualProgression(const std::vector<Individual> &bestIndividuals)
{
    std::ofstream file(best_individual_progression_filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << best_individual_progression_filename << std::endl;
        return;
    }
    for (const auto &ind : bestIndividuals)
    {
        file << ind.fitness << ",";
        for (const auto &gene : ind.vector)
        {
            file << gene << ",";
        }
        file << "\n";
    }
    file.close();
}

/**
 * @brief Write the best individual progression (best fitness per generation) to a file (ALPS version).
 * Each line represents a generation.
 * @param bestIndividuals Vector of best Individual_ALPS objects per generation.
 */
void GeneticAlgorithmUtils::writeBestIndividualProgression(const std::vector<Individual_ALPS> &bestIndividuals)
{
    // write it filtered-out so that every line is unique (only the changes in best individuals are captured)
    std::vector<Individual_ALPS> unique_best_individuals;
    // filter out the unique best individuals
    for (const auto &ind : bestIndividuals)
    {
        if (unique_best_individuals.empty() || unique_best_individuals.back().fitness != ind.fitness)
        {
            unique_best_individuals.push_back(ind);
        }
    }

    // write the unique best individuals to the file
    std::ofstream file(best_individual_progression_filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << best_individual_progression_filename << std::endl;
        return;
    }
    for (const auto &ind : unique_best_individuals)
    {
        file << ind.fitness << ",";
        for (const auto &gene : ind.vector)
        {
            file << gene << ",";
        }
        file << "\n";
    }
    file.close();
}

/**
 * @brief Write the meta data (hyper-parameter values) to a file.
 * Each line represents a hyper-parameter value.
 * @param filename Output file name.
 * @param metaData Vector of meta data values.
 */
void GeneticAlgorithmUtils::writeMetaData(const std::string &filename, const std::vector<std::string> &metaData)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    for (const auto &data : metaData)
    {
        file << data << "\n";
    }
    file.close();
}

/**
 * @brief Write the meta data (hyper-parameter values) to a file from Algorithm_Parameters.
 * @param params Algorithm_Parameters object.
 */
void GeneticAlgorithmUtils::writeMetaData(Algorithm_Parameters &params)
{
    /**
     * Writes the meta data (hyper-parameter values) into a file.
     * First converts the Algoirithm_Parameters struct to a vector of strings.
     * Then calls the writeMetaData function to write the vector to a file.
     * 
     * @param params - algorithm parameters
     */

    std::vector<std::string> meta_data;
    meta_data.push_back(std::to_string(params.max_iterations));
    meta_data.push_back(std::to_string(params.stall_iterations));
    meta_data.push_back(std::to_string(params.cross_prob));
    meta_data.push_back(std::to_string(params.mutate_prob));
    meta_data.push_back(std::to_string(params.pop_size));
    meta_data.push_back(std::to_string(params.number_of_units));
    meta_data.push_back(std::to_string(params.tournament_size));
    meta_data.push_back(std::to_string(params.tournament_rate));

    writeMetaData(meta_data_filename, meta_data);
}

/**
 * @brief Create the output directory if it does not exist.
 */
void GeneticAlgorithmUtils::createDirectory() {
    std::string command = "mkdir -p " + directory;
    system(command.c_str());
}

/**
 * @brief Write age layer distribution of the individuals (ALPS only) to a file.
 * Each line represents a generation.
 * Each line contains the comma-separated values of the age layer distribution of each layer (e.g., 10 layers = 10 values).
 * @param ageLayerDistribution 2D vector of age layer distribution [generation][layer].
 */
void GeneticAlgorithmUtils::writeAgeLayerDistribution(const std::vector<std::vector<double>> &ageLayerDistribution)
{
    std::ofstream file(age_layer_distribution_filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << age_layer_distribution_filename << std::endl;
        return;
    }
    for (const auto &generation : ageLayerDistribution)
    {
        for (const auto &layer : generation)
        {
            file << layer << ",";
        }
        file << "\n";
    }
    file.close();
}

/**
 * @brief Write the best fitness of each layer per generation (ALPS only) to a file.
 * Each line represents a generation.
 * Each line contains the comma-separated values of the best fitness of each layer (e.g., 10 layers = 10 values).
 * @param bestFitnessPerLayer 2D vector of best fitness values [generation][layer].
 */
void GeneticAlgorithmUtils::writeBestFitnessPerLayer(const std::vector<std::vector<double>> &bestFitnessPerLayer)
{
    std::ofstream file(best_fitness_per_layer_filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << best_fitness_per_layer_filename << std::endl;
        return;
    }
    for (const auto &generation : bestFitnessPerLayer)
    {
        for (const auto &layer : generation)
        {
            file << layer << ",";
        }
        file << "\n";
    }
    file.close();
}
