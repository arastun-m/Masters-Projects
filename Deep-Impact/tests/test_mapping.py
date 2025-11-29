from pytest import fixture
import folium


@fixture(scope="module")
def deepimpact():
    import deepimpact

    return deepimpact


@fixture(scope="module")
def plot_circle(deepimpact):
    return deepimpact.plot_circle()


# 1. test_plot_circle_basic
def test_plot_circle_basic(deepimpact):
    blat = 52.85
    blon = -2.96
    radii = [500, 1300, 2000, 3000]
    fmap = deepimpact.plot_circle(blat, blon, radii)

    assert fmap
    assert isinstance(fmap, folium.Map)


# 2. test_plot_circle_no_fmap
def test_plot_circle_no_fmap(deepimpact):
    blat = 52.85
    blon = -2.96
    radii = [500, 1300, 2000, 3000]
    fmap = None
    fmap = deepimpact.plot_circle(blat, blon, radii, fmap)

    assert fmap
    assert isinstance(fmap, folium.Map)


# 3. test_plot_circle_bad_fmap
def test_plot_circle_bad_fmap(deepimpact):
    blat = 52.85
    blon = -2.96
    radii = [500, 1300, 2000, 3000]
    fmap = "fmap"
    try:
        deepimpact.plot_circle(blat, blon, radii, fmap)
    except TypeError as e:
        assert str(e) == "fmap must be a folium.Map object"


# 4. test with optional parameters
def test_plot_circle_optional(deepimpact):
    blat = 52.85
    blon = -2.96
    radii = [500, 1300, 2000, 3000]
    elat = 53.5
    elon = -2.5
    fmap = deepimpact.plot_circle(blat, blon, radii, elat, elon)

    assert fmap
    assert isinstance(fmap, folium.Map)
