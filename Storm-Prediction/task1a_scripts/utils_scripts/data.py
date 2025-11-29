# Data preprocessing utilities for the Conv3D model.

import h5py
import numpy as np
import tensorflow as tf
from itertools import cycle
from skimage.transform import resize

def extract_event_ids(file_path):
    """
    Extract all event IDs from the HDF5 file.

    Parameters:
        file_path (str): Path to the HDF5 file.
    Returns:
        list: List of event IDs.

    """
    with h5py.File(file_path, "r") as f:
        return list(f.keys())

def standard_normalize(data):
    """
    Standard normalization: (data - mean) / std.
    Avoid division by zero by adding a small epsilon to std.

    Parameters:
        data (numpy.ndarray): Input data.
    Returns:
        numpy.ndarray: Normalized data.
    """
    mean = np.mean(data)
    std = np.std(data)
    return (data - mean) / (std + 1e-8)

def mse_ssim_loss(alpha=0.7, beta=0.3):
    """
    Combined MSE + SSIM loss function.

    Parameters:
        alpha (float): Weight for the MSE component.
        beta (float): Weight for the SSIM component.
    Returns:
        Callable loss function.
    """
    def loss(y_true, y_pred):
        # Compute MSE loss
        mse_loss = tf.reduce_mean(tf.square(y_true - y_pred))

        # Compute SSIM loss (1 - mean SSIM)
        ssim_value = tf.image.ssim(y_true, y_pred, max_val=1.0)  # Normalize inputs to [0, 1]
        ssim_loss = 1 - tf.reduce_mean(ssim_value)

        # Combine MSE and SSIM losses
        return alpha * mse_loss + beta * ssim_loss

    return loss

def min_max_normalize(data):
    """
    Applies Min-Max Normalization to scale data into a specified range.

    Parameters:
        data (numpy.ndarray): Input data.
    Returns:
        numpy.ndarray: Normalized data.

    """
    min_val = np.min(data)
    max_val = np.max(data)
    return (data - min_val) / (max_val - min_val)

# This below is the data generator but selects randomly one index to start the count of 24 from. Then selects frist 12 and next 12 frames.
# def dynamic_data_generator(file_path, event_ids, batch_size=8, input_length=12, output_length=12, target_shape=(384, 384)):
#     """
#     Dynamically loads and processes data from HDF5 file for training and validation.
#     """
#     event_cycle = cycle(event_ids)  # Cycle through event IDs indefinitely
#     np.random.shuffle(event_ids)  # Shuffle event IDs at the start

#     with h5py.File(file_path, "r") as f:
#         while True:
#             X_batch, Y_batch = [], []

#             # Select a batch of event IDs
#             unique_event_ids = [next(event_cycle) for _ in range(batch_size)]

#             for event_id in unique_event_ids:
#                 if event_id not in f:
#                     raise ValueError(f"Event ID {event_id} not found in file.")

#                 # Load dataset (Only 'vil' is used; extend for multiple features if needed)
#                 vil = f[event_id]["vil"][:]

#                 # Standardize the dataset
#                 vil = min_max_normalize(vil)

#                 # Determine the total number of valid frames for sliding windows
#                 min_frames = vil.shape[0]
#                 total_windows = (min_frames - input_length - output_length) // 12 + 1

#                 if total_windows <= 0:
#                     continue  # Skip if not enough frames

#                 # Randomly select a starting sliding window index
#                 window_idx = np.random.randint(0, total_windows)
#                 start_frame = window_idx * 12

#                 # Define input and output frame slices
#                 input_frames = slice(start_frame, start_frame + input_length)
#                 output_frames = slice(start_frame + input_length, start_frame + input_length + output_length)

#                 # Process inputs
#                 X_resized = np.array([
#                     upsample(vil[frame], target_shape)  # Resize each frame
#                     for frame in range(input_frames.start, input_frames.stop)
#                 ])  # Shape: (input_length, height, width)

