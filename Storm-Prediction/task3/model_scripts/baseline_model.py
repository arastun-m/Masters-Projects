"""
Contains the implemntation of the baseline model.

"""


from utility_scripts.data_preprocessing import normalise_sample, get_lightning_time_series


def baseline_model(event, threshold=0.7):
    """
    Demonstrates the correlation between radar images and the location of lightning strikes.
    The baseline model uses the VIL data to generate a lightning time series.

    Parameters:
    - event (dict): A dictionary representing a single storm event.
    - threshold (float): A threshold value for generating the lightning time series.
    
    Returns:
    - numpy.ndarray: A lightning time series array with shape (N, 5).
    """
    # Normalise vil
    vil = normalise_sample(event["vil"])
    # Return lightning time series at a higher threshold
    lght = get_lightning_time_series(vil, threshold=threshold)

    return lght
