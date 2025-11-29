# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
#
# Copyright (C) 2024 ACSE Hygiea Team

"""Module to calculate the damage and impact risk for given scenarios"""
import pandas as pd
import os
import math
from collections import Counter
import statistics
from deepimpact.locator import GeospatialLocator
from deepimpact.solver import Planet  # noqa: F401

__all__ = ['damage_zones', 'impact_risk']

# Formula defining the nonlinear function p(r)


def p(r, zb, Ek):
    term1 = 3 * 10**11 * ((r**2 + zb**2) / (Ek**(2/3)))**(-1.3)
    term2 = 2 * 10**7 * ((r**2 + zb**2) / (Ek**(2/3)))**(-0.57)
    return term1 + term2


def solve_r(p_target, zb, ek, tol=1e-6, max_iter=1000):
    """
    Create a nonlinear solver to calculate r satisfying the equation.

    parameters
    ----------
    p_target: float
        Four thresholds of different levels
    zb: float
        burst altitude (m)
    ek: float
        burst energy (kt)
    tol: float
        tolerance(default 1e-6)
    max_iter: int
        Max Iterations(default 1000)

    Returns
    -------
    r: float
        burst range(m)
    """
    def pressure_function(r):
        """Define overpressure as a function of r."""
        term1 = 3e11 * ((r**2 + zb**2) / (ek**(2/3)))**-1.3
        term2 = 2e7 * ((r**2 + zb**2) / (ek**(2/3)))**-0.57
        return term1 + term2 - p_target

    # Initialize the boundaries
    r_low = 1.0
    r_high = 1e6
    f_low = pressure_function(r_low)
    f_high = pressure_function(r_high)

    # Make sure the initial range contains the roots.
    if f_low * f_high > 0:
        return 0
    # raise ValueError("The initial range does not contain roots.")

    # Bisection Method
    for iteration in range(max_iter):
        r_mid = (r_low + r_high) / 2.0
        f_mid = pressure_function(r_mid)

        if abs(f_mid) < tol:
            return r_mid

        if f_low * f_mid < 0:
            r_high = r_mid
            f_high = f_mid
        else:
            r_low = r_mid
            f_low = f_mid

    # it does not converge if the loop exits
    raise RuntimeError("The solver does not converge!")


def damage_zones(outcome, lat, lon, bearing, pressures):
    """
    Calculate the latitude and longitude of the surface zero location and the
    list of airblast damage radii (m) for a given impact scenario.

    Parameters
    ----------

    outcome: Dict
        the outcome dictionary from an impact scenario
    lat: float
        latitude of the meteoroid entry point (degrees)
    lon: float
        longitude of the meteoroid entry point (degrees)
    bearing: float
        Bearing (azimuth) relative to north of meteoroid trajectory (degrees)
    pressures: float, arraylike
        List of threshold pressures to define airblast damage levels

    Returns
    -------

    blat: float
        latitude of the surface zero point (degrees)
    blon: float
        longitude of the surface zero point (degrees)
    damrad: arraylike, float
        List of distances specifying the blast radii
        for the input damage levels

    Examples
    --------

    >>> import deepimpact
    >>> outcome = {'burst_altitude': 8e3, 'burst_energy': 7e3,
                   'burst_distance': 90e3, 'burst_peak_dedz': 1e3,
                   'outcome': 'Airburst'}
    >>> deepimpact.damage_zones(outcome, 52.79, -2.95, 135,
                                pressures=[1e3, 5e3, 25e3, 40e3])
    """

    # Convert latitude, longitude, and bearing from degrees to radians
    lat = math.radians(lat)
    lon = math.radians(lon)
    bearing = math.radians(bearing)

    # Calculate the angular distance (distance / Earth's radius)
    earth_radius = 6371000
    angular_distance = outcome['burst_distance'] / earth_radius

    # Calculate the destination latitude (phi_2)
    blat = math.asin(
        math.sin(lat) * math.cos(angular_distance) +
        math.cos(lat) * math.sin(angular_distance) * math.cos(bearing)
    )

    # Calculate the destination longitude (lambda_2)
    blon = lon + math.atan2(
        math.sin(bearing) * math.sin(angular_distance) * math.cos(lat),
        math.cos(angular_distance) - math.sin(lat) * math.sin(blat)
    )

    # Convert latitude and longitude from radians to degrees
    blat = math.degrees(blat)
    blon = math.degrees(blon)

    # Normalize longitude to the range [-180, 180]
    blon = (blon + 180) % 360 - 180
    damrad = []
    for i in range(0, len(pressures)):
        pr = pressures[i]
        zb = outcome['burst_altitude']
        ek = outcome['burst_energy']
        r_solution = solve_r(pr, zb, ek)
        damrad.append(r_solution)

    return blat, blon, damrad


