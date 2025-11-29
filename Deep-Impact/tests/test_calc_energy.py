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


class TestCalculateEnergy(object):
    def test_calculate_energy_basic(self, planet):
        df = pd.DataFrame(
            {"velocity": [100, 200, 300], "mass": [10, 10, 10],
             "altitude": [10, 9, 8]}
        )

        expected_dedz = [0, -3.585086e-05, -5.975143e-05]

        result = planet.calculate_energy(df)

        assert "dedz" in result.columns, \
            "Column 'dedz' missing in dataframe"
        assert not result["dedz"].isna().any(), "dedz should not be NaN"
        assert all(
            result["dedz"].replace([np.inf, -np.inf], 0).notna()
        ), "dedz should not contain Inf or NaN"
        np.isclose(result["dedz"], expected_dedz, rtol=1e-6)

    def test_calculate_energy_empty_df(self, planet):
        df = pd.DataFrame({"velocity": [], "mass": [], "altitude": []})

        result = planet.calculate_energy(df)
        assert result.empty, "Should return an empty \
            DataFrame when input is empty"

    def test_calculate_energy_constant_values(self, planet):
        df = pd.DataFrame(
            {
                "velocity": [200, 200, 200],
                "mass": [10, 10, 10],
                "altitude": [10, 10, 10],
            }
        )

        result = planet.calculate_energy(df)
        assert (
            result["dedz"] == 0
        ).all(), "'dedz' should be 0 for constant energy or altitude"

    def test_calculate_energy_decreasing_altitude(self, planet):
        df = pd.DataFrame(
            {"velocity": [200, 250, 300], "mass": [5, 5, 5],
             "altitude": [10, 8, 6]}
        )

        result = planet.calculate_energy(df)

        assert (
            result["dedz"].iloc[1] < 0
        ), "'dedz' should be negative when altitude decreases"

    def test_calculate_energy_nan_values(self, planet):
        df = pd.DataFrame(
            {
                "velocity": [np.nan, 200, 300],
                "mass": [10, np.nan, 10],
                "altitude": [10, 9, 8],
            }
        )

        result = planet.calculate_energy(df)
        assert not result["dedz"].isna().any(), \
            "'dedz' should not contain NaN values"

    def test_calculate_energy_single_row(self, planet):
        df = pd.DataFrame({"velocity": [1000], "mass": [5000],
                           "altitude": [1000]})
        result = planet.calculate_energy(df)
        assert result["dedz"].iloc[0] == 0, \
            "dedz should be 0 for one row"
