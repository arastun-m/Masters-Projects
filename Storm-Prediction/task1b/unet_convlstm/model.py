import tensorflow as tf
from tensorflow.keras.models import Model
from tensorflow.keras.layers import (
    Input, ConvLSTM2D, Conv3D, Conv2D, UpSampling2D, TimeDistributed,
    LeakyReLU, Concatenate, MaxPooling2D, Multiply, Lambda, Dropout
)
from tensorflow.keras.optimizers import Adam
from .loss import MSE_SSIM_Loss

@tf.keras.utils.register_keras_serializable(package="CustomLayers")
def attention(x):
    """ Temporal attention mechanism that assigns higher weight to later frames. """
    batch_size = tf.shape(x)[0]
    t_weight = tf.range(1, 13, dtype=tf.float32) / 12.0  # Generate weights for 12 frames
    t_weight = tf.reshape(t_weight, (1, 12, 1, 1, 1))
    t_weight = tf.tile(t_weight, [batch_size, 1, 1, 1, x.shape[-1]])  # Expand along batch and channel dimensions
    return Multiply()([x, t_weight])

def temporal_attention_layer():
    """ Applies the temporal attention mechanism using a Lambda layer. """
    return Lambda(attention, name="temporal_attention")

@tf.keras.utils.register_keras_serializable(package="CustomLayers")
class RecursiveLSTMLayer(tf.keras.layers.Layer):
    """ Recursive ConvLSTM2D layer to predict future frames (t=13 to t=24). """
    def __init__(self, units, output_length, **kwargs):
        super().__init__(**kwargs)
        self.units = units
        self.output_length = output_length
        self.lstm = ConvLSTM2D(units, (3, 3), activation=None, padding='same', return_sequences=True)

    def call(self, inputs):
        outputs = []
        x = inputs[:, -1:, :, :, :]  # Use only the last frame (t=12) as the initial input
        for _ in range(self.output_length):
            x = self.lstm(x)  # Recursively feed the predicted frame back into LSTM
            outputs.append(x)

        return tf.concat(outputs, axis=1)  # Concatenate all predicted frames along the time axis

def unet_convlstm_recursive(input_shape=(12, 384, 384, 4), output_length=12):
    """ U-Net + ConvLSTM2D model for frame prediction. """
    inputs = Input(shape=input_shape, name="input_layer")

    # ** Encoding Path**
    conv1 = TimeDistributed(Conv2D(16, (3, 3), activation=None, padding='same'))(inputs)
    conv1 = LeakyReLU(negative_slope=0.1)(conv1)
    pool1 = TimeDistributed(MaxPooling2D(pool_size=(2, 2)))(conv1)

    # ** Bottleneck**
    convlstm = ConvLSTM2D(32, (3, 3), activation=None, padding='same', return_sequences=True)(pool1)
    convlstm = Dropout(0.3)(convlstm)
    bottleneck = ConvLSTM2D(64, (3, 3), activation=None, padding='same', return_sequences=True)(convlstm)
    bottleneck = Dropout(0.3)(bottleneck)

    # ** Temporal Attention**
    bottleneck = temporal_attention_layer()(bottleneck)

    # ** Conv3D for Feature Extraction**
    conv3d = Conv3D(64, (3, 3, 3), activation=None, padding='same')(bottleneck)
    conv3d = Conv3D(64, (3, 3, 3), activation=None, padding='same')(conv3d)
    conv3d = LeakyReLU(negative_slope=0.1)(conv3d)

    # ** Recursive Prediction for t=13 to t=24**
    future_preds = RecursiveLSTMLayer(64, output_length)(conv3d)

    # ** Decoding Path**
    conv_restore = TimeDistributed(Conv2D(32, (3, 3), activation=None, padding='same'))(future_preds)
    conv_restore = LeakyReLU(negative_slope=0.1)(conv_restore)

    up1 = TimeDistributed(UpSampling2D(size=(2, 2)))(conv_restore)
    concat1 = Concatenate()([up1, conv1])

    outputs = TimeDistributed(Conv2D(1, (1, 1), activation="linear", padding='same'))(concat1)

    # ** Define and Compile the Model**
    model = Model(inputs=inputs, outputs=outputs)
    model.compile(
        optimizer=Adam(learning_rate=1e-3),
        loss=MSE_SSIM_Loss(alpha=0.9, beta=0.1),  # Custom loss function
        metrics=['mae']
    )
    return model
