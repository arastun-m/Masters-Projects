"""
Data visualisation utility functions.

"""

from utility_scripts.data_preprocessing import load_event


import IPython.display
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import cartopy.crs as ccrs
import cartopy.feature as cfeature



__all__ = ['show_gif', 'show_reduced_events_table', 'get_events_shape',
            'plot_lightning_dist', 'plot_event_time_of_day', 'plot_event_season',
            'plot_flashes_USA', 'show_loss_plot']


""" ----------------- Base Auxiliary Functions ----------------- """
def show_gif(filename="visualisations/S778114.gif"):
    """
    Show a GIF in Jupyter notebook.

    :param filename: Path to the GIF file.
    """
    im = IPython.display.Image(filename=filename)
    im.reload()
    IPython.display.display(im)

def show_loss_plot(filename="visualisations/task3_vis/unet_event_loss.png"):
    """
    Show a loss plot in Jupyter notebook.

    :param filename: Path to the loss plot image file.
    """
    im = IPython.display.Image(filename=filename)
    im.reload()
    IPython.display.display(im)

def get_event_ids():
    df = pd.read_csv("data/events.csv", parse_dates=["start_utc"])
    ids = df.id.unique()
    return ids

def get_events_shape():
    ids = get_event_ids()
    event = load_event(ids[0])
    for img_type in event:
        print(f"{img_type}: {event[img_type].shape} ({event[img_type].dtype})")

def get_no_strikes():
    ids = get_event_ids()
    no_strikes = []
    for id in ids:
        event = load_event(id)
        no_strikes.append(event["lght"].shape[0])
    return no_strikes

def get_reduced_events_table(filename="data/events.csv"):
    df = pd.read_csv(filename, parse_dates=["start_utc"])
    df_reduced = df.copy()
    df_reduced = df_reduced.drop(columns=["img_type", "proj", 'llcrnrlon', 
                                        'llcrnrlat', 'urcrnrlon', 'urcrnrlat', 
                                        "height_m", "width_m"])
    df_reduced['center_lon'] = (df['llcrnrlon'] + df['urcrnrlon']) / 2
    df_reduced['center_lat'] = (df['llcrnrlat'] + df['urcrnrlat']) / 2
    df_reduced = df_reduced.drop_duplicates(subset=["id"])
    return df_reduced

def show_reduced_events_table(filename="data/events.csv"):
    return get_reduced_events_table(filename).head()

def get_df_eda():
    no_strikes = get_no_strikes()
    df_reduced = get_reduced_events_table()
    df_eda = df_reduced.copy()
    df_eda["strikes"] = no_strikes
    df_eda = df_eda.reset_index(drop=True)
    df_eda.head(10)
    return df_eda



""" ----------------- Visualisation Functions ----------------- """
def plot_lightning_dist():
    no_strikes = get_no_strikes()
    plt.figure(figsize=(10, 6)) 
    plt.hist(no_strikes, bins=100)
    plt.title("Number of Strikes per Event")
    plt.xlabel("Number of Strikes")
    plt.ylabel("Number of Events")
    plt.xlim(0, np.max(no_strikes))
    plt.show()


def plot_event_time_of_day():
    df_reduced = get_reduced_events_table()
    df_reduced['hour'] = df_reduced['start_utc'].dt.hour
    bins = [0, 6, 12, 18, 24]
    labels = ['00:00-06:00', '06:00-12:00', '12:00-18:00', '18:00-24:00']
    df_reduced['time_interval'] = pd.cut(df_reduced['hour'], bins=bins,
                                          labels=labels, right=False)
    # plot of the histogram of the time of day
    plt.figure(figsize=(10, 6))
    df_reduced['time_interval'].value_counts().sort_index().plot(kind='bar')
    plt.title('Number of Events by Time of Day')
    plt.xlabel('Time Interval')
    plt.ylabel('Number of Events')
    plt.xticks(rotation=45)
    plt.show()

def plot_event_season():
    df_reduced = get_reduced_events_table()
    # plot of histogram of the season
    df_reduced['month'] = df_reduced['start_utc'].dt.month
    season_bins = [0, 3, 6, 9, 12]
    season_labels = ['Winter', 'Spring', 'Summer', 'Fall']
    df_reduced['season'] = pd.cut(df_reduced['month'] % 12, bins=season_bins,
                                   labels=season_labels, right=False, include_lowest=True)

    plt.figure(figsize=(10, 6))
    df_reduced['season'].value_counts().sort_index().plot(kind='bar')
    plt.title('Number of Events by Season')
    plt.xlabel('Season')
    plt.ylabel('Number of Events')
    plt.xticks(rotation=45)
    plt.show()

def plot_flashes_USA():
    df_eda = get_df_eda()
    # Create a plot with Cartopy
    plt.figure(figsize=(12, 8))
    ax = plt.axes(projection=ccrs.PlateCarree())  # Use PlateCarree projection for latitude/longitude
    ax.set_extent([-125, -65, 25, 50], crs=ccrs.PlateCarree())  # Focus on the US region

    # adding map features
    ax.add_feature(cfeature.LAND, color='lightgrey')
    ax.add_feature(cfeature.COASTLINE, linewidth=0.8)
    ax.add_feature(cfeature.BORDERS, linestyle='--', linewidth=0.5)
    ax.add_feature(cfeature.STATES, linestyle=':', linewidth=0.5)
    ax.add_feature(cfeature.LAKES, edgecolor='blue', alpha=0.5)
    ax.add_feature(cfeature.RIVERS, edgecolor='blue', alpha=0.7)
    plt.scatter(df_eda["center_lon"], df_eda["center_lat"], s=2, color="red",
                 transform=ccrs.PlateCarree(), label="Events")
    plt.title("Event Locations")
    plt.legend(loc="upper right", fontsize=10)
    plt.show()


