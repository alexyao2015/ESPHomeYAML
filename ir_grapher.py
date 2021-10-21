from datetime import datetime
import numpy as np
import matplotlib.pyplot as plt

raw_data = """[03:52:11][D][remote.raw:041]: Received Raw: 432, -256, 187, -255, 189, -582, 188, -252, 189, -753, 184, -434, 174, -251, 189, -253, 187, -254, 187, -255, 190, -582, 188, -419, 188, -581, 188, -419, 187, -418, 189, -748, 184, -256, 185
[03:52:11][D][remote.raw:041]: Received Raw: 430, -257, 187, -254, 190, -582, 188, -252, 189, -764, 174, -418, 187, -252, 188, -253, 189, -253, 188, -254, 187, -585, 188, -418, 188, -583, 187, -419, 184, -421, 190, -748, 188, -251, 186
[03:52:11][D][remote.raw:041]: Received Raw: 433, -255, 189, -252, 187, -585, 189, -251, 187, -751, 187, -420, 187, -253, 187, -254, 191, -250, 188, -254, 188, -585, 187, -418, 191, -580, 189, -417, 186, -420, 188, -749, 189, -250, 189
[03:52:11][D][remote.raw:041]: Received Raw: 433, -254, 188, -256, 185, -585, 188, -252, 190, -749, 188, -418, 188, -252, 187, -256, 185, -255, 188, -254, 188, -584, 187, -419, 187, -583, 189, -417, 187, -420, 188, -748, 189, -251, 187
[03:52:11][D][remote.raw:041]: Received Raw: 433, -255, 188, -252, 188, -585, 188, -252, 186, -752, 189, -417, 188, -252, 186, -256, 188, -253, 192, -250, 189, -584, 189, -416, 188, -583, 188, -418, 189, -418, 187, -749, 188, -251, 187
[03:52:11][D][remote.raw:041]: Received Raw: 436, -252, 186, -255, 190, -583, 187, -252, 187, -751, 186, -421, 188, -252, 183, -259, 190, -251, 186, -255, 188, -585, 188, -420, 186, -597, 172, -419, 189, -417, 188, -749, 188, -252, 187
[03:52:11][D][remote.raw:041]: Received Raw: 433, -255, 190, -252, 187, -585, 188, -252, 184, -754, 187, -419, 188, -252, 187, -254, 189, -253, 187, -255, 188, -584, 186, -420, 188, -582, 184, -423, 188, -417, 185, -753, 187, -252, 188
[03:52:11][D][remote.raw:041]: Received Raw: 432, -254, 187, -256, 189, -583, 189, -251, 189, -750, 187, -418, 187, -253, 187, -256, 188, -253, 185, -256, 188, -584, 188, -419, 188, -582, 188, -418, 187, -419, 185, -752, 186, -255, 185
[03:52:12][D][remote.raw:041]: Received Raw: 433, -255, 190, -252, 187, -585, 188, -252, 188, -751, 189, -417, 188, -251, 188, -254, 187, -255, 189, -257, 183, -585, 188, -418, 186, -584, 187, -419, 189, -417, 192, -745, 188, -252, 190
[03:52:12][D][remote.raw:041]: Received Raw: 431, -256, 189, -253, 188, -584, 187, -253, 186, -752, 189, -418, 187, -255, 184, -255, 188, -254, 187, -254, 187, -585, 187, -420, 187, -583, 188, -417, 187, -419, 188, -749, 188, -252, 189
[03:52:12][D][remote.raw:041]: Received Raw: 431, -256, 186, -255, 186, -586, 187, -253, 186, -753, 187, -419, 187, -252, 189, -253, 187, -255, 187, -255, 189, -582, 187, -420, 188, -582, 187, -419, 187, -419, 188, -749, 188, -252, 187"""
raw_data = raw_data.split("\n")
raw_data = [raw.split("Received Raw: ")[1].split(", ") for raw in raw_data]
for raw in raw_data:
    for idx, raw_str in enumerate(raw):
        raw[idx] = int(raw_str)

data = raw_data

TICK_INTERVAL = 250

fig, axs = plt.subplots(len(data), sharex=True, sharey=True, figsize=(100, 20))
data_plot = {}
max_value = 0

for idx, points in enumerate(data):
    data_x = [0]
    for point in points:
        data_x.append(data_x[-1] + abs(point))
    data_y = [1]
    for point in points:
        data_y.append(1 if point > 0 else -1)
    data_plot[idx] = dict(data_x=data_x, data_y=data_y)
    max_data_x = max(data_x)
    if max_value < max_data_x:
        max_value = max_data_x

for plot_idx, data_points in data_plot.items():
    axs[plot_idx].step(data_points["data_x"], data_points["data_y"])
    axs[plot_idx].set_xlim(0, max_value + 10)
    axs[plot_idx].set_ylim(-1.5, 1.5)
    axs[plot_idx].set_xticks(np.arange(min(data_points["data_x"]),max(data_points["data_x"]),TICK_INTERVAL))
current_time = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
fig.savefig(f'plot-{current_time}.jpg', dpi=300)
