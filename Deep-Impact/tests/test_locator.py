from pytest import fixture


@fixture(scope='module')
def deepimpact():
    import deepimpact
    return deepimpact


@fixture(scope='module')
def loc(deepimpact):
    return deepimpact.GeospatialLocator()


def test_locator_postcodes(loc):

    latlon = (51.421079, -0.253638)

    result = loc.get_postcodes_by_radius(latlon, [100, 101])
    value = "SW200AA"
    value1 = ["KT2 7DS", "SW200AA"]
    assert (value in result[0])
    assert (value1 == result[1])


def test_locator_postcodes2(loc):

    latlon = (57.13610799999999, -2.123629)

    result = loc.get_postcodes_by_radius(latlon, [1, 2])
    value = "AB106PJ"
    value1 = ["AB106PJ"]
    assert (value in result[0])
    assert (value1 == result[1])


def test_population_by_radius(loc):

    latlon = (49.9582079, -6.34465248)

    result = loc.get_population_by_radius(latlon, [997, 7])
    res = loc.get_population_by_radius([-7.30875516, 49.8459863], [5e2])

    value = 0
    value1 = 135
    value2 = 54
    assert (res[0] == value)
    assert (result[0] == value1)
    assert (result[1] == value2)


def test_population_by_radius2(loc):

    latlon = (57.6945793, -2.83202658)

    result = loc.get_population_by_radius(latlon, [1, 1400])

    value = 181.0
    value1 = 1023

    assert (result[0] == value)
    assert (result[1] == value1)
