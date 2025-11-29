# Standard library imports
from utility_scripts.data_preprocessing import load_event, load_ids, get_targets, upsample, normalise_sample
import os
import sys

# Third-party imports
import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import zoom
from sklearn.model_selection import train_test_split

# PyTorch imports
import torch
from torch.utils.data import Dataset, DataLoader


__all__ = ['LightningDataset', 'example_loader_unet']


class LightningDataset(Dataset):
    def __init__(self, ids, target_max=100):
        self.target_max = target_max
        self.events = [load_event(id) for id in ids]
        self.targets = [get_targets(event) for event in self.events]

        # Mean and standard deviation of the dataset for normalisation
        self.targets = torch.tensor(self.targets, dtype=torch.float16)
        self.std = torch.std(self.targets)
        self.mean = torch.mean(self.targets)

        for event in self.events:
            # Normalise Images
            event["vis"] = normalise_sample(event["vis"])
            event["vil"] = normalise_sample(event["vil"])
            event["ir069"] = normalise_sample(event["ir069"])
            event["ir069"] = normalise_sample(event["ir069"])
            # Upsample Images
            event["vis"] = upsample(event["vis"])
            event["vil"] = upsample(event["vil"])
            event["ir069"] = upsample(event["ir069"])
            event["ir107"] = upsample(event["ir107"])

    def __len__(self):
        "Returns total number of samples"
        return len(self.events)

    def __getitem__(self, idx):
        "Returns a single sample from the dataset"
        # Load event from id
        event = self.events[idx]

        # Get the targets and normalise
        target = self.targets[idx]
        target = (target - self.mean) / self.std

        # Convert to PyTorch tensors and stack the image types into a single tensor
        vis = torch.tensor(event["vis"], dtype=torch.float16)
        vil = torch.tensor(event["vil"], dtype=torch.float16)
        ir069 = torch.tensor(event["ir069"], dtype=torch.float16)
        ir107 = torch.tensor(event["ir107"], dtype=torch.float16)

        # Stack the inputs to form a single tensor of shape (36, 4, 384, 384)
        X = torch.stack([vis, ir069, ir107, vil], dim=0).permute(3, 0, 1, 2)
        # Convert dense_targets to PyTorch tensor and ensure shape is (36, 1, 384, 384)
        Y = target.permute(2, 0, 1).unsqueeze(1)

        return X, Y
    

def example_loader_unet():
    """
    An example implemention of a DataLoader for the given LightningDataset (for U-NET_Event model).
    Note that Dataset and DataLoader definitions will vary slighting based on model choice.
    This is based on defined input and output shapes of our models.
    """

    # Load IDs and split into train and test sets
    ids = load_ids(frac=0.05)
    train_ids, valid_ids = train_test_split(ids, test_size=0.2)

    # Print the number of events
    print(f"Number of training events: {len(train_ids)}")
    print(f"Number of testing events: {len(valid_ids)}")

    # Initialize the datasets and dataloaders
    train_dataset = LightningDataset(train_ids)
    train_dataloader = DataLoader(train_dataset, batch_size=6, shuffle=True)
    val_dataset = LightningDataset(valid_ids)
    val_dataloader = DataLoader(val_dataset, batch_size=6, shuffle=True)

    # Training DataLoader
    for X, y in train_dataloader:
        print("Training batch shapes:")
        print(X.shape)  # Should be (6, 36, 4, 64, 64)
        print(y.shape)  # Should be (6, 36, 1, 384, 384)
        break
    # Validation DataLoader
    for X, y in val_dataloader:
        print("Validation batch shapes:")
        print(X.shape)  # Should be (6, 36, 4, 64, 64)
        print(y.shape)  # Should be (6, 36, 1, 384, 384)
        break
