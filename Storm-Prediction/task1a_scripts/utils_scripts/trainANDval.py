
# This file contains utility functions for training and validation of the model.

import os
import matplotlib.pyplot as plt
from tqdm.keras import TqdmCallback
from google.colab import drive
from utils.losses import mse_ssim_loss

def mount_drive_and_set_save_path():
    """Mounts Google Drive and sets up the save path.    
    """
    drive.mount('/content/drive')
    save_path = "/content/drive/My Drive/models/"
    os.makedirs(save_path, exist_ok=True)  # Create folder if it doesn't exist
    return save_path

def compute_steps(train_ids, val_ids, batch_size):
    """Computes steps per epoch for training and validation."""
    train_steps = len(train_ids) // batch_size
    val_steps = len(val_ids) // batch_size
    return train_steps, val_steps

def train_model(model, train_generator, val_generator, epochs, train_steps, val_steps):
    """Trains the model with tqdm progress bar and returns training history."""

    model.compile(optimizer="adam", loss=mse_ssim_loss(alpha=0.7, beta=0.3), metrics=["mae"])

    history = model.fit(
        train_generator,
        validation_data=val_generator,
        epochs=epochs,
        steps_per_epoch=train_steps,
        validation_steps=val_steps,
        callbacks=[TqdmCallback()]  # Add tqdm progress bar
    )
    return history

def save_model(model, save_path, model_name="conv3DLSTM_lightning_prediction.keras"):
    """Saves the trained model to Google Drive."""
    model_save_path = os.path.join(save_path, model_name)
    model.save(model_save_path)
    print(f"Model saved successfully at: {model_save_path}")

def plot_training_history(history):
    """Plots training and validation loss curves."""
    plt.plot(history.history['loss'], label='Train Loss')
    plt.plot(history.history['val_loss'], label='Validation Loss')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.legend()
    plt.title('Training vs Validation Loss')
    plt.show()

def training_pipeline(model, train_generator, val_generator, train_ids, val_ids, epochs, batch_size=8):
    """Runs the entire training pipeline: setup, training, saving, and plotting."""

    # Ensure save path is set
    save_path = mount_drive_and_set_save_path()

    # Compute training steps
    train_steps, val_steps = compute_steps(train_ids, val_ids, batch_size)

    # Train model
    history = train_model(model, train_generator, val_generator, epochs, train_steps, val_steps)

    # Save the trained model
    save_model(model, save_path)

    # Plot training loss history
    plot_training_history(history)
