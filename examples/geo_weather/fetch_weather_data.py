#!/usr/bin/env python

import csv
import json
import requests
import numpy as np
import sqlite3

csv_filename = "testdata/vic_usgs/south_west_stations.csv"
db_filename = "testdata/vic_usgs/south_west_stations.db"

# open database
conn = sqlite3.connect(db_filename)
c = conn.cursor()

# Create stations table
c.execute('''CREATE TABLE IF NOT EXISTS stations 
             (id text, short text, name text, lon real, lat real, height real, json text, UNIQUE(id))''')

# Create observations table
c.execute('''CREATE TABLE IF NOT EXISTS observations 
             (id text, local_date_time text, local_date_time_full text,  
              apparent_t real, delta_t real, air_temp real, rain_trace text, rel_hum real, wind_dir text, wind_spd_kmh real,
              UNIQUE(id, local_date_time_full))''')

# Create time point table
c.execute('''CREATE TABLE IF NOT EXISTS time 
             (id text, local_date_time_full text, UNIQUE(local_date_time_full))''')

# stations
with open(csv_filename, 'rb') as csvfile:
    stations = csv.DictReader(csvfile)
    for station in stations:
        id = station["ID"]
        short = station["Short"]
        name = station["Name"]
        lon = float(station["Lon"])
        lat = float(station["Lat"])
        height = float(station["Height"])
        json = station["Json"]

        # Insert a row of data into stations
        query_str = "INSERT OR IGNORE INTO stations VALUES ('%s', '%s', '%s', %f, %f, %f, '%s')" % (id, short, name, lon, lat, height, json)
        print query_str
        c.execute( query_str )

        # Update observations table
        r = requests.get(json)
        stationdata = r.json()
        data = stationdata["observations"]["data"]
        nrows = len(data)
        for i in range(nrows):
            apparent_t = data[i]['apparent_t']
            if apparent_t is None:
                apparent_t = 0

            delta_t = data[i]['delta_t']
            if delta_t is None:
                delta_t = 0

            air_temp = data[i]['air_temp']
            if air_temp is None:
                air_temp = 0

            rain_trace = data[i]['rain_trace']
            if rain_trace is None:
                rain_trace = ''

            rel_hum = data[i]['rel_hum']
            if rel_hum is None:
                rel_hum = 0

            wind_spd_kmh = data[i]['wind_spd_kmh']
            if wind_spd_kmh is None:
                wind_spd_kmh = 0

            query_str = "INSERT OR IGNORE INTO observations VALUES ('%s', '%s', '%s', %0.2f, %0.2f, %0.2f, '%s', %0.2f, '%s', %0.2f)" % \
                (id, data[i]['local_date_time'], data[i]['local_date_time_full'], apparent_t, delta_t, \
                    air_temp, rain_trace, rel_hum, data[i]['wind_dir'], wind_spd_kmh)
            print query_str
            c.execute(query_str)

            query_str = "INSERT OR IGNORE INTO time VALUES (%s, '%s')" % (id, data[i]['local_date_time_full'])
            print query_str
            c.execute(query_str)

# commit the change
conn.commit()
# close database
conn.close()




# TESTING: print out to check
"""
conn = sqlite3.connect(db_filename)
c = conn.cursor()
c.execute("SELECT name FROM sqlite_master WHERE type='table';")
tables = c.fetchall()
print tables
for table_name in tables:
    table_name = table_name[0]
    table = pd.read_sql_query("SELECT * from %s" % table_name, conn)
    #print table
    table.to_csv('testdata/vic_usgs/south_west_stations_' + table_name + '.csv', index_label='index')
# close database
conn.close()
"""