import numpy as np
import os
from pytest import fixture, mark


@fixture(scope="module")
def deepimpact():
    import deepimpact

    return deepimpact


@fixture(scope="module")
def planet(deepimpact):
    return deepimpact.Planet()


@fixture(scope="module")
def simple_planet(deepimpact):
    return deepimpact.Planet(
        atmos_func="constant",
        Cd=1.0,
        Ch=0.0,
        Q=1e7,
        Cl=0.0,
        alpha=0.3,
        Rp=float("inf"),
        g=0.0,
        H=8000.0,
        rho0=1.2,
        pancake_factor=7.0,
    )


@fixture(scope="module")
def less_simple_planet(deepimpact):
    return deepimpact.Planet(
        atmos_func="exponential",
        Cd=1.0,
        Ch=0.0,
        Q=1e7,
        Cl=0.0,
        alpha=0.3,
        Rp=float("inf"),
        g=0.0,
        H=8000.0,
        rho0=1.2,
        pancake_factor=7.0,
    )


@fixture(scope="module")
# use this fixture to load the scenario.npz
# file form tests/scenario.npz
def scenario_npz():
    filename = "./tests/scenario.npz"
    return np.load(filename)


@fixture(scope="module")
def params_planet():
    return {
        "atmos_func": "constant",
        "atmos_filename": os.sep.join(
            (os.path.dirname(__file__), "..",
             "resources", "AltitudeDensityTable.csv")
        ),
        "Cd": 1.0,
        "Ch": 0.1,
        "Q": 1e7,
        "Cl": 1e-3,
        "alpha": 0.3,
        "Rp": 6371e3,
        "g": 9.81,
        "H": 8000.0,
        "rho0": 1.2,
        "pancake_factor": 7.0,
    }


@fixture(scope="module")
def params_entry():
    return {
        "radius": 35,
        "angle": 45,
        "strength": 1e7,
        "density": 3000,
        "velocity": 19e3,
        "init_altitude": 100e3,
        "dt": 0.05,
        "t_max": 100,
        "radians": False,
    }


@fixture(scope="module")
def params_entry_solver():
    return {
        "radius": 35,
        "velocity": 19e3,
        "density": 3000,
        "strength": 1e7,
        "angle": 45,
        "init_altitude": 100e3,
        "dt": 0.05,
        "radians": False,
    }


