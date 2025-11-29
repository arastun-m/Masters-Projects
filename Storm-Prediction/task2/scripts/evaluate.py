import os
import torch
import matplotlib.pyplot as plt
from task2.models.model import UNetGenerator
import PIL
import IPython.display
from dataset import get_data_loader


def evaluate(model, val_loader, state_dict_path):

    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model.load_state_dict(state_dict_path)
    model.eval()

    # fetch a batch from the validation loader
    with torch.no_grad():
        count = 0
        for batch in val_loader:

            if count == 2:
                inputs = batch[0].to(device)   # (1, 3, 384, 384)
                target = batch[1].to(device)   # (1, 1, 384, 384)
                break
            count += 1
        prediction = model(inputs)
        target = target.cpu().numpy().squeeze(1)
        prediction = prediction.cpu().numpy().squeeze(1)

    vis = inputs[:, 0, :, :].cpu().numpy()
    ir069 = inputs[:, 0, :, :].cpu().numpy()
    ir107 = inputs[:, 1, :, :].cpu().numpy()
    return vis, ir069, ir107, target, prediction


def make_gif(outfile, files, fps=4, loop=0):
    "Helper function for saving GIFs"
    imgs = [PIL.Image.open(file) for file in files]
    imgs[0].save(fp=outfile, format='gif', append_images=imgs[1:],
                 save_all=True, duration=int(1000/fps), loop=loop)
    im = IPython.display.Image(filename=outfile)
    im.reload()
    return im


def plot_event(vis, ir069, ir107, target, prediction, output_gif=False, save_gif=False, frames=36):

    def plot_frame(ti):
        fig, axs = plt.subplots(1, 5, figsize=(20, 10))
        fig.suptitle(f"Frame: {ti}, Time: {ti*5} min")
        axs[0].imshow(vis[ti], cmap="grey"), axs[0].set_title('vis')
        axs[1].imshow(ir069[ti], cmap="viridis"), axs[1].set_title('ir069')
        axs[2].imshow(ir107[ti], cmap="inferno"), axs[2].set_title('ir107')
        im3 = axs[3].imshow(target[ti], cmap="turbo")
        axs[3].set_title('target')
        fig.colorbar(im3, ax=axs[3], fraction=0.046, pad=0.04)

        im4 = axs[4].imshow(prediction[ti], cmap="turbo")
        axs[4].set_title('prediction')
        fig.colorbar(im4, ax=axs[4], fraction=0.046, pad=0.04)

        axs[4].set_xlim(0, 384), axs[1].set_ylim(384, 0)

        if output_gif:
            file = f"_temp_{id}_{ti}.png"
            fig.savefig(file, bbox_inches="tight", dpi=150, pad_inches=0.02, facecolor="white")
            plt.close()
        else:
            plt.show()
    if output_gif:
        for ti in range(frames):
            plot_frame(ti)
        im = make_gif(f"{id}.gif", [f"_temp_{id}_{ti}.png" for ti in range(frames)])
        for ti in range(frames):
            os.remove(f"_temp_{id}_{ti}.png")
        IPython.display.display(im)
        if not save_gif:
            os.remove(f"{id}.gif")
    else:
        plot_frame(0)
        plot_frame(17)
        plot_frame(34)


if __name__ == '__main__':
    model = UNetGenerator().cuda()
    train_loader, val_loader = get_data_loader()
    state_dict_path = "gan_best_model_epoch20.pth"
    vis, ir069, ir107, target, prediction = evaluate(model, val_loader, state_dict_path)
    plot_event(output_gif=True, save_gif=True, frames=36)