""" ----------------- Model Output Visualisation Functions ----------------- """
""" EVENT PLOTS WITH PREDICTIONS PER FRAME """
def plot_event_w_predictions(event, predicted_lght, frame=0):
    """
    Plots the event with 4 input image channels
    Plots target/label lightning strikes of the event
    Plots predicted/output lightning strikes of the event
    """

    t = event["lght"][:,0]# time of lightning strike (in seconds relative to first frame)
    if len(predicted_lght) != 0: t_pred = predicted_lght[:,0]

    def plot_frame(ti):
        f = (t >= ti*5*60 - 2.5*60) & (t < ti*5*60 + 2.5*60)# find which lightning strikes fall in current frame
        if len(predicted_lght) != 0: f_pred =  (t_pred >= ti*5*60 - 2.5*60) & (t_pred < ti*5*60 + 2.5*60)

        fig,axs = plt.subplots(1,5,figsize=(16,5))
        fig.suptitle(f"Event: {id}, Frame: {ti}, Time: {ti*5} min")
        axs[0].imshow(event["vis"][:,:,ti], vmin=0, vmax=10000, cmap="grey"), axs[0].set_title('Visible')
        axs[1].imshow(event["ir069"][:,:,ti], vmin=-8000, vmax=-1000, cmap="viridis"), axs[1].set_title('Infrared (Water Vapor)')
        axs[2].imshow(event["ir107"][:,:,ti], vmin=-7000, vmax=2000, cmap="inferno"), axs[2].set_title('Infrared (Cloud/Surface Temperature)')
        axs[3].imshow(event["vil"][:,:,ti], vmin=0, vmax=255, cmap="turbo"), axs[3].set_title('Radar')
        axs[3].scatter(event["lght"][f,3], event["lght"][f,4], marker="x", s=30, c="tab:red")
        axs[3].set_xlim(0,384), axs[3].set_ylim(384,0)

        axs[4].imshow(event["vil"][:,:,ti], vmin=0, vmax=255, cmap="turbo"), axs[4].set_title('Radar Predicted Strikes')
        if len(predicted_lght) != 0:
          axs[4].scatter(predicted_lght[f_pred,3], predicted_lght[f_pred,4], marker="x", s=30, c="tab:red")
          axs[4].set_xlim(0,384), axs[3].set_ylim(384,0)
        plt.show()
  
    plot_frame(frame)


""" EVENT PLOT WITH PREDICTIONS PER EVENT (GIF) """
def make_gif(outfile, files, fps=10, loop=0):
    "Helper function for saving GIFs"
    imgs = [PIL.Image.open(file) for file in files]
    imgs[0].save(fp=outfile, format='gif', append_images=imgs[1:],
                 save_all=True, duration=int(1000/fps), loop=loop)
    im = IPython.display.Image(filename=outfile)
    im.reload()
    return im

  
def plot_event_w_predictions_gif(event, predicted_lght, output_gif=False, save_gif=False):
    t = event["lght"][:,0]# time of lightning strike (in seconds relative to first frame)
    if len(predicted_lght) != 0: t_pred = predicted_lght[:,0]

    def plot_frame(ti):
        f = (t >= ti*5*60 - 2.5*60) & (t < ti*5*60 + 2.5*60)# find which lightning strikes fall in current frame
        if len(predicted_lght) != 0: f_pred =  (t_pred >= ti*5*60 - 2.5*60) & (t_pred < ti*5*60 + 2.5*60)

        fig,axs = plt.subplots(1,5,figsize=(16,5))
        fig.suptitle(f"Event: {id}, Frame: {ti}, Time: {ti*5} min")
        axs[0].imshow(event["vis"][:,:,ti], vmin=0, vmax=10000, cmap="grey"), axs[0].set_title('Visible')
        axs[1].imshow(event["ir069"][:,:,ti], vmin=-8000, vmax=-1000, cmap="viridis"), axs[1].set_title('Infrared (Water Vapor)')
        axs[2].imshow(event["ir107"][:,:,ti], vmin=-7000, vmax=2000, cmap="inferno"), axs[2].set_title('Infrared (Cloud/Surface Temperature)')
        axs[3].imshow(event["vil"][:,:,ti], vmin=0, vmax=255, cmap="turbo"), axs[3].set_title('Radar')
        axs[3].scatter(event["lght"][f,3], event["lght"][f,4], marker="x", s=30, c="tab:red")
        axs[3].set_xlim(0,384), axs[3].set_ylim(384,0)

        axs[4].imshow(event["vil"][:,:,ti], vmin=0, vmax=255, cmap="turbo"), axs[4].set_title('Radar Predicted Strikes')
        if len(predicted_lght) != 0:
          axs[4].scatter(predicted_lght[f_pred,3], predicted_lght[f_pred,4], marker="x", s=30, c="tab:red")
          axs[4].set_xlim(0,384), axs[3].set_ylim(384,0)

        if output_gif:
            file = f"_temp_{id}_{ti}.png"
            fig.savefig(file, bbox_inches="tight", dpi=150, pad_inches=0.02, facecolor="white")
            plt.close()
        else:
            plt.show()
  
    if output_gif:
        for ti in range(36): plot_frame(ti)
        im = make_gif(f"{id}.gif", [f"_temp_{id}_{ti}.png" for ti in range(36)])
        for ti in range(36): os.remove(f"_temp_{id}_{ti}.png")
        IPython.display.display(im)
        if not save_gif: os.remove(f"{id}.gif")
    else:
        plot_frame(0)
        plot_frame(17)
        plot_frame(34)
