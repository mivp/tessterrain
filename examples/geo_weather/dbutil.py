import sqlite3

class DBUtil:
    def __init__(self, db_filename):
        self.db_filename = db_filename

    def getStations(self):
        conn = sqlite3.connect(self.db_filename)
        c = conn.cursor()
        stations = []
        data = c.execute("SELECT * FROM stations;")
        for row in data:
            stations.append(row)
        conn.close()
        return stations

    def getData(self, time):
        conn = sqlite3.connect(self.db_filename)
        c = conn.cursor()
        stations = []
        data = c.execute("SELECT * FROM observations WHERE local_date_time_full=" + time + ";")
        for row in data:
            stations.append(row)
        conn.close()
        return stations