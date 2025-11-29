import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow.keras.models import Model
import torch
import torch.nn as nn

def generate_predictions(model, test_generator):
    """
    Fetches a batch from the test generator and generates predictions.

    Returns:
        - X_test_sample: Input sequence (past frames)
        - Y_test_sample: Ground truth future sequence
        - Y_pred_sample: Model predicted future sequence
    """
    # Get one batch from test generator
    X_test_batch, Y_test_batch = next(test_generator)

    # Make predictions
    Y_pred = model.predict(X_test_batch)  # Output shape: (batch, 12, 384, 384, 1)

    # Select the first sample from the batch
    X_test_sample = X_test_batch[0]  # Shape: (12, 384, 384, 1)
    Y_test_sample = Y_test_batch[0]  # Shape: (12, 384, 384, 1)
    Y_pred_sample = Y_pred[0]  # Shape: (12, 384, 384, 1)

    return X_test_sample, Y_test_sample, Y_pred_sample

def plot_prediction_comparison(X_input, Y_real, Y_pred, num_frames=12):
    """
    Plot the first num_frames of input, real vs predicted sequences side by side.

    - First row: Input frames (past observations)
    - Second row: Real future frames (ground truth)
    - Third row: Predicted future frames
    """
    fig, axes = plt.subplots(3, num_frames, figsize=(20, 8))

    for i in range(num_frames):
        # Input sequence (past)
        axes[0, i].imshow(X_input[i, :, :, 0], cmap="turbo")
        axes[0, i].set_title(f"Input Frame {i+1}")
        axes[0, i].axis("off")

        # Real sequence (ground truth)
        axes[1, i].imshow(Y_real[i, :, :, 0], cmap="turbo")
        axes[1, i].set_title(f"Real Frame {i+1}")
        axes[1, i].axis("off")

        # Predicted sequence
        axes[2, i].imshow(Y_pred[i, :, :, 0], cmap="turbo")
        axes[2, i].set_title(f"Pred Frame {i+1}")
        axes[2, i].axis("off")

    plt.suptitle("Input vs Real vs Predicted Lightning Frames")
    plt.show()

def compute_saliency_maps(model, X_input, Y_pred):
    """
    Computes saliency maps using TensorFlow's GradientTape.
    """
    X_input_tf = tf.convert_to_tensor(X_input, dtype=tf.float32)

    with tf.GradientTape() as tape:
        tape.watch(X_input_tf)
        Y_pred_tf = model(X_input_tf)

    gradients = tape.gradient(Y_pred_tf, X_input_tf)

    return np.abs(gradients.numpy())