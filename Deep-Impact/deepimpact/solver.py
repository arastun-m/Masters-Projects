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

"""
This module contains the atmospheric entry solver class
for the Deep Impact project
"""

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.interpolate import PchipInterpolator

__all__ = ["Planet"]


class Planet():
    """
    The class called Planet is initialised with constants appropriate
    for the given target planet, including the atmospheric density profile
    and other constants
    """

    def __init__(self, atmos_func='exponential',
                 atmos_filename=os.sep.join((os.path.dirname(__file__), '..',
                                             'resources',
                                             'AltitudeDensityTable.csv')),
                 Cd=1., Ch=0.1, Q=1e7, Cl=1e-3, alpha=0.3, Rp=6371e3,
                 g=9.81, H=8000., rho0=1.2, pancake_factor=7.0):
        """
        Set up the initial parameters and constants for the target planet

        Parameters
        ----------
        atmos_func : string, optional
            Function which computes atmospheric density, rho, at altitude, z.
            Default is the exponential function rho = rho0 exp(-z/H).
            Options are 'exponential', 'tabular' and 'constant'

        atmos_filename : string, optional
            Name of the filename to use with the tabular atmos_func option

        Cd : float, optional
            The drag coefficient

        Ch : float, optional
            The heat transfer coefficient

        Q : float, optional
            The heat of ablation (J/kg)

        Cl : float, optional
            Lift coefficient

        alpha : float, optional
            Dispersion coefficient

        Rp : float, optional
            Planet radius (m)

        rho0 : float, optional
            Air density at zero altitude (kg/m^3)

        g : float, optional
            Surface gravity (m/s^2)

        H : float, optional
            Atmospheric scale height (m)

        pancake_factor : float, optional
            The factor of the initial radius at which
            fragmentation will stop. Default is 7.
        """

        # Input constants
        self.Cd = Cd
        self.Ch = Ch
        self.Q = Q
        self.Cl = Cl
        self.alpha = alpha
        self.Rp = Rp
        self.g = g
        self.H = H
        self.rho0 = rho0
        self.pancake_factor = pancake_factor
        self.atmos_filename = atmos_filename

        try:
            # set function to define atmoshperic density
            if atmos_func == 'exponential':
                self.rhoa = self.atmos_expo_func
            elif atmos_func == 'tabular':
                self.rhoa = self.atmos_tabular_func
            elif atmos_func == 'constant':
                # value checks
                if (self.rho0 < 0):
                    raise ValueError(
                        "rho0 must be greater than or equal to zero")
                self.rhoa = lambda x: rho0
            else:
                raise NotImplementedError(
                    "atmos_func must be 'exponential', 'tabular' or 'constant'"
                    )
        except NotImplementedError:
            print("atmos_func {} not implemented yet.".format(atmos_func))
            print("Falling back to constant density atmosphere for now")
            self.rhoa = lambda x: rho0

    def solve_atmospheric_entry(
            self, radius, velocity, density, strength, angle,
            init_altitude=100e3, dt=0.05, radians=False):
        """
        Solve the system of differential equations for a given impact scenario

        Parameters
        ----------
        radius : float
            The radius of the asteroid in meters

        velocity : float
            The entery speed of the asteroid in meters/second

        density : float
            The density of the asteroid in kg/m^3

        strength : float
            The strength of the asteroid (i.e. the maximum pressure it can
            take before fragmenting) in N/m^2

        angle : float
            The initial trajectory angle of the asteroid to the horizontal
            By default, input is in degrees. If 'radians' is set to True, the
            input should be in radians

        init_altitude : float, optional
            Initial altitude in m

        dt : float, optional
            The output timestep, in s

        radians : logical, optional
            Whether angles should be given in degrees or radians. Default=False
            Angles returned in the dataframe will have the same units as the
            input

        Returns
        -------
        Result : DataFrame
            A pandas dataframe containing the solution to the system.
            Includes the following columns:
            'velocity', 'mass', 'angle', 'altitude',
            'distance', 'radius', 'spreading_rate', 'time'
        """
        if not radians:
            angle = np.deg2rad(angle)
        if (density < 0):
            raise ValueError(
                    "density must be greater than or equal to zero")
        if (init_altitude < 0):
            raise ValueError(
                    "inital altitude must be greater than or equal to zero")
        if (density < 0):
            raise ValueError(
                    "radius must be greater than or equal to zero")

        mass = (4/3)*np.pi*radius**3*density

        if (dt <= 0):
            return pd.DataFrame({'velocity': velocity,
                                 'mass': mass,
                                 'angle': angle,
                                 'altitude': init_altitude,
                                 'distance': 0.0,
                                 'radius': radius,
                                 'spreading_rate': 0.0,
                                 'time': 0.0})

        # We have to initialize spreading_rate = dr/dt for our solver to work
        # We set this to 0 as we assume that at entry the meteor
        # is not changing its radius
        spreading_rate = 0
        y0 = np.array([
            0, init_altitude, velocity, mass, angle, radius, spreading_rate
        ])
        timestep = min(0.05, dt)
        y_all, t_all = self.RK4(
            self.meteor_eq, y0, t0=0, t_max=50, dt=timestep,
            strength=strength, density=density
        )

        # Check if the meteor has cratered and change the values
        #  for the last altitude to be 0.
        # It does a linear interpolation to find the last point.
        if y_all[-1, 1] < 0:
            z_before_last = y_all[-2, 1]
            z_last = y_all[-1, 1]
            ratio = z_before_last / (z_before_last - z_last)
            y_all[-1, :] = (
                y_all[-2, :] + ratio * (y_all[-1, :] - y_all[-2, :])
            )

        extracted_y_all = y_all[::int(dt/timestep)]
        extracted_t_all = t_all[::int(dt/timestep)]
        distance = extracted_y_all[:, 0]
        altitude = extracted_y_all[:, 1]
        velocity = extracted_y_all[:, 2]
        mass = extracted_y_all[:, 3]
        if not radians:
            angle = np.rad2deg(extracted_y_all[:, 4])
        else:
            angle = extracted_y_all[:, 4]
        radius = extracted_y_all[:, 5]
        spreading_rate = extracted_y_all[:, 6]

        time = extracted_t_all
        # Enter your code here to solve the differential equations

        return pd.DataFrame({'velocity': velocity.T,
                             'mass': mass.T,
                             'angle': angle.T,
                             'altitude': altitude.T,
                             'distance': distance.T,
                             'radius': radius.T,
                             'spreading_rate': spreading_rate.T,
                             'time': time.T})

    def calculate_energy(self, result):
        """
        Function to calculate the kinetic energy lost per unit altitude in
        kilotons TNT per km, for a given solution.

        Parameters
        ----------
        result : DataFrame
            A pandas dataframe with columns for the velocity, mass, angle,
            altitude, horizontal distance and radius as a function of time

        Returns : DataFrame
            Returns the dataframe with additional column ``dedz`` which is the
            kinetic energy lost per unit altitude

        """

        if result.empty:
            result['dedz'] = []
            return result

        table = result.copy()

        initial_kinetic_energy = self.calculate_kinetic_energy(
            table['mass'].iloc[0], table['velocity'].iloc[0])

        # Compute kinetic energy
        table['kinetic_energy'] = self.calculate_kinetic_energy(
            table['mass'], table['velocity'])

        # Compute kinetic energy loss
        table['ke_loss'] = initial_kinetic_energy - table['kinetic_energy']

        de = table['ke_loss'].diff()
        dz = table['altitude'].diff() / 1000  # Convert to km

        # Calculate the rate of energy loss per unit altitude
        # Negative sign since altitude decreases with time
        table['dedz'] = np.where(
            np.isclose(dz, 0, rtol=1e-5),  # Avoid division by zero
            np.nan,
            - de / dz
        )

        # Handle any NaN or infinite values
        table['dedz'].fillna(0, inplace=True)
        table.replace([np.inf, -np.inf], 0, inplace=True)
        result['dedz'] = table['dedz']

        return result

    def calculate_kinetic_energy(self, mass, velocity):
        """
        Function to calculate the kinetic energy of an object

        Parameters
        ----------
        mass : float
            The mass of the object in kg

        velocity : float
            The velocity of the object in m/s

        Returns
        -------
        kinetic_energy : float
            The kinetic energy of the object in TNT
        """
        J_PER_KILOTON = 4.184e12  # (1 kiloton TNT = 4.184e12 joules)

        return (0.5 * mass * velocity**2) / J_PER_KILOTON

    def analyse_outcome(self, result):
        """
        Inspect a pre-found solution to calculate the impact and airburst stats

        Parameters
        ----------
        result : DataFrame
            pandas dataframe with velocity, mass, angle, altitude, horizontal
            distance, radius and dedz as a function of time

        Returns
        -------
        outcome : Dict
            dictionary with details of the impact event, which should contain
            the key:
                ``outcome`` (which should contain one of the
                following strings: ``Airburst`` or ``Cratering``),
            as well as the following 4 keys:
                ``burst_peak_dedz``, ``burst_altitude``,
                ``burst_distance``, ``burst_energy``
        """
        outcome = {'outcome': 'Unknown',
                   'burst_peak_dedz': 0.,
                   'burst_altitude': 0.,
                   'burst_distance': 0.,
                   'burst_energy': 0.}

        if result.empty:
            return outcome

        # Identify the peak energy deposition per unit height (dedz)
        peak_dedz_idx = result['dedz'].idxmax()
        burst_altitude = result.loc[peak_dedz_idx, 'altitude']
        outcome['burst_peak_dedz'] = result.loc[peak_dedz_idx, 'dedz']
        outcome['burst_altitude'] = max(burst_altitude, 0)
        outcome['burst_distance'] = result.loc[peak_dedz_idx, 'distance']

        initial_energy = self.calculate_kinetic_energy(
            result['mass'].iloc[0],
            result['velocity'].iloc[0]
        )
        peak_dedz_energy = self.calculate_kinetic_energy(
            result['mass'].iloc[peak_dedz_idx],
            result['velocity'].iloc[peak_dedz_idx]
        )

        # Check if the event is an airburst or cratering event
        if burst_altitude > 0:
            # Airburst event
            outcome['burst_energy'] = initial_energy - peak_dedz_energy
            outcome['outcome'] = 'Airburst'

        else:
            # Cratering event
            ground_energy = self.calculate_kinetic_energy(
                result['mass'].iloc[-1], result['velocity'].iloc[-1])
            total_energy_lost = initial_energy - ground_energy
            outcome['burst_energy'] = max(total_energy_lost, ground_energy)
            outcome['outcome'] = 'Cratering'

        return outcome

    def atmos_expo_func(self, z):
        """
        Function to calculate the atmospheric density at a given altitude, z,
        using an exponential model

        Parameters
        ----------
        z : float
            Altitude in meters

        Returns
        -------
        rho : float
            Atmospheric density at altitude z in kg/m^3
        """
        return self.rho0 * np.exp(-z / self.H)

    def atmos_tabular_func(self, z):
        """
        Function to calculate the atmospheric density at a given altitude, z,
        using a tabulated model

        -----------------------------------------------------------------------
        PROBLEM WITH THIS METHOD:
        Extrapolation: negative altitude value extrapolation not
        in exponential structure comparing with exponential function
        reveals the problem

        -----------------------------------------------------------------------

        Parameters
        ----------
        z : float
            Altitude in meters

        Returns
        -------
        rho : float
            Atmospheric density at altitude z in kg/m^3
        """

        # NOTE: not handling negative values of z
        # applying interpolation based on tabular data
        # for edge cases (outside tabular data) use exponential
        # function values instead of extrapolating
        if not hasattr(self, '_atmos_tabular_interpolation'):
            x_vals, y_vals = self.load_atmos_tabular_func()
            x_min, x_max = min(x_vals), max(x_vals)
            x_negative_edge = [x_min - 10000, x_min - 5000, x_min - 1000]
            x_positive_edge = [x_max + 1000, x_max + 5000, x_max + 10000]
            y_negative_edge = [self.atmos_expo_func(x) for x
                               in x_negative_edge]
            y_positive_edge = [self.atmos_expo_func(x) for x
                               in x_positive_edge]

            x_extended = np.concatenate((x_negative_edge, x_vals,
                                         x_positive_edge))
            y_extended = np.concatenate((y_negative_edge, y_vals,
                                         y_positive_edge))
            self._atmos_tabular_interpolation = \
                PchipInterpolator(x_extended, y_extended, extrapolate=True)

        return self._atmos_tabular_interpolation(z)

    def load_atmos_tabular_func(self):
        """
        Function to load the tabulated atmospheric density data

        Returns
        -------
        x_vals : array-like
            Altitude values
        y_vals : array-like
            Atmospheric density values
        """
        df = pd.read_csv(
            self.atmos_filename,
            comment="#",
            delim_whitespace=True,
            names=["Altitude (m)", "Atmospheric Density (kg/m3)"],
        )
        x_vals = df.iloc[:, 0].values
        y_vals = df.iloc[:, 1].values
        return x_vals, y_vals

    def plot_atmos_tabular_func(self):
        """
        Function to plot the tabulated atmospheric density data

        Parameters
        ----------
        x_vals : array-like
            Altitude values

        y_vals : array-like
            Atmospheric density values
        """

        df = pd.read_csv(
            self.atmos_filename,
            comment="#",
            sep=r"\s+",
            names=["Altitude (m)", "Atmospheric Density (kg/m3)"],
        )
        x_vals = df.iloc[:, 0].values
        y_vals = df.iloc[:, 1].values

        # extend the range of x estimate values
        x_est = np.linspace(min(x_vals) - 10000, max(x_vals) + 50000, 1000)
        y_est = [self.atmos_tabular_func(est) for est in x_est]
        y_est_exponential = [self.atmos_expo_func(est) for est in x_est]
        fig, ax = plt.subplots()
        ax.scatter(x_vals, y_vals, label="Tabulated data")
        ax.plot(x_est, y_est, color="red",
                label="Interpolated tabular function")
        ax.plot(x_est, y_est_exponential, color="green",
                label="Exponential function")
        ax.set_xlabel("Altitude (m)")
        ax.set_ylabel("Atmospheric Density (kg/m^3)")
        ax.set_title("Tabulated Atmospheric Density Data")
        ax.legend()
        plt.show()

    def forward_euler(self, f, y0, t0, t_max, dt, strength, density):
        """
        Solve the system of differential equations using the RK4 method

        Parameters
        ----------
        f : function
            The RHS of the system of differential equations
        y0 : array
            Array of the initial state variables
        t0 : float
            Initial time
        t_max : float
            Final time
        dt : float
            Time step

        Returns
        -------
        y_all : array
            Array of the state variables at each time step
        """
        y = np.array(y0)
        t = np.array(t0)
        y_all = [y0]
        t_all = [t0]
        while t < t_max:
            break_condition = False
            # Check if the meteor has fragmented
            rhoa_val = self.rhoa(y[1])
            if (rhoa_val * y[2]**2 > strength)\
                    and (y[5]/y0[5] < self.pancake_factor):
                # print("Break at t=", t)
                break_condition = True
            else:
                y[6] = 0.0
            y = y + dt*f(t, y, break_condition, density)
            # Ateroid has hit the ground
            if y[1] < 0:
                # print("Asteroid has hit the ground")
                break
            # Asteroid has desintegrated
            if y[3] < 0:
                # print("Asteroid has desintegrated")
                # print("mass: ", y[3])
                break
            y_all.append(y)
            t = t + dt
            t_all.append(t)
        return np.array(y_all), np.array(t_all)

    def RK4(self, f, y0, t0, t_max, dt, strength, density):
        """
        Solve the system of differential equations using the RK4 method

        Parameters
        ----------
        f : function
            The RHS of the system of differential equations
        y0 : array
            Array of the initial state variables
        t0 : float
            Initial time
        t_max : float
            Final time
        dt : float
            Time step

        Returns
        -------
        y_all : array
            Array of the state variables at each time step
        """
        y = np.array(y0)
        t = np.array(t0)
        y_all = [y0]
        t_all = [t0]
        while t < t_max:
            break_condition = False
            # Check if the meteor has fragmented
            rhoa_val = self.rhoa(y[1])
            if (rhoa_val * y[2]**2 > strength)\
                    and (y[5]/y0[5] < self.pancake_factor):
                # print("Break at t=", t)
                break_condition = True
            else:
                y[6] = 0.0
            k1 = dt*f(t, y, break_condition, density)
            k2 = dt*f(t + 0.5*dt, y + 0.5*k1, break_condition, density)
            k3 = dt*f(t + 0.5*dt, y + 0.5*k2, break_condition, density)
            k4 = dt*f(t + dt, y + k3, break_condition, density)
            y = y + (1./6.)*(k1 + 2*k2 + 2*k3 + k4)
            # Ateroid has hit the ground
            if y[1] < 0:
                # print("Asteroid has hit the ground")
                y_all.append(y)
                t = t + dt
                t_all.append(t)
                break
            # Asteroid has desintegrated
            if y[3] < 1e-8:
                # print("Asteroid has desintegrated")
                # print("mass: ", y[3])
                break
            if y[2] < 0:
                # print("negative velocity")
                break
            y_all.append(y)
            t = t + dt
            t_all.append(t)
        return np.array(y_all), np.array(t_all)

    def simplified_meteor_eq(self, t, y, break_condition=False, density=3000):
        """
        The RHS of system of differential equations for the simplified
        atmospheric entry problem (Only drag force is considered)

        Parameters
        ----------
        t : float
            Time
        y : array
            Array of the state variables
            y[0] = x (horizontal distance)
            y[1] = z (altitude)
            y[2] = v (velocity)
        theta : float
            The angle of the trajectory
        m : float
            The mass of the meteor
        A : float
            The cross-section area of the meteor
        Returns
        -------
        f : array
            Array of the RHS of the system of differential equations
        """
        theta = y[4]
        rhoa_val = self.rhoa(y[1])
        A = np.pi * y[5]**2
        f = np.zeros_like(y)
        m = y[3]
        f[0] = y[2]*np.cos(theta)
        f[1] = -y[2]*np.sin(theta)
        f[2] = -(self.Cd * rhoa_val * A * y[2]**2) / (2*m)
        return f

    def meteor_eq(self, t, y, break_condition=False, density=3000):
        """
        The RHS of system of differential equations for the atmospheric entry
        problem for the RK4 implementation.

        Parameters
        ----------
        t : float
            Time
        y : array
            Array of the state variables
            y[0] = x (horizontal distance)
            y[1] = z (altitude)
            y[2] = v (velocity)
            y[3] = m (mass)
            y[4] = theta (angle)
            y[5] = r (radius)
            y[6] = dr/dt (spreading rate)

        Returns
        -------
        f : array
            Array of the RHS of the system of differential equations
        """
        f = np.zeros_like(y)
        rhoa_val = self.rhoa(y[1])
        A = np.pi * y[5]**2

        # Calculate the RHS of dx/dt = v cos(theta) / (1 + z/Rp)
        f[0] = y[2]*np.cos(y[4])/(1+y[1]/self.Rp)

        # Calculate the RHS of dz/dt = -v sin(theta)
        f[1] = -y[2]*np.sin(y[4])

        # Calculate the RHS of dv/dt = -Cd rho A v^2 / (2m) + g sin(theta)
        f[2] = -(self.Cd * rhoa_val * A * y[2]**2) / (2*y[3]) + \
            self.g*np.sin(y[4])

        # Calculate the RHS of dm/dt = -Ch rhoa A v^3 / 2Q
        f[3] = -(self.Ch * rhoa_val * A * y[2]**3) / (2*self.Q)

        # Calculate the RHS of dtheta/dt =
        # g cos(theta) / v - Cl rhoa A v / (2m) - v cos(theta) / (Rp + z)
        f[4] = self.g*np.cos(y[4]) / y[2]\
            - (self.Cl * rhoa_val * A * y[2]) / (2*y[3])\
            - y[2]*np.cos(y[4]) / (self.Rp + y[1])

        # Calculate the RHS of dr/dt (radius change rate)
        # and dr/dt (spreading rate)
        # for breaking and non breaking conditions
        if break_condition:
            f[5] = y[6]
            f[6] = self.Cd * rhoa_val * y[2]**2 / (y[5]*density)
        else:
            f[5] = 0.0
            f[6] = 0.0
        return f
