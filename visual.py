import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

import numpy as np

# Load your DataFrame from the CSV file
df = pd.read_csv('cluster_data.csv')
print(df)
# Create a 1x2 grid of subplots
fig, axes = plt.subplots(1, 2, figsize=(12, 6))

# Extract 'WAF' data and drop it from the DataFrame
data_WAF = df["WAF"]
df = df.drop("WAF", axis=1)

data = df.transpose()  # Transpose the data for the heatmap

# Create a heatmap
sns.heatmap(data, cmap="YlGnBu", ax=axes[0])

# Set labels and title for the heatmap
axes[0].set_xlabel("I/O Amount of processed (GB)")
axes[0].set_ylabel("I/O Rewrite Interval (system time - current time)")
axes[0].set_title("Cluster Data Heatmap")

# Create a line plot
axes[1].plot(data_WAF)

# Set labels for the line plot
axes[1].set_xlabel("X-axis Label")  # Add an appropriate label
axes[1].set_ylabel("Y-axis Label")  # Add an appropriate label

# Tight layout for better visualization
plt.tight_layout()

# Show the plots
plt.show()

# Save the plot to a file
plt.savefig('graph.png', format='png')