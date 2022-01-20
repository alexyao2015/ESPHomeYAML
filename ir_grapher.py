from datetime import datetime
import numpy as np
import matplotlib.pyplot as plt

TICK_INTERVAL = 250
PLOT_DPI = 100

with open("data.txt", mode="r") as fd:
    raw_data = fd.read()

print(f"Obtained raw data: {raw_data}")

raw_data = raw_data.split("\n")
raw_data = [raw.split("Received Raw: ")[1].rstrip("\x1b").split(", ") for raw in raw_data if raw != ""]
for raw in raw_data:
    for idx, raw_str in enumerate(raw):
        raw[idx] = int(raw_str)

data = raw_data
print(f"Cleaned data: {data}")
subplots = len(data)
print(f"Creating {subplots} subplots")

fig, axs = plt.subplots(subplots, sharex=True, sharey=True, figsize=(100, 20))
data_plot = {}
max_x_value = 0

for idx, points in enumerate(data):
    data_x = [0]
    for point in points:
        data_x.append(data_x[-1] + abs(point))
    data_y = [1]
    for point in points:
        data_y.append(1 if point > 0 else -1)
    data_plot[idx] = dict(data_x=data_x, data_y=data_y)
    max_data_x = max(data_x)
    if max_x_value < max_data_x:
        max_x_value = max_data_x

for plot_idx, data_points in data_plot.items():
    axs[plot_idx].step(data_points["data_x"], data_points["data_y"])
    axs[plot_idx].set_xlim(0, max_x_value + 10)
    axs[plot_idx].set_ylim(-1.5, 1.5)
    axs[plot_idx].set_xticks(np.arange(0, max_x_value, TICK_INTERVAL))
    print(f"Created {plot_idx + 1} of {subplots} subplots")
current_time = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
print("Saving plot...")
fig.savefig(f'plot-{current_time}.jpg', dpi=PLOT_DPI)
