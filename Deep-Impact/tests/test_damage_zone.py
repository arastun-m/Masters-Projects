import pytest
from pytest import fixture
import numpy as np # noqa
from deepimpact import damage_zones, impact_risk
from deepimpact.solver import Planet


@fixture(scope='module')
def deepimpact():
    import deepimpact
    return deepimpact


@fixture(scope='module')
def planet(deepimpact):
    return deepimpact.Planet()


# Fixtures to set up test data and dependencies
@pytest.fixture(scope='module')
def test_outcome():
    """Fixture for a test outcome dictionary."""
    return {
        'burst_peak_dedz': 1000.0,
        'burst_altitude': 9000.0,
        'burst_distance': 90000.0,
        'burst_energy': 6000.0,
        'outcome': 'Airburst'
    }


@pytest.fixture(scope='module')
def pressures():
    """Fixture for a list of pressure thresholds."""
    return [1e3, 5e3, 25e3, 40e3]


# Test cases
def test_damage_zones_basic(test_outcome, pressures):
    """
    Test the basic functionality of damage_zones.
    """
    blat, blon, damrad = \
        damage_zones(test_outcome, 55.0, 0.0, 135.0, pressures)

    assert isinstance(blat, float), "Latitude (blat) should be a float"
    assert isinstance(blon, float), "Longitude (blon) should be a float"
    assert isinstance(damrad, list), "Damage radii (damrad) should be a list"
    assert len(damrad) == len(pressures), \
        "Damage radii length should match the pressures input"
    assert all(r > 0 for r in damrad), "All radii should be positive"


def test_damage_zones_alternate_scenario():
    """
    Test damage_zones with a different set of input data.
    """
    # Define an alternate outcome and pressure scenario
    alternate_outcome = {
        'burst_peak_dedz': 2000.0,
        'burst_altitude': 12000.0,
        'burst_distance': 150000.0,
        'burst_energy': 8000.0,
        'outcome': 'Airburst'
    }
    alternate_pressures = [1.5e3, 3.0e3, 7.5e3, 15.0e3]
    # Alternate pressure levels

    # Call the damage_zones function
    blat, blon, damrad = \
        damage_zones(alternate_outcome, 60.0, 10.0, 45.0, alternate_pressures)

    # Assertions
    assert isinstance(blat, float), "Latitude (blat) should be a float"
    assert isinstance(blon, float), "Longitude (blon) should be a float"
    assert isinstance(damrad, list), "Damage radii (damrad) should be a list"
    assert len(damrad) == len(alternate_pressures), \
        "Length of damrad should match the length of pressures"
    assert all(r > 0 for r in damrad), "All radii should be positive"
    assert -90 <= blat <= 90, f"Latitude (blat) out of valid range: {blat}"
    assert -180 <= blon <= 180, f"Longitude (blon) out of valid range: {blon}"


def test_damage_zones_invalid_outcome():
    """
    Test damage_zones with an invalid outcome dictionary.
    """
    invalid_outcome = {
        'burst_altitude': None,
        'burst_energy': None,
        'burst_distance': None
    }
    pressures = [1e3, 5e3]

    with pytest.raises(TypeError):
        damage_zones(invalid_outcome, 55.0, 0.0, 135.0, pressures)


def test_impact_risk():
    p = Planet()
    temp = impact_risk(p)
    assert ("CV100FD" in temp[0]["postcode"].values)
    assert (temp[0].loc[temp[0]["postcode"] == "CV100FD", "probability"].iloc[0] == 0.1) # noqa
