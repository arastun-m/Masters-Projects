"""
Data preprocessing utility functions

Includes:
- load_ids: Load a random selection of unique event IDs from a CSV file.
- load_event: Load event data from an HDF5 file for a given event ID.
- down_sample: Downsample a 3D image to a specified size.
- upsample: Upsample data to a target resolution.
- normalise_sample: Normalise an image to the [0, 1] range.
- get_targets: Generate a sparse target mask for lightning strikes in each frame of an event.
"""


# Standard library imports
import os
import sys
import random

# Third-party imports
import numpy as np
import pandas as pd
import h5py
from scipy.ndimage import zoom

# PyTorch imports
import torch
from torch import nn
from torch.utils.data import DataLoader
import torch.optim as optim


__all__ = ['load_ids', 'load_event', 'down_sample', 'upsample',
            'normalise_sample', 'get_targets', 'get_lightning_time_series',
            'TotalAbsoluteError']


def load_ids(file_name="data/events.csv", n=10):
    "Load ids"
    df = pd.read_csv(file_name)
    ids = df.id.unique()
    ids = np.random.choice(ids, size=n, replace=False)
    return ids

def load_event(id):
    "Load event"
    with h5py.File(f'data/train.h5','r') as f:
        event = {img_type: f[id][img_type][:] for img_type in ['vis', 'ir069', 'ir107', 'vil', 'lght']}
    return event

def down_sample(img, to_size):
    """
    Downsample image.

    :param img: Input image of shape [H, W, D] where D is the number of frames.
    :param to_size: Target size for downsampling (e.g., 64 for 64x64).
    :return: Downsampled image of shape [to_size, to_size, D].
    """
    if img.ndim != 3:
        raise ValueError("Input image must have 3 dimensions [H, W, D]")

    img = torch.tensor(img, dtype=torch.float32)
    # Permute to shape [D, H, W] for processing
    img = img.permute(2, 0, 1).unsqueeze(1)  # Shape: [D, 1, H, W]
    downsampled_img = torch.nn.functional.interpolate(img, size=(to_size, to_size), mode='bilinear', align_corners=False)
    # Squeeze and permute back to shape [to_size, to_size, D]
    downsampled_img = downsampled_img.squeeze(1).permute(1, 2, 0).numpy()
    return downsampled_img

def upsample(data, target_shape=(384, 384)):
    """
    Upsample data to target resolution.
    :param data: Original data, shape (height, width).
    :param target_shape: Target resolution (height, width).
    :return: Upsampled data.
    """

    zoom_factors = (target_shape[0] / data.shape[0], target_shape[1] / data.shape[1], 1)
    return zoom(data, zoom_factors, order=1)


def normalise_sample(img):
    """
    Normalise image to [0, 1] range.
    """
    img = img - np.min(img)
    img = img / np.max(img)
    return img


def get_targets(event):
    """
    Maps lightning strikes of an event into a sparse target mask.
    Returned targets represent lightning strikes map for each frame (36) of the event.

    :rtype: list of sparse matrices of shape (384, 384) for each frame
    """
    t = event["lght"][:,0]
    target = np.zeros((384, 384, 36))

    # For each frame in the event
    for ti in range(36):
        # Find which lightning strikes fall in current frame and store their coodinates
        f = (t >= ti*5*60) & (t < ti*5*60 + 5*60)
        xs = event["lght"][f,3]
        ys = event["lght"][f,4]

        # Set the coordinates to 1 in the target tensor
        for x, y in zip(xs, ys):
            target[int(x), int(y), ti] += 1
    return target


def get_lightning_time_series(prediction):
    """
    Converts the given prediction (36 lightning maps of 36 frames of an event) 
    into a ligthning time series of that event.

    :param target: list of sparse matrices of shape (384, 384, 36) representing lightning strikes map for each frame
    :rtype: lightning time series of that prediction/event.
    """
    lght = []
    n_frames = 36 # assumes 36 frames
    # lightining strike values (in the map) above masked 
    # value are considered as lightnings
    mask_value = 1

    # For each frame in the target
    prediction = prediction.reshape(384, 384, n_frames)
    for ti in range(n_frames):
        frame = prediction[:, :, ti]
        xs, ys = np.where(frame >= mask_value)
        
        # For each lightning strike in the frame
        for x, y in zip(xs, ys):
            count = frame[x, y]
            count = 1 if count > 0 else 0 # only store one lightning per location for now
            for _ in range(int(count)):
                lght.append([ti * 5 * 60, 0, 0, x, y])

    lght = np.array(lght)
    return lght


class TotalAbsoluteError(nn.Module):
    """
    Custom loss function used by the U-Net model for frame prediction.
    """
    def __init__(self):
        super(TotalAbsoluteError, self).__init__()

    def forward(self, input, target):
        # Compute the absolute error
        absolute_error = torch.abs(input - target)
        # Sum up all the absolute errors
        total_absolute_error = absolute_error.sum()
        return total_absolute_error