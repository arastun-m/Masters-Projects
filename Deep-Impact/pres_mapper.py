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

# flake8: noqa

"""This module contains some useful mapping functions"""

import folium
from deepimpact.locator import GeospatialLocator
from deepimpact.solver import Planet  # noqa: F401
import pres_mapper
from deepimpact.damage import damage_zones
import pandas as pd
import os

__all__ = ["plot_circle"]


def plot_circle(blat, blon, radii,
                entrylat=None, entrylon=None, fmap=None, lgd=False,
                fliprad=False, **kwargs):
    """
    Plot circles representing blast radii on a map, with an optional existing
    folium map instance.

    Parameters
    ----------

    blat : float
        Latitude of the blast location.
    blon : float
        Longitude of the blast location.
    radii : arraylike
        array of radial distances from X
    entrylat : float, optional
        Latitude of the entry location.
    entrylon : float, optional
        Longitude of the entry location
    fmap : folium.Map, optional
        Existing folium map object to add the circles to. If None, a new map
        will be created.
    **kwargs : dict
        Additional keyword arguments to pass to folium elements.

    Returns
    -------

    folium.Map
        Folium map object with the plotted circles and additional elements.

    Examples
    --------

    >>> blat = 52.85
    >>> blon = -2.96
    >>> radii = [500, 1300, 2000, 3000]
    >>> fmap = plot_circle(gzero_coords, blast_coords, radii, postcodes,
    populations)
    """

    # switch radii order
    if fliprad:
        radii = radii[::-1]

    # create locator object
    locator = GeospatialLocator()

    # blast coordinates
    blast_coords = [blat, blon]

    # get populations by radius
    populations = locator.get_population_by_radius(blast_coords, radii)

    # check if fmap is None
    if fmap is None:
        # create new folium map
        fmap = folium.Map(location=blast_coords, control_scale=True)
    # else use existing fmap but check if it is a folium map object
    else:
        if not isinstance(fmap, folium.Map):
            raise TypeError("fmap must be a folium.Map object")

    # number of colors
    num_colors = len(radii)

    # create a list of colors based on the number of radii based on rgb values
    circle_rgb = [
        [
            int(255 - (i * 255 / num_colors)),
            int(100 - (i * 100 // num_colors)),
            int(50 - (i * 50 // num_colors)),
        ]
        for i in range(len(radii))
    ]

    def rgb_to_color(rgb):
        return f"rgb({rgb[0]}, {rgb[1]}, {rgb[2]})"

    # create list of circle colors
    circle_colors = [rgb_to_color(rgb) for rgb in circle_rgb]

    # create class for circle
    class BlastCircle:
        def __init__(self, radius, population, color):
            self.radius = radius
            # self.postcode_list = postcode_list
            self.population = population
            self.color = color

    # create list of circle objects
    blast_circles = [
        BlastCircle(radii[i], populations[i], circle_colors[i])
        for i in range(len(radii))
    ]

    # blast coordinates
    lat = blast_coords[0]
    lon = blast_coords[1]

    # function to plot a circle
    def pcircle(circle):
        # this function plots a circle on the map
        tooltip = (
            f"Blast Radius: {circle.radius:.2f}m<br>"
            f"Center: {lat:.2f},{lon:.2f}<br>"
            f"Population: {circle.population}"
        )
        folium.Circle(
            location=blast_coords,
            color=circle.color,
            radius=circle.radius,
            fill=True,
            fillOpacity=0.3,
            tooltip=tooltip,
            **kwargs,
        ).add_to(fmap)

    # plot circles
    for circle in blast_circles:
        pcircle(circle)

    if entrylat is not None and entrylon is not None:
        print(entrylat, entrylon)
        # ground zero
        # NOTE: The following code is adapted from the folium documentation
        folium.Marker(
            location=[entrylat, entrylon],
            icon=folium.Icon(color="blue", prefex="fa"),
            tooltip=f"Entry Point<br>Coordinates:\
                  {entrylat:.2f}, {entrylon:.2f}",
            **kwargs,
        ).add_to(fmap)

        # line connecting ground zero to impact point
        folium.PolyLine(
            locations=[[entrylat, entrylon], [lat, lon]],
            color="blue",
            weight=2,
            opacity=0.5,
            dash_array="5, 5",
            tooltip="Trajectory",
            **kwargs,
        ).add_to(fmap)

    dzdict = {1: '1kPa ~10percent glass windows shatter',
               2: '5kPa ~90percent glass windows shatter',
                3: '25kPa Trees and Wood Buildings flattened',
                    4: '40kPa Multistory Stone Buildings Collapse'}

    if lgd:
        # legend
        legend_html = f"""
        <div style="position: fixed;
                    bottom: 50px; left: 50px; width: 600x; height: 150px;
                    border:2px solid grey; z-index:9999; font-size:14px;
                    background-color: white;
                    opacity: 0.85;
                    ">
        &nbsp; Surface Zero: {lat:.3f}, {lon:.3f} <br>
        &nbsp; Total Population Affected: {populations[0]:.2f} <br>
        """
        for j, circle in enumerate(blast_circles):
            legend_html += (
                f"&nbsp; <i class='fa fa-circle fa-1x' style='color:"
                f"rgb({circle_rgb[j][0]}, "
                f"{circle_rgb[j][1]},{circle_rgb[j][2]});'></i> {dzdict[j+1]}; Population: "
                f"{blast_circles[j].population}; Radius: "
                f"{blast_circles[j].radius:.0f}m <br>"
            )
        legend_html += "</div>"
        fmap.get_root().html.add_child(folium.Element(legend_html))

    # click for coordinates
    fmap.add_child(folium.LatLngPopup())

    return fmap

circle_rgb = [
    [
        int(255 - (i * 255 / 2)),
        int(100 - (i * 100 // 2)),
        int(50 - (i * 50 // 2)),
    ]
    for i in range(2)
]

def rgb_to_color(rgb):
    return f"rgb({rgb[0]}, {rgb[1]}, {rgb[2]})"

damage_zone_dict = {3: '25kPa Trees and Wood Buildings flattened', 4: '40kPa Multistory Stone Buildings Collapse'}

def add_legend(blat, blon, fmap):
    # legend
    legend_html = f"""
    <div style="position: fixed;
                bottom: 50px; left: 50px; width: 450px; height: 75px;
                border:2px solid grey; z-index:9999; font-size:14px;
                background-color: white;
                opacity: 0.85;
                ">
    &nbsp; Surface Zero: {blat:.3f}, {blon:.3f} <br>
    """
    for j in range(2):
        legend_html += (
            f"&nbsp; <i class='fa fa-circle fa-1x' style='color:"
            f"rgb({circle_rgb[j][0]}, "
            f"{circle_rgb[j][1]}, {circle_rgb[j][2]});'></i> Damage Zone {3+j}: {damage_zone_dict[3+j]}<br>"
        )
    legend_html += "</div>"
    fmap.get_root().html.add_child(folium.Element(legend_html))
    return fmap

def create_map(dzbool=False):
    planet = Planet()

    csv_file = os.path.join(os.path.dirname(__file__), 'impact_parameters', 'impact_parameter_list_10.csv')

    df = pd.read_csv(csv_file)
    nsamples = df.shape[0]

    # results will be df with columns 'blat', 'blon', 'brad'
    def solve_entry(row):
        return planet.solve_atmospheric_entry(
            row['radius'], row['velocity'], row['density'], row['strength'], row['angle'] # noqa
        )

    solver_outcome = df.apply(solve_entry, axis=1).tolist()

    solver_list = [planet.analyse_outcome(planet.calculate_energy(x)) for x in solver_outcome] # noqa

    dz_list = []

    press=[30e3]

    if dzbool:
        press=[25e3, 40e3]

    for i in range(nsamples):
        dz_list.append(damage_zones(solver_list[i],
                                    df['entry latitude'][i],
                                    df['entry longitude'][i],
                                    df['bearing'][i],
                                    pressures=press))

    ex_map = None

    if dzbool:
        for i in dz_list:
            ex_map = pres_mapper.add_legend(i[0], i[1], pres_mapper.plot_circle(i[0], i[1], i[2], entrylat=52.03, entrylon=0.21, lgd=False, fmap=ex_map))
        return ex_map
    else:
        for i in dz_list:
            ex_map = pres_mapper.plot_circle(i[0], i[1], i[2], entrylat=52.03, entrylon=0.21, lgd=False, fmap=ex_map)
        return ex_map      

fmap = plot_circle(52.85, -2.96, [500, 1300, 2000, 3000], entrylat=55.2, entrylon=-2.5, fliprad=True, lgd=True)
# fmap.save("../deepimpact_maps/test.html")

# exmap = create_map(dzbool=True)
# exmap.save("../deepimpact_maps/test2.html")

# exmap2 = create_map()
# exmap2.save("../deepimpact_maps/test3.html")

