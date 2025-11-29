"""
Contains the U-NET model for frame prediction.
This U-NET model processes samples frame by frame.

For examples of  data loading and model training refer to references notebooks.
"""


import torch
import torch.nn as nn


__all__ = ['UNet_Frame']


class UNet_Frame(nn.Module):
    """
    Event-by-Event implementation of the U-Net model.

    Input shape: (batch_size, 4, 64, 64)
    Output shape: (batch_size, 1, 384, 384)
    Where batch size is set to 36 (number of frames in an event)
    """

    def __init__(self, input_channels=4, output_channels=1):
        super(UNet_Frame, self).__init__()

        # Encoding path
        self.enc_conv1 = nn.Conv2d(input_channels, 128, kernel_size=3, padding=1)
        self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2)

        self.enc_conv2 = nn.Conv2d(128, 256, kernel_size=3, padding=1)
        self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2)

        # Bottleneck
        self.bottleneck = nn.Conv2d(256, 512, kernel_size=3, padding=1)

        self.up1 = nn.ConvTranspose2d(512, 256, kernel_size=2, stride=2)
        self.dec_conv1 = nn.Conv2d(512, 256,kernel_size=3, padding=1)

        self.up2 = nn.ConvTranspose2d(256, 128, kernel_size=2, stride=2)
        self.dec_conv2 = nn.Conv2d(256, 128, kernel_size=3, padding=1)

        self.up3 = nn.ConvTranspose2d(128, 128, kernel_size=6, stride=6)
        self.dec_conv3 = nn.Conv2d(128, 64, kernel_size=3, padding=1)

        # Output layer
        self.output_layer = nn.Conv2d(64, output_channels, kernel_size=1)  # Linear activation (no ReLU)

    def forward(self, x):
        # Encoding path
        enc1 = torch.relu(self.enc_conv1(x))
        pool1 = self.pool1(enc1)

        enc2 = torch.relu(self.enc_conv2(pool1))
        pool2 = self.pool2(enc2)

        # Bottleneck
        bottleneck = torch.relu(self.bottleneck(pool2))

        # Decoding path using transposed convolutions
        up1 = self.up1(bottleneck)
        concat1 = torch.cat([up1, enc2], dim=1)
        dec1 = torch.relu(self.dec_conv1(concat1))

        up2 = self.up2(dec1)
        concat2 = torch.cat([up2, enc1], dim=1)
        dec2 = torch.relu(self.dec_conv2(concat2))

        up3 = self.up3(dec2)
        dec3 = torch.relu(self.dec_conv3(up3))

        # Output layer (applied sigmoid to ensure output is between 0 and 1)
        outputs = torch.sigmoid(self.output_layer(dec3))
        return outputs