class TestDPToAnalytical(object):
    """
    Compares results from deepimpact solvers
    and analytical solutions.
    """

    def test_simpler_case1(self, simple_planet):
        """
        Cl, Ch and g are set to 0 for no gravity,
        no ablation, no lift
        Rd is set to infity to get a flat earth
        stremgth is set to infinity for no pancaking
        rho is constant for analytical solution
        the solution to this v(t)= 1/(Kt+1/v0)
        where K = (Cd rho A)/2m
        Solve the atmospheric entry problem for a given
        set of input parameters
        """

        radius = 35
        density = 3000
        init_velocity = 1e3

        approximation = simple_planet.solve_atmospheric_entry(
            radius=radius,
            angle=np.pi / 4,
            strength=float("inf"),
            density=density,
            velocity=init_velocity,
            radians=True,
        )
        A = np.pi * radius**2
        vol = 4 / 3 * np.pi * radius**3

        def sol(t):
            K = simple_planet.Cd * simple_planet.rho0 * A / (
                2 * density * vol)
            return 1 / (K * t + 1 / init_velocity)

        true_solution_velocity = sol(approximation["time"])
        assert np.allclose(approximation["velocity"],
                           true_solution_velocity, atol=1e-5)

    def test_simpler_case2(self, less_simple_planet):
        """
        Cl, Ch and g are set to 0 for no gravity, no ablation, no lift
        Rd is set to infity to get a flat earth
        stremgth is set to infinity for no pancaking
        rho is exponential for analytical solution
        the solution to this v(z)= v0exp[-HK/c(exp(-z/H)-exp(-z0/H))]
        where K = (Cd rho A)/2m and C = sin(angle)
        Solve the atmospheric entry problem for a given set of input parameters
        """

        radius = 35
        density = 3000
        init_velocity = 1e3
        angle = np.pi / 4
        z0 = 100e3

        approximation = less_simple_planet.solve_atmospheric_entry(
            radius=radius,
            angle=angle,
            strength=float("inf"),
            density=density,
            velocity=init_velocity,
            init_altitude=z0,
            radians=True,
        )
        A = np.pi * radius**2
        vol = 4 / 3 * np.pi * radius**3

        def sol_exp_rho(z):
            K = (
                less_simple_planet.Cd
                * less_simple_planet.rho0
                * A
                / (2 * density * vol)
            )
            C = np.sin(angle)
            return init_velocity * np.exp(
                -less_simple_planet.H
                * K
                / C
                * (
                    np.exp(-z / less_simple_planet.H)
                    - np.exp(-z0 / less_simple_planet.H)
                )
            )

        true_solution_velocity = sol_exp_rho(approximation["altitude"])
        assert np.allclose(approximation["velocity"],
                           true_solution_velocity, atol=1e-5)

    def test_simpler_case3(self, less_simple_planet):
        """
        Cl, Ch and g are set to 0 for no gravity, no ablation, no lift
        Rd is set to infity to get a flat earth
        stremgth is set to infinity for no pancaking
        rho is exponential for analytical solution
        the solution to this v(z)= v0exp[-HK/c(exp(-z/H)-exp(-z0/H))]
        where K = (Cd rho A)/2m and C = sin(angle)
        Solve the atmospheric entry problem for a given set of input parameters
        """

        radius = 35
        density = 3000
        init_velocity = 1e3
        angle = np.pi / 4
        z0 = 100e3

        approximation = less_simple_planet.solve_atmospheric_entry(
            radius=radius,
            angle=angle,
            strength=float("inf"),
            density=density,
            velocity=init_velocity,
            init_altitude=z0,
            radians=True,
        )
        A = np.pi * radius**2
        vol = 4 / 3 * np.pi * radius**3

        def sol_exp_rho(z):
            K = (
                less_simple_planet.Cd
                * less_simple_planet.rho0
                * A
                / (2 * density * vol)
            )
            C = np.sin(angle)
            return init_velocity * np.exp(
                -less_simple_planet.H
                * K
                / C
                * (
                    np.exp(-z / less_simple_planet.H)
                    - np.exp(-z0 / less_simple_planet.H)
                )
            )

        true_solution_velocity = sol_exp_rho(approximation["altitude"])
        assert np.allclose(approximation["velocity"],
                           true_solution_velocity, atol=1e-6)


class TestDPTimeoutEntry(object):
    """
    Checks for potential timeout with range of
    solve_atmospheric_entry parameters.
    paramaters = [dt, radius, angle, strength, density,
    velocity, init_altitude, radians].
    """

    @mark.parametrize(
        "input_dt",
        [
            (0.25),
            (0.125),
            (0.0625),
            (0.03125),
            (0.0125),
            (0.00625),
        ],
    )
    @mark.timeout(120)
    def test_timeout_dt(object, planet, params_entry_solver, input_dt):
        params_entry_solver["dt"] = input_dt
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_radius",
        [
            (35),
            (50),
            (75),
            (100),
            (150),
            (200),
        ],
    )
    @mark.timeout(120)
    def test_timmeout_radius(object, planet,
                             params_entry_solver, input_radius):
        params_entry_solver["radius"] = input_radius
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_angle",
        [
            (15),
            (30),
            (45),
            (60),
            (75),
            (90),
        ],
    )
    @mark.timeout(120)
    def test_timeout_angle(object, planet,
                           params_entry_solver, input_angle):
        params_entry_solver["angle"] = input_angle
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_strength",
        [
            (1e3),
            (1e5),
            (1e7),
            (1e9),
            (1e11),
        ],
    )
    @mark.timeout(120)
    def test_timeout_strength(object, planet,
                              params_entry_solver, input_strength):
        params_entry_solver["strength"] = input_strength
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_density",
        [
            (1000),
            (2000),
            (3000),
            (4000),
            (5000),
        ],
    )
    @mark.timeout(120)
    def test_timeout_density(object, planet,
                             params_entry_solver, input_density):
        params_entry_solver["density"] = input_density
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_velocity",
        [
            (1e3),
            (5e3),
            (10e3),
            (15e3),
            (20e3),
        ],
    )
    @mark.timeout(120)
    def test_timeout_velocity(object, planet,
                              params_entry_solver, input_velocity):
        params_entry_solver["velocity"] = input_velocity
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_init_altitude",
        [
            (10e3),
            (50e3),
            (100e3),
            (150e3),
            (200e3),
        ],
    )
    @mark.timeout(120)
    def test_timeout_init_altitude(
        object, planet, params_entry_solver, input_init_altitude
    ):
        params_entry_solver["init_altitude"] = input_init_altitude
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_radians",
        [
            (True),
            (False),
        ],
    )
    @mark.timeout(120)
    def test_timeout_radians(object, planet,
                             params_entry_solver, input_radians):
        params_entry_solver["radians"] = input_radians
        planet.solve_atmospheric_entry(**params_entry_solver)


