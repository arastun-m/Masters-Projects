"""
This script is designed to load and visualize the performance data of a genetic algorithm.
"""

from collections import namedtuple
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

""" ========================== DATA LOADING FUNCTIONS ========================== """
hyperparameters_filename = "../build/performance_data/meta_data.csv"
fitness_values_filename = "../build/performance_data/fitness_values.csv"
population_diversity_filename = "../build/performance_data/population_diversity.csv"
age_distribution_filename = "../build/performance_data/age_layer_distribution.csv"
fitness_distirbution_filename = "../build/performance_data/fitness_layer_distribution.csv"

AlgorithmParameters = namedtuple("AlgorithmParameters", [
    "max_iterations",
    "stall_iterations",
    "cross_prob",
    "mutate_prob",
    "pop_size",
    "number_of_units",
    "tournament_size",
    "tournament_rate"
])

def load_hyperparameters(filename: str) -> AlgorithmParameters:
    with open(filename, "r") as file:
        lines = [line.strip() for line in file if line.strip()]
    
    if len(lines) != 8:
        raise ValueError(f"Expected 8 parameters, found {len(lines)}")

    return AlgorithmParameters(
        int(lines[0]),
        int(lines[1]),
        float(lines[2]),
        float(lines[3]),
        int(lines[4]),
        int(lines[5]),
        int(lines[6]),
        float(lines[7])
    )

def load_population_diversity(filename: str) -> list:
    """
    Loads population diversity values from a text file.

    Each line is expected to contain a single float value corresponding to a generation.

    :param filename: Path to the file containing diversity values.
    :return: List of float values (one per generation).
    """
    with open(filename, "r") as file:
        values = [float(line.strip()) for line in file if line.strip()]
    return values

def load_fitness_values(filepath):
    """
    Loads fitness values of all individuals across all generations from a given file.

    Parameters:
    filepath (str): Path to the file containing fitness values.

    Returns:
    list[list[float]]: A list where each sublist contains fitness values of individuals for a generation.
    """
    fitness_data = []
    
    with open(filepath, 'r') as file:
        for line in file:
            # Strip any whitespace and split by comma
            values = line.strip().split(',')
            # Convert to floats, skipping empty strings if any
            generation = [float(value) for value in values if value]
            fitness_data.append(generation)

    return fitness_data


def load_age_layer_distribution(filepath):
    """
    Loads age-layer distribution values from a given file.

    Each line is expected to represent a generation, and each comma-separated
    value represents the percentage of individuals in that age layer.

    Parameters:
    filepath (str): Path to the file containing age-layer distribution data.

    Returns:
    list[list[float]]: A list where each sublist contains percentages per layer for a generation.
    """
    age_layer_data = []

    with open(filepath, 'r') as file:
        for line in file:
            values = line.strip().split(',')
            generation = [float(value) for value in values if value]
            age_layer_data.append(generation)

    return age_layer_data

def load_fitness_layer_distribution(filepath):
    """
    Loads fitness-layer distribution values from a given file.

    Each line is expected to represent a generation, and each comma-separated
    value represents the percentage of individuals in that fitness layer.

    Parameters:
    filepath (str): Path to the file containing fitness-layer distribution data.

    Returns:
    list[list[float]]: A list where each sublist contains percentages per layer for a generation.
    """
    fitness_layer_data = []

    with open(filepath, 'r') as file:
        for line in file:
            values = line.strip().split(',')
            generation = [float(value) for value in values if value]
            fitness_layer_data.append(generation)

    return fitness_layer_data



""" ========================== PLOTTING FUNCTIONS ========================== """
"""
1. Best fitness per generation plot
2. Average fitness per generation plot
3. Worrst fitness per generation plot
4. Fitness diversity per generation plot (low diversity = convergence, sudden drops = loss of exploration)

5. Histogram of fitness values in the final generation
6. Population diversity per generation plot
7. Age layer distribution over generations (for ALPS)
8. Fitness layer distribution over generations (for ALPS)
"""

