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

"""Module dealing with postcode information."""

import os
import numpy as np
import pandas as pd

__all__ = ['GeospatialLocator', 'great_circle_distance']


def great_circle_distance(latlon1, latlon2):
    """
    Calculate the great circle distance (in metres) between pairs of
    points specified as latitude and longitude on a spherical Earth
    (with radius 6371 km).

    Parameters
    ----------

    latlon1: arraylike
        Latitudes and longitudes of
        the first set of points (as [n, 2] array for n points).
    latlon2: arraylike
        Latitudes and longitudes of the second
        set of points (as [m, 2] array for m points).

    Returns
    -------

    numpy.ndarray
        Distance in metres between each pair of points (as an n x m array).

    Examples
    --------

    >>> import numpy as np
    >>> fmt = lambda x: np.format_float_scientific(x, precision=3)
    >>> with np.printoptions(formatter={'all': fmt}):
    ...     print(great_circle_distance([[54.0, 0.0], [55, 0.0]], [[55, 1.0]]))
    [[128611.742]
     [ 63777.774]]
    """

    EARTH_RADIUS = 6371e3

    lat1, lon1 = np.radians(latlon1).T
    lat2, lon2 = np.radians(latlon2).T

    # Compute differences between each pair of points
    dlat = lat2 - lat1[:, None]  # n x m
    dlon = lon2 - lon1[:, None]

    a = (
        np.sin(dlat / 2) ** 2
        + np.cos(lat1[:, None])
        * np.cos(lat2[None, :])
        * np.sin(dlon / 2) ** 2
    )
    c = 2 * np.arcsin(np.sqrt(a))

    distance = np.empty((len(latlon1), len(latlon2)), float)
    distance = EARTH_RADIUS * c
    return distance


# print(great_circle_distance([[54.0, 0.0], [55, 0.0]], [[55, 1.0]]))


