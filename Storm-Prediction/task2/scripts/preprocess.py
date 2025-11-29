from huggingface_hub import hf_hub_download
import pandas as pd
import h5py
import torch
import torch.nn.functional as F
import numpy as np
from sklearn.model_selection import train_test_split


def load_event(id):
    # this function loads all of the image arrays for a given id
    "Load event"
    with h5py.File('data/train.h5', 'r') as f:
        event = {img_type: f[id][img_type][:] for img_type in ['vis', 'ir069', 'ir107', 'vil', 'lght']}
    return event


# devide region
def assign_region(row):
    if row['llcrnrlat'] > 40:  # latitude divide
        return 'North'
    elif row['llcrnrlat'] < 30:
        return 'South'
    elif row['llcrnrlon'] < -100:  # longitude divide
        return 'West'
    else:
        return 'East'
    
        
def strafied_sampling():

    # this DataFrame contains all of the event meta data
    df = pd.read_csv("data/events.csv", parse_dates=["start_utc"])

    # delete duplicates
    events = df.drop_duplicates(subset=['id']).copy()

    #  time devide
    events['start_date'] = pd.to_datetime(events['start_utc']).dt.date
    events['month'] = pd.to_datetime(events['start_utc']).dt.month
    events['season'] = events['month'].map({
        12: 'Winter', 1: 'Winter', 2: 'Winter',
        3: 'Spring', 4: 'Spring', 5: 'Spring',
        6: 'Summer', 7: 'Summer', 8: 'Summer',
        9: 'Fall', 10: 'Fall', 11: 'Fall'
    })
    
    events['region'] = events.apply(assign_region, axis=1)

    # split train and validation set using stratified sampling
    train_events, val_events = train_test_split(
        events,
        test_size=0.2,  # validation set size
        stratify=events[['season', 'region']],  # stratified sampling based on season and region
        random_state=42
    )

    train_ids = train_events['id']
    val_ids = val_events['id']
    print(f"Train Set Size: {len(train_ids)}")
    print(f"Validation Set Size: {len(val_ids)}")
    return train_events, val_events


def process_data(event_ids):

    # prepare the data tensor for the model input and target
    N = len(event_ids)
    H, W = 384, 384
    input_channels = 3  
    target_channels = 1 
    frames = 36
    # directly allocate tensor to avoid list append
    data_inputs = torch.empty((N, frames, input_channels, H, W), dtype=torch.float32)
    data_targets = torch.empty((N, frames, target_channels, H, W), dtype=torch.float32)

    for i, id in enumerate(event_ids):
        event = load_event(id)

        # read data and convert to Tensor
        vis = torch.from_numpy(event['vis'].astype(np.float32)).permute(2, 0, 1).unsqueeze(0)
        ir069 = torch.from_numpy(event['ir069'].astype(np.float32)).permute(2, 0, 1).unsqueeze(0)
        ir107 = torch.from_numpy(event['ir107'].astype(np.float32)).permute(2, 0, 1).unsqueeze(0)
        vil = torch.from_numpy(event['vil'].astype(np.float32)).permute(2, 0, 1).unsqueeze(1)  # (36, 384, 384)

        # upsample infrared data to 384x384
        ir069 = F.interpolate(ir069, size=(H, W), mode='bilinear', align_corners=False)
        ir107 = F.interpolate(ir107, size=(H, W), mode='bilinear', align_corners=False)

        # Normalize the input data, min-max normalization
        def normalize(tensor):
            min_val = tensor.min(dim=1, keepdim=True)[0].min(dim=2, keepdim=True)[0]  # only calculate min once
            max_val = tensor.max(dim=1, keepdim=True)[0].max(dim=2, keepdim=True)[0]  # only calculate max once
            return (tensor - min_val) / (max_val - min_val + 1e-8)  # advoid division by zero

        vis = normalize(vis)
        ir069 = normalize(ir069)
        ir107 = normalize(ir107)
        vil = normalize(vil)

        # concat along the channel dimension (3, 36, 384, 384) and permute to (36, 3, 384, 384)
        inputs = torch.cat([vis, ir069, ir107], dim=0).permute(1, 0, 2, 3)  # (36, 3, 384, 384)

        # store the data in the pre-allocated tensor
        data_inputs[i] = inputs
        data_targets[i] = vil

        if (i + 1) % 10 == 0:
            print(f"Processed {i+1}/{N}")
    
    # reshape the data to (N*frames, C, H, W)
    data_inputs = data_inputs.view(N * frames, input_channels, 384, 384)
    data_targets = data_targets.view(N * frames, target_channels, 384, 384)
    return data_inputs, data_targets


if __name__ == '__main__':
    hf_hub_download(repo_id="benmoseley/ese-dl-2024-25-group-project", filename="train.h5", repo_type="dataset", local_dir="data")
    hf_hub_download(repo_id="benmoseley/ese-dl-2024-25-group-project", filename="events.csv", repo_type="dataset", local_dir="data")
    train_ids, val_ids = strafied_sampling()
    train_inputs, train_targets = process_data(train_ids)
    val_inputs, val_targets = process_data(val_ids)

    # check the final shape
    print("Final Input Shape:", train_inputs.shape, "Final Target Shape:", train_targets.shape)
    print("Final Input Shape:", val_inputs.shape, "Final Target Shape:", val_targets.shape)