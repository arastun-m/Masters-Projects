
from torch.utils.data import Dataset, DataLoader
from preprocess import strafied_sampling, process_data


# Define the Dataset class
class StormDataset(Dataset):
    def __init__(self, data_inputs, data_targets):
        self.data_inputs = data_inputs
        self.data_targets = data_targets

    def __len__(self):
        return self.data_inputs.shape[0]

    def __getitem__(self, idx):
        inputs = self.data_inputs[idx]
        target = self.data_targets[idx]

        return inputs, target


def get_data_loader():
    # Load the data
    train_ids, val_ids = strafied_sampling()
    train_inputs, train_targets = process_data(train_ids)
    val_inputs, val_targets = process_data(val_ids)
    # Create the DataLoader
    batch_size = 36
    train_dataset = StormDataset(train_inputs, train_targets)
    train_loader = DataLoader(train_dataset,
        batch_size=batch_size,
        shuffle=True,
        num_workers=8,
        pin_memory=True
    )
    val_dataset = StormDataset(val_inputs, val_targets)
    val_loader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False, num_workers=8, pin_memory=True)
    return train_loader, val_loader


if __name__ == "__main__":
    train_loader, val_loader = get_data_loader()
    train_input, train_target = next(iter(train_loader))
    print(f"✅ Data Loaded: Input {train_input.shape}, Target {train_target.shape}")
    val_input, val_target = next(iter(val_loader))
    print(f"✅ Data Loaded: Input {val_input.shape}, Target {val_target.shape}")