class GeospatialLocator(object):
    """
    Class to interact with a postcode database file and a population grid file.
    """

    def __init__(self, postcode_file='',
                 census_file='',
                 norm=great_circle_distance):
        """
        Parameters
        ----------

        postcode_file : str, optional
            Filename of a .csv file containing geographic
            location data for postcodes.

        census_file :  str, optional
            Filename of a .asc file containing census data on a
            latitude-longitude grid.

        norm : function
            Python function defining the distance between points in
            latitude-longitude space.

        """
        self.ER = 6371000
        self.norm = norm

    def haversine(self, lat1, lon1, lat2, lon2):
        R = self.ER
        lat1, lon1, lat2, lon2 = map(np.radians, [lat1, lon1, lat2, lon2]) # noqa
        dlat = np.abs(lat2 - lat1)
        dlon = np.abs(lon2 - lon1)
        a = np.sin(dlat / 2.0)**2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon / 2.0)**2 # noqa
        c = 2 * np.arcsin(np.sqrt(a))
        return R * c

    def get_postcodes_by_radius(self, X, radii):
        """
        Return postcodes within specific distances of
        input location.

        Parameters
        ----------
        X : arraylike
            Latitude-longitude pair of centre location
        radii : arraylike
            array of radial distances from X

        Returns
        -------
        list of lists
            Contains the lists of postcodes closer than the elements
            of radii to the location X.


        Examples
        --------

        >>> locator = GeospatialLocator()
        >>> locator.get_postcodes_by_radius((51.4981, -0.1773), [1.5e3])
        >>> locator.get_postcodes_by_radius((51.4981, -0.1773),
                                            [1.5e3, 4.0e3])
        """
        assert (len(radii) > 0)
        assert (i > 0 for i in radii)
        assert (len(X) == 2)
        # radii = [i * 10**-3 for i in radii]
        current_dir = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(current_dir,
                                 "../resources/full_postcodes.csv")
        df = pd.read_csv(file_path)

        centralLatitude = X[0]
        centralLongitude = X[1]
        centralLatitudeRad = np.radians(centralLatitude)
        centralLongitudeRad = np.radians(centralLongitude)

        def getPostCodes(radius: float):
            temp = radius/self.ER
            minLatitudeRad = centralLatitudeRad - temp
            maxLatitudeRad = centralLatitudeRad + temp

            '''
            diff_long = np.arcsin((np.sin(temp)/np.cos(centralLatitudeRad)))
            minLongitudeRad = centralLongitudeRad - diff_long
            maxLongitudeRad = centralLongitudeRad + diff_long
            '''
            diff_long = np.abs(temp/np.cos(centralLatitudeRad))
            minLongitudeRad = centralLongitudeRad - diff_long
            maxLongitudeRad = centralLongitudeRad + diff_long

            filter_df = df[
                            (df['Latitude'] >= np.degrees(minLatitudeRad)-1) & (df['Latitude'] <= np.degrees(maxLatitudeRad)+1) & # noqa
                            (df['Longitude'] >= np.degrees(minLongitudeRad)-1) & (df['Longitude'] <= np.degrees(maxLongitudeRad)+1) # noqa
                          ].copy()

            postcodes = []
            filter_df['Distance'] = filter_df.apply(lambda row: self.haversine(centralLatitude, centralLongitude, row['Latitude'], row['Longitude']), axis=1) # noqa
            result_df = filter_df[filter_df['Distance'] <= radius].copy()
            result_df['Postcode'] = result_df['Postcode'].apply(lambda x: x[:3] + ' ' + x[3:] if len(x) < 7 else x) # noqa

            postcodes = result_df['Postcode'].tolist()

            return postcodes

        postcodesForEachRadius = [getPostCodes(radius) for radius in radii]

        return postcodesForEachRadius

    def get_population_by_radius(self, X, radii):
        """
        Return the population within specific distances of input location.

        Parameters
        ----------
        X : arraylike
            Latitude-longitude pair of centre location
        radii : arraylike
            array of radial distances from X

        Returns
        -------
        list
            Contains the population closer than the elements of radii to
            the location X. Output should be the same shape as the radii array.

        Examples
        --------
        >>> loc = GeospatialLocator()
        >>> loc.get_population_by_radius((51.4981, -0.1773), [1e2, 5e2, 1e3])

        """
        assert (len(radii) > 0)
        # assert all(i > 0 for i in radii)
        assert (len(X) == 2)
        # radii = [i * 10**-3 for i in radii]
        current_dir = os.path.dirname(os.path.abspath(__file__))
        file_path = os.path.join(current_dir, "../resources/UK_residential_population_2011.csv") # noqa
        df = {}
        if os.path.exists(file_path):
            df = pd.read_csv(file_path)
        else:
            input_file_path = os.path.join(current_dir, "../resources/UK_residential_population_2011_latlon.asc") # noqa
            self.convertToCsv(input_file_path, file_path)
            df = pd.read_csv(file_path)



        df['population'] = df['population'].apply(lambda x: 0 if x == -9999 else x) # noqa

        centralLatitude = X[0]
        centralLongitude = X[1]
        centralLatitudeRad = np.radians(centralLatitude)
        centralLongitudeRad = np.radians(centralLongitude)

        def getPop(radius: float):
            temp = radius/self.ER
            minLatitudeRad = centralLatitudeRad - temp
            maxLatitudeRad = centralLatitudeRad + temp

            '''
            diff_long = np.arcsin((np.sin(temp)/np.cos(centralLatitudeRad)))
            minLongitudeRad = centralLongitudeRad - diff_long
            maxLongitudeRad = centralLongitudeRad + diff_long
            '''
            diff_long = np.abs(temp/np.cos(centralLatitudeRad))
            minLongitudeRad = centralLongitudeRad - diff_long
            maxLongitudeRad = centralLongitudeRad + diff_long

            filter_df = df[
                            (df['latitude'] >= np.degrees(minLatitudeRad)-1) & (df['latitude'] <= np.degrees(maxLatitudeRad)+1) & # noqa
                            (df['longitude'] >= np.degrees(minLongitudeRad)-1) & (df['longitude'] <= np.degrees(maxLongitudeRad)+1) # noqa
                          ].copy()

            filter_df['Distance'] = filter_df.apply(lambda row: self.haversine(centralLatitude, centralLongitude, row['latitude'], row['longitude']), axis=1) # noqa
            result_df = filter_df[filter_df['Distance'] <= radius].copy()

            totalPop = result_df['population'].sum()
            return int(totalPop)

        res = [getPop(radius) for radius in radii]

        return res

    def convertToCsv(self, input_file, output_file):

        data = pd.read_table(input_file, skiprows=6, header=None, sep=r'\s+')

        nrows = 1211
        ncols = 652

        latitudes = []
        longitudes = []
        population = []

        for i in range(nrows):

            lat = data.iloc[i]
            lon = data.iloc[i+nrows]
            pop = data.iloc[i+2*nrows]

            for j in range(ncols):
                latitudes.append(lat[j])
                longitudes.append(lon[j])
                population.append(pop[j])

        df = pd.DataFrame({
            'longitude': longitudes,
            'latitude': latitudes,
            'population': population
        })

        df.to_csv(output_file, index=False)

        return df