def plot_fitness_metrics(fitness_values, algorithm_parameters):
    """
    Plots a 2x2 grid showing fitness metrics and a sidebar displaying algorithm parameters.

    :param fitness_values: List of lists with fitness values for each generation.
    :param algorithm_parameters: NamedTuple or object with algorithm parameters to display.
    """
    avg_fitness = [np.mean(gen) for gen in fitness_values]
    best_fitness = [np.max(gen) for gen in fitness_values]
    worst_fitness = [np.min(gen) for gen in fitness_values]

    fitness_diversity = []
    for gen in fitness_values:
        gen = np.array(gen)
        if gen.max() == gen.min():
            norm_gen = np.zeros_like(gen)
        else:
            norm_gen = (gen - gen.min()) / (gen.max() - gen.min())
        diversity = np.std(norm_gen)
        fitness_diversity.append(diversity)

    fig, axes = plt.subplots(2, 2, figsize=(10, 6))
    
    # Adjust main layout to leave space for sidebar
    fig.subplots_adjust(right=0.75)

    # Average Fitness
    axes[0, 0].plot(avg_fitness, label="Average Fitness", color='tab:green')
    axes[0, 0].set_title("Average Fitness per Generation")
    axes[0, 0].set_xlabel("Generation")
    axes[0, 0].set_ylabel("Fitness")
    axes[0, 0].legend()
    axes[0, 0].grid(True)

    # Best Fitness
    axes[0, 1].plot(best_fitness, label="Best Fitness", color='tab:blue')
    axes[0, 1].set_title("Best Fitness per Generation")
    axes[0, 1].set_xlabel("Generation")
    axes[0, 1].set_ylabel("Fitness")
    axes[0, 1].legend()
    axes[0, 1].grid(True)

    # Worst Fitness
    axes[1, 0].plot(worst_fitness, label="Worst Fitness", color='tab:red')
    axes[1, 0].set_title("Worst Fitness per Generation")
    axes[1, 0].set_xlabel("Generation")
    axes[1, 0].set_ylabel("Fitness")
    axes[1, 0].legend()
    axes[1, 0].grid(True)

    # Normalized Diversity
    axes[1, 1].plot(fitness_diversity, label="Normalized Fitness Diversity", color='tab:orange')
    axes[1, 1].set_title("Normalized Fitness Diversity per Generation")
    axes[1, 1].set_xlabel("Generation")
    axes[1, 1].set_ylabel("Diversity (0–1)")
    axes[1, 1].set_ylim(0, 0.5)
    axes[1, 1].legend()
    axes[1, 1].grid(True)

    # Sidebar for Algorithm Parameters
    sidebar_ax = fig.add_axes([0.78, 0.15, 0.2, 0.7])  # [left, bottom, width, height]
    sidebar_ax.axis('off')  # Hide axis
    sidebar_ax.set_title("Hyperparameters", fontsize=12, fontweight='bold')

    # Format parameters
    param_text = "\n".join([
        f"Max Iterations: {algorithm_parameters.max_iterations}",
        f"Stall Iterations: {algorithm_parameters.stall_iterations}",
        f"Cross Prob: {algorithm_parameters.cross_prob}",
        f"Mutate Prob: {algorithm_parameters.mutate_prob}",
        f"Population Size: {algorithm_parameters.pop_size}",
        f"Units: {algorithm_parameters.number_of_units}",
        #"Rank-based Selection"
        f"Tournament Size: \n(age layer size / 2)"
        #f"Tournament Rate: {algorithm_parameters.tournament_rate}"
    ])
    sidebar_ax.text(0, 1, param_text, fontsize=10, verticalalignment='top')

    plt.tight_layout(rect=[0, 0, 0.75, 1])  # Leave room for the sidebar
    plt.show()


