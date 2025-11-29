# This file contains the model definition for the Conv3DLSTM model.

import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv3D, Conv3DTranspose, ConvLSTM2D, Dropout

# Define input shape (batch, depth, height, width, channels)
input_shape = (12, 384, 384, 1)

model_conv3DLSTM_v6 = Sequential([

    Conv3D(16, (5, 5, 5), padding='same', activation='relu', strides=(1, 1, 1), input_shape=input_shape),

    Conv3D(32, (5, 5, 5), padding='same', activation='relu', strides=(1, 2, 2)),

    Conv3D(64, (3, 3, 3), padding='same', activation='relu', strides=(1, 2, 2)),

    Dropout(0.3),

    ConvLSTM2D(128, (3, 3), padding="same", return_sequences=True, activation="relu"),

    Dropout(0.3),

    ConvLSTM2D(64, (3, 3), padding="same", return_sequences=True, activation="relu"),

    Dropout(0.3),

    ConvLSTM2D(32, (3, 3), padding="same", return_sequences=True, activation="relu"),

    Dropout(0.2),

    ConvLSTM2D(32, (3, 3), padding="same", return_sequences=True, activation="relu"),  # Additional ConvLSTM layer

    Dropout(0.2),

    Conv3DTranspose(64, (3, 3, 3), strides=(1, 2, 2), padding='same', activation='relu'),  # Restore spatial dimensions
    Dropout(0.2),

    Conv3DTranspose(32, (3, 3, 3), strides=(1, 1, 1), padding='same', activation='relu'),  # Keep spatial features
    Dropout(0.2),

    Conv3DTranspose(16, (3, 3, 3), strides=(1, 2, 2), padding='same', activation='relu'),  # Restore full spatial resolution

    Dropout(0.2),

    Conv3D(1, (1, 1, 1), padding='same', activation='linear')
])