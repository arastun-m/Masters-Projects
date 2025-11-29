import torch
import torch.nn as nn
import torch.nn.functional as F


class UNetGenerator(nn.Module):
    def __init__(self, in_channels=3, out_channels=1):
        super(UNetGenerator, self).__init__()

        # encoder
        self.enc1 = self.conv_block(in_channels, 64)
        self.enc2 = self.conv_block(64, 128)
        self.enc3 = self.conv_block(128, 256)

        # Bottleneck
        self.bottleneck = self.conv_block(256, 512)

        # decoder
        self.up3 = self.up_conv(512, 256)
        self.dec3 = self.conv_block(512, 256)
        self.up2 = self.up_conv(256, 128)
        self.dec2 = self.conv_block(256, 128)
        self.up1 = self.up_conv(128, 64)
        self.dec1 = self.conv_block(128, 64)

        # final output
        self.final = nn.Conv2d(64, out_channels, kernel_size=1)

    def conv_block(self, in_channels, out_channels):
        return nn.Sequential(
            nn.Conv2d(in_channels, out_channels, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(out_channels, out_channels, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
        )

    def up_conv(self, in_channels, out_channels):
        return nn.ConvTranspose2d(in_channels, out_channels, kernel_size=2, stride=2)

    def forward(self, x):
        enc1 = self.enc1(x)
        enc2 = self.enc2(F.max_pool2d(enc1, 2))
        enc3 = self.enc3(F.max_pool2d(enc2, 2))

        bottleneck = self.bottleneck(F.max_pool2d(enc3, 2))

        up3 = self.up3(bottleneck)
        dec3 = self.dec3(torch.cat([up3, enc3], dim=1))
        up2 = self.up2(dec3)
        dec2 = self.dec2(torch.cat([up2, enc2], dim=1))
        up1 = self.up1(dec2)
        dec1 = self.dec1(torch.cat([up1, enc1], dim=1))

        return torch.sigmoid(self.final(dec1))


class PatchGANDiscriminator(nn.Module):
    def __init__(self, in_channels=1):
        super(PatchGANDiscriminator, self).__init__()

        def conv_block(in_c, out_c, stride=2):
            return nn.Sequential(
                nn.Conv2d(in_c, out_c, kernel_size=4, stride=stride, padding=1),
                nn.BatchNorm2d(out_c),
                nn.LeakyReLU(0.2, inplace=True)
            )

        self.model = nn.Sequential(
            conv_block(1, 64, stride=2),
            conv_block(64, 128, stride=2),
            conv_block(128, 256, stride=2),
            conv_block(256, 512, stride=1),  # PatchGAN keep the shape as 30x30
            nn.Conv2d(512, 1, kernel_size=4, padding=1)
        )

    def forward(self, x):
        return self.model(x)


criterion_gan = nn.BCEWithLogitsLoss()  # more stable than BCELoss
criterion_l1 = nn.L1Loss()  # L1 Loss


def generator_loss(pred_fake, fake_vil, real_vil, lambda_l1=100):
    gan_loss = criterion_gan(pred_fake, torch.ones_like(pred_fake))
    l1_loss = criterion_l1(fake_vil, real_vil)
    return gan_loss + lambda_l1 * l1_loss


def discriminator_loss(pred_real, pred_fake):
    loss_real = criterion_gan(pred_real, torch.ones_like(pred_real))
    loss_fake = criterion_gan(pred_fake, torch.zeros_like(pred_fake))
    return (loss_real + loss_fake) / 2
