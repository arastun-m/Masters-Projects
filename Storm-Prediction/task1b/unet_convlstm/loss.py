import tensorflow as tf
from tensorflow.keras.losses import Loss

@tf.keras.utils.register_keras_serializable(package="CustomLoss")
class MSE_SSIM_Loss(Loss):
    def __init__(self, alpha=0.9, beta=0.1, **kwargs):
        super().__init__(**kwargs)
        self.alpha = alpha
        self.beta = beta

    def call(self, y_true, y_pred):
        mse = tf.reduce_mean(tf.square(y_true - y_pred))
        ssim = tf.image.ssim(y_true, y_pred, max_val=1.0)
        return self.alpha * mse + self.beta * (1 - tf.reduce_mean(ssim))

    def get_config(self):
        config = super().get_config()
        config.update({"alpha": self.alpha, "beta": self.beta})
        return config
