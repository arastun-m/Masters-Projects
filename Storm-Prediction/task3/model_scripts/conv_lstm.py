"""
Contains implementatino of the Convolutional Long Short Term Memory (conv-LSTM) model.
Given its sequential processing nature, conv-LSTM model processes samples frame by frame.

"""


import torch
import torch.nn as nn


__all__ = ['ConvLSTM']


class ConvLSTMCell(nn.Module):
    """
    Implementation of a Convolutional LSTM cell.
    """

    def __init__(self, input_channels, hidden_channels, kernel_size, padding):
        super(ConvLSTMCell, self).__init__()
        self.hidden_channels = hidden_channels

        self.conv = nn.Conv2d(
            in_channels=input_channels + hidden_channels,
            out_channels=4 * hidden_channels,
            kernel_size=kernel_size,
            padding=padding
        )

    def forward(self, x, hidden):
        h_prev, c_prev = hidden  # Previous hidden and cell state

        combined = torch.cat([x, h_prev], dim=1)  # Concatenate along channel dimension
        conv_output = self.conv(combined)

        i, f, g, o = torch.chunk(conv_output, chunks=4, dim=1)  # Split into 4 gates
        i = torch.sigmoid(i)  # Input gate
        f = torch.sigmoid(f)  # Forget gate
        g = torch.tanh(g)      # Cell gate
        o = torch.sigmoid(o)  # Output gate

        c_next = f * c_prev + i * g  # New cell state
        h_next = o * torch.tanh(c_next)  # New hidden state

        return h_next, c_next

class ConvLSTM(nn.Module):
    """
    Implementation of a Convolutional LSTM model.
    Makes use of the ConvLSTMCell to process samples frame by frame.

    print(X.shape)  # (36, 4, 384, 384)
    print(y.shape)  # (36, 1, 384, 384)
    Input shape: (batch_size, 4, 384, 384)
    Output shape: (batch_size, 1, 384, 384)
    Where batch size is set to 36 (number of frames in an event)
    """
    def __init__(self, input_channels, hidden_channels, kernel_size, num_layers, padding=1):
        super(ConvLSTM, self).__init__()
        self.num_layers = num_layers

        self.layers = nn.ModuleList([
            ConvLSTMCell(
                input_channels=input_channels if i == 0 else hidden_channels,
                hidden_channels=hidden_channels,
                kernel_size=kernel_size,
                padding=padding
            ) for i in range(num_layers)
        ])

        self.final_conv = nn.Conv2d(hidden_channels, 1, kernel_size=1)

    def forward(self, x, hidden=None):
        batch_size, seq_len, _, height, width = x.shape

        if hidden is None:
            hidden = [(
                torch.zeros(batch_size, self.layers[i].hidden_channels, height, width, device=x.device),
                torch.zeros(batch_size, self.layers[i].hidden_channels, height, width, device=x.device)
            ) for i in range(self.num_layers)]

        outputs = []
        for t in range(seq_len):
            x_t = x[:, t, :, :, :]
            for i, layer in enumerate(self.layers):
                hidden[i] = layer(x_t, hidden[i])
                x_t = hidden[i][0]

            x_t = self.final_conv(x_t)
            outputs.append(x_t.unsqueeze(1))

        return torch.cat(outputs, dim=1), hidden
