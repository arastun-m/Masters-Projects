import numpy as np
from pytest import fixture, mark, raises


@fixture(scope="module")
def deepimpact():
    import deepimpact

    return deepimpact


@fixture(scope="module")
def planet_constant(deepimpact):
    return deepimpact.Planet(atmos_func="constant", rho0=1.2)


@fixture(scope="module")
def planet_exponential(deepimpact):
    return deepimpact.Planet(atmos_func="exponential", rho0=1.2, H=8000)


@fixture(scope="module")
def planet_tabular(deepimpact):
    return deepimpact.Planet(atmos_func="tabular", rho0=1.2, H=8000)


@fixture(scope="module")
def tabular_data(planet_tabular):
    return planet_tabular.load_atmos_tabular_func()


class TestAtmosConstant(object):
    @mark.parametrize(
        "test_altitude, expected_rho",
        [(0, 1.2), (1, 1.2), (10, 1.2), (100, 1.2), (1000, 1.2), (10000, 1.2)],
    )
    def test_constant(self, planet_constant, test_altitude, expected_rho):
        assert np.isclose(planet_constant.rhoa(test_altitude),
                          expected_rho, rtol=1e-5)

    @mark.parametrize("test_altitude, expected_rho", [(0, 1.2), (1e6, 1.2)])
    def test_constant_edge(self, planet_constant, test_altitude, expected_rho):
        assert np.isclose(planet_constant.rhoa(test_altitude),
                          expected_rho, rtol=1e-5)

    @mark.parametrize(
        "test_altitude",
        [
            -1,
            -1e6,
        ],
    )
    def test_constant_negative(self, deepimpact, test_altitude):
        with raises(ValueError):
            planet_constant = deepimpact.Planet(
                atmos_func="constant", rho0=test_altitude
            )
            planet_constant.rhoa(test_altitude)


class TestAtmosExponential(object):
    @mark.parametrize(
        "test_altitude, expected_rho",
        [
            (0, 1.2),
            (8000, 1.2 * np.exp(-1)),
            (16000, 1.2 * np.exp(-2)),
            (24000, 1.2 * np.exp(-3)),
            (32000, 1.2 * np.exp(-4)),
            (40000, 1.2 * np.exp(-5)),
        ],
    )
    def test_exponential(self, planet_exponential,
                         test_altitude, expected_rho):
        assert np.isclose(
            planet_exponential.rhoa(test_altitude), expected_rho, rtol=1e-5
        )

    @mark.parametrize(
        "test_altitude, expected_rho",
        [
            (0, 1.2),
            (1e6, 1.2 * np.exp(-1e6 / 8000)),
        ],
    )
    def test_exponential_edge(self, planet_exponential,
                              test_altitude, expected_rho):
        assert np.isclose(
            planet_exponential.rhoa(test_altitude), expected_rho, rtol=1e-5
        )

    @mark.parametrize(
        "test_altitude, expected_rho",
        [
            (-1, 1.2 * np.exp(1 / 8000)),
            (-1e6, 1.2 * np.exp(1e6 / 8000)),
        ],
    )
    def test_exponential_negative(
        self, planet_exponential, test_altitude, expected_rho
    ):
        assert np.isclose(
            planet_exponential.rhoa(test_altitude),
            expected_rho, rtol=1e-5
        )


class TestAtmosTabular(object):
    def test_tabular_data(self, planet_tabular, tabular_data):
        for test_altitude, expected_rho in zip(*tabular_data):
            assert np.isclose(
                planet_tabular.rhoa(test_altitude), expected_rho, rtol=1e-10
            )

    def test_tabular_to_exponential(self, planet_tabular,
                                    planet_exponential):
        for test_altitude in np.linspace(
            0, 8.6e04, 100
        ):  # discrepancy values to be expected (but not too large)
            assert np.isclose(
                planet_tabular.rhoa(test_altitude),
                planet_exponential.rhoa(test_altitude),
                atol=1e-1,
            )

    def test_tabular_to_exponential_edge(
        self, planet_tabular, planet_exponential, tabular_data
    ):
        x_vals, _ = tabular_data
        for test_altitude in np.linspace(min(x_vals), 50000, 100):
            assert np.isclose(
                planet_tabular.rhoa(test_altitude),
                planet_exponential.rhoa(test_altitude),
                atol=1e-1,
            )

    def test_tabular_to_exponential_edge_negative(
        self, planet_tabular, planet_exponential, tabular_data
    ):
        x_vals, _ = tabular_data
        for test_altitude in np.linspace(-10000, min(x_vals), 100):
            assert np.isclose(
                planet_tabular.rhoa(test_altitude),
                planet_exponential.rhoa(test_altitude),
                atol=1e-1,
            )

    @mark.parametrize(
        "H",
        [
            15900,  # Venus
            8500,  # Earth
            11100,  # Mars
            27000,  # Jupiter
            59500,  # Saturn
            21000,  # Titan
            27700,  # Uranus
            19600,  # Neptune
            50000,  # Pluto
        ],
    )
    def test_tabular_to_exponential_H_vals(self, deepimpact, H):
        planet_tabular = deepimpact.Planet(atmos_func="tabular", rho0=1.2, H=H)
        planet_exponential = deepimpact.Planet(atmos_func="exponential",
                                               rho0=1.2, H=H)
        for test_altitude in np.linspace(
            0, 8.6e04, 100
        ):  # discrepancy values to be expected (but not too large)
            assert np.isclose(
                planet_tabular.rhoa(test_altitude),
                planet_exponential.rhoa(test_altitude),
                atol=1e-0,
            )
