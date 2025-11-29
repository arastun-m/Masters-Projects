Quick Start Guide
=======================

This guide demonstrates how to quickly use the core functionality of the `deepimpact` library to solve airburst problems, calculate damage zones, and assess the potential impact on postcodes and populations. Follow the example below to get started.

Airburst Solver
-------------------

To solve the atmospheric entry problem and determine the outcomes of an airburst:

.. code-block:: python

   import deepimpact

   # Initialise the Planet class
   earth = deepimpact.Planet()

   # Solve the atmospheric entry problem for a given set of input parameters
   result = earth.solve_atmospheric_entry(radius=35, angle=45,
                                          strength=1e7, density=3000,
                                          velocity=19e3)

   # Calculate the kinetic energy lost per unit altitude
   result = earth.calculate_energy(result)

   # Determine the outcomes of the impact event
   outcome = earth.analyse_outcome(result)

Explanation:
   - **`solve_atmospheric_entry`**: Solves the atmospheric entry problem for a given asteroid size, angle, strength, density, and velocity.
   - **`calculate_energy`**: Adds a column to the result DataFrame to calculate the energy lost per unit altitude.
   - **`analyse_outcome`**: Analyzes the results and determines whether the outcome is an airburst or cratering event, along with key event statistics.

Damage Mapper
-----------------

To calculate the damage zones caused by an airburst and map the affected regions:

.. code-block:: python

   # Define pressure levels for damage zones
   pressures = [1e3, 4e3, 30e3, 50e3]

   # Calculate the blast location and damage radii
   blast_lat, blast_lon, damage_rad = deepimpact.damage_zones(outcome,
                                                              lat=55.2, lon=-2.5,
                                                              bearing=217.,
                                                              pressures=pressures)

   # Plot a circle to show the limit of the lowest damage level
   damage_map = deepimpact.plot_circle(blast_lat, blast_lon, damage_rad[0])
   damage_map.save("damage_map.html")

Explanation:
    - **`damage_zones`**: Computes the blast location and radii of damage zones for specified pressure levels.
    - **`plot_circle`**: Generates a map highlighting the damage zone for visualization. The map is saved as `damage_map.html`.

Geospatial Analysis
----------------------

To analyze the population and postcodes affected by the damage zones:

.. code-block:: python

   # The GeospatialLocator tool
   locator = deepimpact.GeospatialLocator()

   # Find the postcodes within the damage radii
   postcodes = locator.get_postcodes_by_radius((blast_lat, blast_lon),
                                               radii=damage_rad)

   # Find the population in each damage zone
   population = locator.get_population_by_radius((blast_lat, blast_lon),
                                                 radii=damage_rad)

   # Print the number of people affected in each damage zone
   print()
   print("Pressure |      Damage | Population")
   print("   (kPa) | radius (km) |   affected")
   print("-----------------------------------")
   for pop, rad, zone in zip(population, damage_rad, pressures):
       print(f"{zone/1e3:8.0f} | {rad/1e3:11.1f} | {pop:10,.0f}")
   print()

   # Print the postcodes inside the highest damage zone
   print("Postcodes in the highest damage zone:")
   print(*postcodes[-1])
   print()

Explanation:
    - **`GeospatialLocator`**: Provides tools to query postcodes and populations affected by specific radii.
    - **`get_postcodes_by_radius`**: Identifies postcodes within each damage radius.
    - **`get_population_by_radius`**: Calculates the population affected within each damage zone.

Example Output:

.. code-block:: text

   Postcodes in the highest damage zone:
   AB12 3CD AB12 3CE AB12 3CF


.. csv-table:: 
   :header: "Pressure (kPa)", "Damage radius (km)", "Population affected"
   :align: center

   1, 5.2, 3500
   4, 4.0, 2300
   30, 2.5, 800
   50, 1.8, 300

Impact Risk Assessment
-------------------------

To assess the risk of asteroid impacts:

.. code-block:: python

   # Example usage of impact_risk function
   probability, population = deepimpact.impact_risk(earth, pressure=30e3)

   # Print the probability table and population affected
   print(probability.head())
   print("Total population affected: " +
         f"{population['mean']:,.0f} +/- {population['stdev']:,.0f}")

Explanation:
    - **`impact_risk`**: Estimates the probability of asteroid impacts and the affected population based on a default dataset (`impact_parameter_list.csv`).

Example Output:

.. code-block:: text

   Total population affected: 12,345 +/- 678

Next Steps
------------

Explore more detailed API documentation in the **API Reference** section.

