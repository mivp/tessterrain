#!/usr/bin/env python

import json
import requests
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import six
import numpy as np
#import Image

url = "http://www.bom.gov.au/fwo/IDV60801/IDV60801.94835.json"

r = requests.get(url)
stationdata = r.json()

data = stationdata["observations"]["data"]
print data

"""
fig,ax = plt.subplots()
rows = ['1']
columns = ['1','2']
cell_text = [['a','b']]
table = ax.table(cellText = cell_text, rowLabels = rows, colLabels = columns, loc='top')
plt.subplots_adjust(left=0.2, top=0.8)
plt.show()
"""

"""
fig, ax = plt.subplots()

# Hide axes
ax.xaxis.set_visible(False)
ax.yaxis.set_visible(False)
ax.axis('off')

# Table
nrows = len(data)
if nrows > 5:
    nrows = 5

collabel=("temp", "wind", "hum")
rowlabel = []
for i in range(nrows):
    print data[i]["local_date_time"]
    rowlabel.append(data[i]["local_date_time"])

clust_data = np.random.random((5,3))

ax.table(cellText=clust_data,colLabels=collabel,rowLabels=rowlabel, loc='center')
#plt.show()
plt.savefig('testplot.png')
#Image.open('testplot.png').save('testplot.jpg','JPEG')
"""

nrows = len(data)
if nrows > 5:
    nrows = 5


time = []
temp = []
wind = []
hum = []
for i in range(nrows):
    time.append(data[i]["local_date_time"])
    temp.append(data[i]["air_temp"])
    wind.append(data[i]["wind_spd_kmh"])
    hum.append(data[i]["rel_hum"])

df = pd.DataFrame()
df['time'] = time
df['temp'] = temp
df['wind'] = wind
df['hum'] = hum

def render_mpl_table(data, col_width=3.0, row_height=0.625, font_size=14,
                     header_color='#40466e', row_colors=['#f1f1f2', 'w'], edge_color='w',
                     bbox=[0, 0, 1, 1], header_columns=0,
                     ax=None, **kwargs):
    if ax is None:
        size = (np.array(data.shape[::-1]) + np.array([0, 1])) * np.array([col_width, row_height])
        fig, ax = plt.subplots(figsize=size)
        ax.axis('off')

    mpl_table = ax.table(cellText=data.values, bbox=bbox, colLabels=data.columns, **kwargs)

    mpl_table.auto_set_font_size(False)
    mpl_table.set_fontsize(font_size)

    for k, cell in six.iteritems(mpl_table._cells):
        cell.set_edgecolor(edge_color)
        if k[0] == 0 or k[1] < header_columns:
            cell.set_text_props(weight='bold', color='w')
            cell.set_facecolor(header_color)
        else:
            cell.set_facecolor(row_colors[k[0]%len(row_colors) ])
    return ax


ax = render_mpl_table(df, header_columns=0, col_width=2.0)
fig = ax.get_figure()
fig.savefig('testplot.png')