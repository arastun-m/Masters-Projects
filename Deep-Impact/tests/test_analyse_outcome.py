import pandas as pd
import numpy as np
from pytest import fixture


@fixture(scope="module")
def deepimpact():
    import deepimpact

    return deepimpact


@fixture(scope="module")
def planet(deepimpact):
    return deepimpact.Planet()


@fixture(scope="module")
def airburst_input():
    return {
        "velocity": [15, 14, 13],
        "mass": [1000, 1000, 1000],
        "angle": [45, 45, 45],
        "altitude": [10, 5, 0],
        "distance": [0, 50, 100],
        "radius": [1, 1, 1],
        "dedz": [0, 200, 100],
    }


@fixture(scope="module")
def airburst_expected():
    return {
        "outcome": "Airburst",
        "burst_peak_dedz": 200,
        "burst_altitude": 5,
        "burst_distance": 50,
        "burst_energy": (0.5 * 1000 * 15**2 - 0.5 * 1000 * 14**2) / 4.184e12,
    }


@fixture(scope="module")
def cratering_input():
    return {
        "velocity": [20, 18, 10],
        "mass": [1200, 1200, 1200],
        "angle": [30, 30, 30],
        "altitude": [5, 2, 0],
        "distance": [0, 100, 200],
        "radius": [1, 1, 1],
        "dedz": [50, 100, 150],
    }


@fixture(scope="module")
def cratering_expected():
    ground_energy = 0.5 * 1200 * 10**2 / 4.184e12
    initial_energy = 0.5 * 1200 * 20**2 / 4.184e12
    total_energy_lost = initial_energy - ground_energy
    expected = {
        "outcome": "Cratering",
        "burst_peak_dedz": 150,
        "burst_altitude": 0,
        "burst_distance": 200,
        "burst_energy": max(ground_energy, total_energy_lost),
    }
    return expected


@fixture(scope="module")
def no_event_input():
    return {
        "velocity": [],
        "mass": [],
        "angle": [],
        "altitude": [],
        "distance": [],
        "radius": [],
        "dedz": [],
    }


@fixture(scope="module")
def no_event_expected():
    return {
        "outcome": "Unknown",
        "burst_peak_dedz": 0.0,
        "burst_altitude": 0.0,
        "burst_distance": 0.0,
        "burst_energy": 0.0,
    }


class TestOutcomeEvent(object):
    def test_airburst_event(self, planet, airburst_input, airburst_expected):
        result = pd.DataFrame(airburst_input)
        outcome = planet.analyse_outcome(result)
        assert outcome["outcome"] == airburst_expected["outcome"]
        assert np.isclose(outcome["burst_peak_dedz"],
                          airburst_expected["burst_peak_dedz"], rtol=1e-6)
        assert np.isclose(outcome["burst_altitude"],
                          airburst_expected["burst_altitude"], rtol=1e-6)
        assert np.isclose(outcome["burst_distance"],
                          airburst_expected["burst_distance"], rtol=1e-6)
        assert np.isclose(outcome["burst_energy"],
                          airburst_expected["burst_energy"], rtol=1e-6)

    def test_cratering_evenet(self, planet, cratering_input,
                              cratering_expected):
        result = pd.DataFrame(cratering_input)
        outcome = planet.analyse_outcome(result)
        assert outcome["outcome"] == cratering_expected["outcome"]
        assert np.isclose(outcome["burst_peak_dedz"],
                          cratering_expected["burst_peak_dedz"], rtol=1e-6)
        assert np.isclose(outcome["burst_altitude"],
                          cratering_expected["burst_altitude"], rtol=1e-6)
        assert np.isclose(outcome["burst_distance"],
                          cratering_expected["burst_distance"], rtol=1e-6)
        assert np.isclose(outcome["burst_energy"],
                          cratering_expected["burst_energy"], rtol=1e-6)

    def test_no_event(self, planet, no_event_input, no_event_expected):
        result = pd.DataFrame(no_event_input)
        outcome = planet.analyse_outcome(result)
        assert outcome == no_event_expected