def impact_risk(planet,
                impact_file=os.sep.join((os.path.dirname(__file__),
                                         '..', 'resources',
                                         'impact_parameter_list.csv')),
                pressure=30.e3, nsamples=None):
    """
    Perform an uncertainty analysis to calculate the probability for
    each affected UK postcode and the total population affected.

    Parameters
    ----------
    planet: deepimpact.Planet instance
        The Planet instance from which to solve the atmospheric entry

    impact_file: str
        Filename of a .csv file containing the impact parameter list
        with columns for 'radius', 'angle', 'velocity', 'strength',
        'density', 'entry latitude', 'entry longitude', 'bearing'

    pressure: float
        A single pressure at which to calculate the damage zone for each impact

    nsamples: int or None
        The number of iterations to perform in the uncertainty analysis.
        If None, the full set of impact parameters provided in impact_file
        is used.

    Returns
    -------
    probability: DataFrame
        A pandas DataFrame with columns for postcode and the
        probability the postcode was inside the blast radius.
    population: dict
        A dictionary containing the mean and standard deviation of the
        population affected by the impact, with keys 'mean' and 'stdev'.
        Values are floats.
    """

    df = pd.read_csv(impact_file)
    if nsamples is None:
        nsamples = df.shape[0]
    else:
        df = df.iloc[:nsamples]

    # results will be df with columns 'blat', 'blon', 'brad'
    def solve_entry(row):
        return planet.solve_atmospheric_entry(
            row['radius'], row['velocity'], row['density'], row['strength'], row['angle'] # noqa
        )

    solver_outcome = df.apply(solve_entry, axis=1).tolist()

    solver_list = [planet.analyse_outcome(planet.calculate_energy(x)) for x in solver_outcome] # noqa

    dz_list = []
    for i in range(nsamples):
        dz_list.append(damage_zones(solver_list[i],
                                    df['entry latitude'][i],
                                    df['entry longitude'][i],
                                    df['bearing'][i],
                                    [pressure]))

    centers = [[item[0], item[1]] for item in dz_list]
    radii = [item[2] for item in dz_list]

    locator = GeospatialLocator()
    postToProb = pd.DataFrame(columns=['postcode', 'probability'])

    allPopVals = []

    for i in range(nsamples):
        postcodes = locator.get_postcodes_by_radius(centers[i], radii[i])

        flat_postcodes = [i for sublist in postcodes for i in sublist]
        frequency = Counter(flat_postcodes)
        newCounts = pd.DataFrame(frequency.items(), columns=['postcode', 'probability']) # noqa

        postToProb = ( pd.concat([postToProb, newCounts]).groupby('postcode', as_index=False).sum()) # noqa
        population = locator.get_population_by_radius(centers[i], radii[i])
        allPopVals.append(max(population))

    mean = statistics.mean(allPopVals)
    stdev = statistics.stdev(allPopVals)
    postToProb['probability'] = postToProb['probability'].apply(lambda x: x/(nsamples)) # noqa

    return (pd.DataFrame(postToProb),
            {'mean': mean, 'stdev': stdev})
