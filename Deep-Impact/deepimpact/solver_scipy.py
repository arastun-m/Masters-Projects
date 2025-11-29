"""
Scipy based implementations of the solver.
Used for comparison in testing and debugging (with deep.impact solvers).
"""

from scipy.integrate import RK45 as scipy_RK45
import numpy as np
import deepimpact


__all__ = ["prep_call_RK45", "prep_call_deepimpact_RK4"]


def prep_call_deepimpact_RK4(params_planet, params_entry,
                             meteor_equation="simplified"):
    planet = deepimpact.Planet(**params_planet)
    meteor_equation = (
        planet.simplified_meteor_eq
        if meteor_equation == "simplified"
        else planet.meteor_eq
    )

    mass = (4 / 3) * np.pi * params_entry["radius"] ** 3\
        * params_entry["density"]
    spreading_rate = 0
    y0 = np.array(
        [
            0,
            params_entry["init_altitude"],
            params_entry["velocity"],
            mass,
            params_entry["angle"],
            params_entry["radius"],
            spreading_rate,
        ]
    )

    y_all_RK4, t_all_RK4 = planet.RK4(
        meteor_equation,
        y0,
        t0=0,
        t_max=params_entry["t_max"],
        dt=params_entry["dt"],
        strength=params_entry["strength"],
        density=params_entry["density"],
    )
    return y_all_RK4, t_all_RK4


def prep_call_RK45(params_planet, params_entry, meteor_equation="simplified"):
    planet = deepimpact.Planet(**params_planet)
    meteor_equation = (
        planet.simplified_meteor_eq
        if meteor_equation == "simplified"
        else planet.meteor_eq
    )

    density = params_entry["density"]
    mass = (4 / 3) * np.pi * params_entry["radius"] ** 3 * density
    spreading_rate = 0

    dt = params_entry["dt"]
    t_max = params_entry["t_max"]
    strength = params_entry["strength"]
    density = params_entry["density"]

    y0 = np.array(
        [
            0,
            params_entry["init_altitude"],
            params_entry["velocity"],
            mass,
            params_entry["angle"],
            params_entry["radius"],
            spreading_rate,
        ]
    )

    def f(t, y):
        return meteor_equation(t, y, break_condition=False, density=density)

    return m_scipy_RK45(
        f, y0, t0=0, t_max=t_max, dt=dt, strength=strength, density=density
    )


def m_scipy_RK45(f, y0, t0, t_max, dt, strength, density):
    scipy_model = scipy_RK45(f, t0=0, y0=y0, t_bound=t_max, max_step=dt)

    y_all_scipy = [y0]
    t_all_scipy = [t0]
    while scipy_model.t < t_max:
        scipy_model.step()
        y_all_scipy.append(scipy_model.y)
        t_all_scipy = np.append(t_all_scipy, scipy_model.t)

    return y_all_scipy, t_all_scipy