class TestDPTimeoutPlanet(object):
    """
    Checks for potential timeout with range of
    plane __init__ parameters.
    paramaters =
    [atmos_func, Cd, Ch, Q, Cl, alpha, Rp, g, H, rho0, pancake_factor].
    """

    @mark.parametrize(
        "input_atmos_func",
        [
            ("constant"),
            ("exponential"),
            ("table"),
        ],
    )
    @mark.timeout(120)
    def test_timeout_atmos_func(
        object, deepimpact, params_planet,
        params_entry_solver, input_atmos_func
    ):
        params_planet["atmos_func"] = input_atmos_func
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_Cd",
        [
            (0.1),
            (0.5),
            (1.0),
            (2.0),
            (5.0),
        ],
    )
    @mark.timeout(120)
    def test_timeout_Cd(
        object, deepimpact, params_planet,
        params_entry_solver, input_Cd
    ):
        params_planet["Cd"] = input_Cd
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_Ch",
        [
            (0.01),
            (0.05),
            (0.1),
            (0.5),
            (1.0),
        ],
    )
    @mark.timeout(120)
    def test_timeout_Ch(
        object, deepimpact, params_planet, params_entry_solver, input_Ch
    ):
        params_planet["Ch"] = input_Ch
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_Q",
        [
            (1e3),
            (1e5),
            (1e7),
            (1e9),
            (1e11),
        ],
    )
    @mark.timeout(120)
    def test_timeout_Q(object, deepimpact, params_planet,
                       params_entry_solver, input_Q):
        params_planet["Q"] = input_Q
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_Cl",
        [
            (1e-5),
            (1e-4),
            (1e-3),
            (1e-2),
            (1e-1),
        ],
    )
    @mark.timeout(120)
    def test_timeout_Cl(
        object, deepimpact, params_planet,
        params_entry_solver, input_Cl
    ):
        params_planet["Cl"] = input_Cl
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_alpha",
        [
            (0.1),
            (0.3),
            (0.5),
            (0.7),
            (0.9),
        ],
    )
    @mark.timeout(120)
    def test_timeout_alpha(
        object, deepimpact, params_planet,
        params_entry_solver, input_alpha
    ):
        params_planet["alpha"] = input_alpha
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_Rp",
        [
            (6.371e6),
            (6.371e7),
            (6.371e8),
            (6.371e9),
            (6.371e10),
        ],
    )
    @mark.timeout(120)
    def test_timeout_Rp(
        object, deepimpact, params_planet,
        params_entry_solver, input_Rp
    ):
        params_planet["Rp"] = input_Rp
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_g",
        [
            (9.81),
            (19.62),
            (29.43),
            (39.24),
            (49.05),
        ],
    )
    @mark.timeout(120)
    def test_timeout_g(object, deepimpact,
                       params_planet, params_entry_solver, input_g):
        params_planet["g"] = input_g
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_H",
        [
            (5000),
            (10000),
            (15000),
            (20000),
            (25000),
        ],
    )
    @mark.timeout(120)
    def test_timeout_H(object, deepimpact,
                       params_planet, params_entry_solver, input_H):
        params_planet["H"] = input_H
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_rho0",
        [
            (0.5),
            (1.0),
            (1.5),
            (5),
            (10),
        ],
    )
    @mark.timeout(120)
    def test_timeout_rho0(
        object, deepimpact, params_planet,
        params_entry_solver, input_rho0
    ):
        params_planet["rho0"] = input_rho0
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)

    @mark.parametrize(
        "input_pancake_factor",
        [
            (5.0),
            (7.0),
            (9.0),
            (11.0),
            (20.0),
        ],
    )
    @mark.timeout(120)
    def test_timeout_pancake_factor(
        object, deepimpact, params_planet,
        params_entry_solver, input_pancake_factor
    ):
        params_planet["pancake_factor"] = input_pancake_factor
        planet = deepimpact.Planet(**params_planet)
        planet.solve_atmospheric_entry(**params_entry_solver)