def plot_diversity_summary(fitness_matrix, population_diversity, algorithm_parameters):
    """
    Plots a 1x2 grid:
    - Left: Histogram of fitness values in the final generation
    - Right: Line plot of population diversity over generations

    :param fitness_matrix: List of lists; fitness values for each individual per generation
    :param population_diversity: List of diversity values for each generation
    """
    final_gen = fitness_matrix[-1]

    fig, axes = plt.subplots(1, 2, figsize=(10, 3))
    # Adjust main layout to leave space for sidebar
    fig.subplots_adjust(right=0.75)

    # Plot 1: Final Generation Histogram
    axes[0].hist(final_gen, bins=20, edgecolor='black', color='skyblue')
    axes[0].set_title("Fitness Distribution in Final Generation")
    axes[0].set_xlabel("Fitness")
    axes[0].set_ylabel("Count")
    axes[0].grid(True)

    # Plot 2: Population Diversity
    axes[1].plot(population_diversity, label="Population Diversity", color='orange')
    axes[1].set_title("Population Diversity per Generation")
    axes[1].set_xlabel("Generation")
    axes[1].set_ylabel("Diversity")
    axes[1].legend()
    axes[1].grid(True)

    # Sidebar for Algorithm Parameters
    sidebar_ax = fig.add_axes([0.78, 0.15, 0.2, 0.7])  # [left, bottom, width, height]
    sidebar_ax.axis('off')  # Hide axis
    sidebar_ax.set_title("Hyperparameters", fontsize=12, fontweight='bold')

    # Format parameters
    param_text = "\n".join([
        f"Max Iterations: {algorithm_parameters.max_iterations}",
        f"Stall Iterations: {algorithm_parameters.stall_iterations}",
        f"Cross Prob: {algorithm_parameters.cross_prob}",
        f"Mutate Prob: {algorithm_parameters.mutate_prob}",
        f"Population Size: {algorithm_parameters.pop_size}",
        f"Units: {algorithm_parameters.number_of_units}",
        #"Rank-based Selection"
        f"Tournament Size: \n(age layer size / 2)"
        #f"Tournament Rate: {algorithm_parameters.tournament_rate}"
    ])
    sidebar_ax.text(0, 1, param_text, fontsize=10, verticalalignment='top')

    plt.tight_layout(rect=[0, 0, 0.75, 1])  # Leave room for the sidebar
    plt.show()


def plot_age_layer_distribution(data_str):
    """
    Plots the age layer distribution over generations.
    
    :param data_str: list of lists of age layer distribution (portions) per every generation.
    Inner sublist is the age layer distribution for a generation.
    """
    # Convert the data to a DataFrame for easier plotting
    df = pd.DataFrame(data_str)

    # Plotting
    plt.figure(figsize=(10, 6))
    for i in range(df.shape[1]):
        plt.plot(df.index, df[i], label=f"Layer {i+1}")

    plt.title("Age Layer Distribution Over Generations")
    plt.xlabel("Generation")
    plt.ylabel("Proportion of Individuals")
    plt.legend()
    plt.grid(True)
    plt.show()


def plot_fitness_layer_distribution(data_str):
    """
    Plots the fitness layer distribution over generations as dotted lines.
    
    :param data_str: list of lists of fitness layer distribution (portions) per every generation.
    Inner sublist is the fitness layer distribution for a generation.
    """
    # Convert the data to a DataFrame for easier plotting
    df = pd.DataFrame(data_str)

    # Plotting
    plt.figure(figsize=(10, 6))
    for i in range(df.shape[1]):
        plt.plot(df.index, df[i], label=f"Layer {i+1}", linestyle='dotted')

    plt.title("Fitness Layer Distribution Over Generations")
    plt.xlabel("Generation")
    plt.ylabel("Proportion of Individuals")
    plt.legend()
    plt.grid(True)
    plt.show()


if __name__ == "__main__":
    hyperparameters = load_hyperparameters(hyperparameters_filename)
    population_diversity = load_population_diversity(population_diversity_filename)
    all_fitness_values = load_fitness_values(fitness_values_filename)
    age_distribution = load_age_layer_distribution(age_distribution_filename)
    fitness_distribution = load_fitness_layer_distribution(fitness_distirbution_filename)

    # Plotting
    plot_fitness_metrics(all_fitness_values, algorithm_parameters=hyperparameters)
    plot_diversity_summary(all_fitness_values, population_diversity, algorithm_parameters=hyperparameters)
    # Plotting (for ALPS only)
    #plot_age_layer_distribution(age_distribution)
    #plot_fitness_layer_distribution(fitness_distribution)
