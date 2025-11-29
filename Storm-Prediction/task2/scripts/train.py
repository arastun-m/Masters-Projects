from tqdm import tqdm
import torch
import torch.optim as optim
from torch.cuda.amp import GradScaler, autocast
from dataset import get_data_loader
from task2.models.model import UNetGenerator, PatchGANDiscriminator
from task2.models.model import generator_loss, discriminator_loss


# AMP training
def train(train_loader, num_epochs, generator, discriminator, optimizer_G, optimizer_D, scheduler_G, scheduler_D):
    scaler = GradScaler()
    best_model = None
    best_loss = None
    for epoch in range(num_epochs):
        with tqdm(train_loader, desc=f"Epoch {epoch+1}/{num_epochs} (Training)", unit="batch") as pbar:
            for i, (x, y) in enumerate(pbar):
                x, y = x.cuda().to(memory_format=torch.channels_last), y.cuda().to(memory_format=torch.channels_last)

                # =========== Update D ============
                optimizer_D.zero_grad()
                with autocast():  # AMP mixed precision
                    pred_real = discriminator(y)
                    fake_y = generator(x).detach()
                    pred_fake = discriminator(fake_y)
                    loss_D = discriminator_loss(pred_real, pred_fake)

                scaler.scale(loss_D).backward()
                scaler.step(optimizer_D)
                scaler.update()

                # =========== Update G ============

                optimizer_G.zero_grad()
                with autocast():
                    fake_y = generator(x)
                    pred_fake = discriminator(fake_y)

                    loss_G = generator_loss(pred_fake, fake_y, y)

                scaler.scale(loss_G).backward()
                scaler.step(optimizer_G)
                scaler.update()
                pbar.set_postfix({'loss_G': loss_G.item(), 'loss_D': loss_D.item()})
            scheduler_G.step()
            scheduler_D.step()

            print(f"Epoch {epoch+1}, Loss_G: {loss_G.item():.4f}, Loss_D: {loss_D.item():.4f}")
            if best_loss is None or loss_G.item() < best_loss:
                best_loss = loss_G.item()
                best_model = generator.state_dict()
                print('Saving best model...')
                torch.save(best_model, f'gan_best_model_epoch{epoch+1}.pth')


if __name__ == '__main__':

    generator = UNetGenerator().cuda()
    discriminator = PatchGANDiscriminator().cuda()

    optimizer_G = optim.Adam(generator.parameters(), lr=2e-4, betas=(0.5, 0.999))
    optimizer_D = optim.Adam(discriminator.parameters(), lr=2e-4, betas=(0.5, 0.999))

    # CosineAnnealingLR
    scheduler_G = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer_G, T_max=50, eta_min=1e-6)
    scheduler_D = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer_D, T_max=50, eta_min=1e-6)

    n_critic = 2  # train D twice more than G
    num_epochs = 50
    train_loader = get_data_loader()
    train(train_loader, num_epochs, generator, discriminator, optimizer_G, optimizer_D, scheduler_G, scheduler_D)
