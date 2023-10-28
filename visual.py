import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

import numpy as np

df = pd.read_csv('cluster_data.csv')

print(df)

data = df.transpose()  # Cluster_0부터 시작하도록 조정

# Heatmap 그리기
plt.figure(figsize=(12, 6))  # 그림 크기 조정
sns.heatmap(data, cmap="YlGnBu")  # heatmap 그리기

plt.xlabel("I/O Amount of processed (GB)")
plt.ylabel("I/O Rewrite Interval (system time - current time)")
plt.title("Cluster Data Heatmap")  # 제목 추가


        

plt.show()

plt.savefig('graph.png', format='png')