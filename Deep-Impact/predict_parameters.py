import pandas as pd
import numpy as np
from scipy.optimize import minimize
from deepimpact.solver import Planet


# Function to simulate dedz based on given parameters
def simulate_dedz(planet, radius, strength, pancake_factor, altitudes):
    result = planet.solve_atmospheric_entry(
        radius=radius,
        velocity=19200,  # initial velocity (m/s)
        density=3300,    # asteroid density (kg/m^3)
        strength=strength,
        angle=18.3,        # entry angle
        init_altitude=altitudes.max(),
        dt=0.05
    )
    result_with_dedz = planet.calculate_energy(result)
    return result_with_dedz


# Cost function for optimization
def cost_function(params, planet, altitudes, dedz_observed):
    """
    Cost function for optimization.

    Parameters
    ----------
    params : list
        List of parameters [radius, strength, pancake_factor] to optimize.

    planet : Planet
        Instance of the Planet class.

    altitudes : array
        Array of altitudes (m).

    dedz_observed : array
        Array of observed energy deposition per unit length (kt Km^-1).

    Returns
    -------
    float
        Sum of squared errors between observed and simulated dedz values.
    """
    radius, strength, pancake_factor = params
    planet.pancake_factor = pancake_factor

    # Simulate the dedz values
    result_with_dedz = simulate_dedz(
        planet, radius, strength, pancake_factor, altitudes
    )

    # Extract simulated dedz values from the result
    dedz_simulated = result_with_dedz['dedz'].values

    # Interpolate simulated values to match the observed altitudes
    dedz_interpolated = np.interp(
        altitudes, result_with_dedz['altitude'].values, dedz_simulated
    )

    # Calculate the sum of squared differences
    error = np.sum((dedz_observed - dedz_interpolated)**2)

    return error


# Main function to load data, optimize parameters, and output results
def main(input_csv):
    """
    Main function to load data, preprocess it, and optimize parameters.

    Parameters
    ----------
    input_csv : str
        Path to the input CSV file containing the observed data.

    Returns
    -------
    tuple
        Optimized parameters (radius, strength, pancake_factor).
    """
    # Load data
    data = pd.read_csv(input_csv)

    # Convert Height (km) to altitude in meters
    altitudes = data['Height (km)'].values * 1000  # Convert km to m

    # Extract Energy Per Unit Length (kt Km^-1) as is
    dedz_observed = data['Energy Per Unit Length (kt Km^-1)'].values

    # Initialize the Planet object
    planet = Planet(atmos_func='tabular')

    # Initial guesses and bounds for parameters
    initial_guess = [10, 1e6, 10]
    bounds = [(5, 15), (0.1 * 1e6, 10 * 1e6), (2, 10)]

    # Perform optimization
    result = minimize(
        cost_function,
        initial_guess,
        args=(planet, altitudes, dedz_observed),
        bounds=bounds,
        method='Nelder-Mead'
    )

    # Extract optimized parameters
    radius_opt, strength_opt, pancake_factor_opt = result.x
    print("Optimized parameters:")
    print(f"Radius: {radius_opt:.2f} m")
    print(f"Strength: {strength_opt:.2e} N/m^2")
    print(f"Pancake Factor: {pancake_factor_opt:.2f}")
    return radius_opt, strength_opt, pancake_factor_opt


if __name__ == "__main__":
    input_csv_path = "resources/ChelyabinskEnergyAltitude.csv"
    main(input_csv_path)
