# Implementation of a simple U-Net model for "vil" video frame prediction.

import torch
import torch.nn as nn

class UNet(nn.Module):
    def __init__(self, input_channels=12, output_channels=12):
        super(UNet, self).__init__()

        # Encoding path
        self.enc_conv1 = nn.Conv2d(input_channels, 64, kernel_size=3, padding=1)
        self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2)

        self.enc_conv2 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2)

        self.enc_conv3 = nn.Conv2d(128, 256, kernel_size=3, padding=1)
        self.pool3 = nn.MaxPool2d(kernel_size=2, stride=2)

        # Bottleneck
        self.bottleneck = nn.Conv2d(256, 512, kernel_size=3, padding=1)

        # Decoding path using ConvTranspose2d instead of Upsample
        self.up1 = nn.ConvTranspose2d(512, 256, kernel_size=2, stride=2)  # Transposed conv instead of bilinear upsample
        self.dec_conv1 = nn.Conv2d(512, 256, kernel_size=3, padding=1)  # 512 comes from skip connection concat

        self.up2 = nn.ConvTranspose2d(256, 128, kernel_size=2, stride=2)
        self.dec_conv2 = nn.Conv2d(256, 128, kernel_size=3, padding=1)

        self.up3 = nn.ConvTranspose2d(128, 64, kernel_size=2, stride=2)
        self.dec_conv3 = nn.Conv2d(128, 64, kernel_size=3, padding=1)

        # Output layer
        self.output_layer = nn.Conv2d(64, output_channels, kernel_size=1)  # Linear activation (no ReLU)

    def forward(self, x):
        # Encoding path
        enc1 = torch.relu(self.enc_conv1(x))
        pool1 = self.pool1(enc1)

        enc2 = torch.relu(self.enc_conv2(pool1))
        pool2 = self.pool2(enc2)

        enc3 = torch.relu(self.enc_conv3(pool2))
        pool3 = self.pool3(enc3)

        # Bottleneck
        bottleneck = torch.relu(self.bottleneck(pool3))

        # Decoding path using transposed convolutions
        up1 = self.up1(bottleneck)
        concat1 = torch.cat([up1, enc3], dim=1)
        dec1 = torch.relu(self.dec_conv1(concat1))

        up2 = self.up2(dec1)
        concat2 = torch.cat([up2, enc2], dim=1)
        dec2 = torch.relu(self.dec_conv2(concat2))

        up3 = self.up3(dec2)
        concat3 = torch.cat([up3, enc1], dim=1)
        dec3 = torch.relu(self.dec_conv3(concat3))

        # Output layer
        outputs = self.output_layer(dec3)
        return outputs


# Instantiate the model
input_channels = 12  # 12 frames
output_channels = 12  # Predicting 12 output frames
model = UNet(input_channels=input_channels, output_channels=output_channels)

# Example input tensor
batch_size = 8
temporal_frames = 12
features_per_frame = 4  # Assuming 4 features per frame
height, width = 384, 384

# Creating input in (batch, channels, height, width) format
example_input = torch.randn(batch_size, temporal_frames, height, width)

# Forward pass
output = model(example_input)
print("Output shape:", output.shape)  # Expected: (8, 12, 384, 384)