#                 # Process outputs
#                 Y_resized = np.array([
#                     upsample(vil[frame], target_shape)
#                     for frame in range(output_frames.start, output_frames.stop)
#                 ])  # Shape: (output_length, height, width)

#                 # Reshape data for Conv3D:
#                 # Move the time axis to the second dimension and add a channel axis
#                 X_final = X_resized[:, :, :, np.newaxis]  # (input_length, height, width, 1)
#                 Y_final = Y_resized[:, :, :, np.newaxis]  # (output_length, height, width, 1)

#                 X_batch.append(X_final)
#                 Y_batch.append(Y_final)

#             # Convert to NumPy arrays and return batch in shape (batch, time, height, width, channels)
#             X_batch = np.array(X_batch)  # Shape: (batch_size, input_length, height, width, 1)
#             Y_batch = np.array(Y_batch)  # Shape: (batch_size, output_length, height, width, 1)

#             yield X_batch, Y_batch

def dynamic_data_generator(file_path, event_ids, batch_size=16, input_length=12, output_length=12, target_shape=(384, 384)):
    """
    Dynamically loads and processes data from HDF5 file for training and validation.
    Applies a sliding window to extract sequences (12 input + 12 output) from 36-frame events.

    Parameters:
        file_path (str): Path to the HDF5 file.
        event_ids (list): List of event IDs.
        batch_size (int): Number of samples per batch.
        input_length (int): Number of input frames.
        output_length (int): Number of output frames.
        target_shape (tuple): Target shape for resizing.
    Returns:
        tuple: Batch of input and output data.

    """
    event_cycle = cycle(event_ids)  # Cycle through event IDs indefinitely
    np.random.shuffle(event_ids)  # Shuffle event IDs at the start

    with h5py.File(file_path, "r") as f:
        while True:
            X_batch, Y_batch = [], []
            count = 0  # Track how many samples are added

            # Select a batch of event IDs
            unique_event_ids = [next(event_cycle) for _ in range(batch_size)]

            for event_id in unique_event_ids:
                if event_id not in f:
                    raise ValueError(f"Event ID {event_id} not found in file.")

                # Load dataset (Only 'vil' is used; extend for multiple features if needed)
                vil = f[event_id]["vil"][:]  # Shape: (36, H, W)

                # Standardize the dataset
                vil = min_max_normalize(vil)

                # Ensure we have at least 36 frames
                if vil.shape[0] < 36:
                    continue  # Skip events with fewer frames

                # Apply Sliding Window: Move from 1st frame (index 0) to 13th frame (index 12)
                for start_frame in range(13):  # Sliding window range: 0 to 12
                    input_frames = slice(start_frame, start_frame + input_length)  # 12 frames
                    output_frames = slice(start_frame + input_length, start_frame + input_length + output_length)  # Next 12 frames

                    # Process inputs
                    X_resized = np.array([
                        upsample(vil[frame], target_shape)  # Resize each frame
                        for frame in range(input_frames.start, input_frames.stop)
                    ])  # Shape: (input_length, height, width)

                    # Process outputs
                    Y_resized = np.array([
                        upsample(vil[frame], target_shape)
                        for frame in range(output_frames.start, output_frames.stop)
                    ])  # Shape: (output_length, height, width)

                    # Reshape data for Conv3D: Move time axis to second dimension and add a channel axis
                    X_final = X_resized[:, :, :, np.newaxis]  # (12, height, width, 1)
                    Y_final = Y_resized[:, :, :, np.newaxis]  # (12, height, width, 1)

                    X_batch.append(X_final)
                    Y_batch.append(Y_final)
                    count += 1  # Increase sample count

                    # Stop adding more samples if batch size is reached
                    if count >= batch_size:
                        break  # Stop sliding window iterations

                # Stop adding more events if batch size is reached
                if count >= batch_size:
                    break

            # Convert to NumPy arrays and return batch in shape (batch_size, time, height, width, channels)
            yield np.array(X_batch), np.array(Y_batch)
