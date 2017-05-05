import utm
import sys

if len(sys.argv) != 3:
    sys.exit('ERROR: invalid arguments. Usage: ' + sys.argv[0] + ' lat lon')

lat = float(sys.argv[1])
lon = float(sys.argv[2])

print utm.from_latlon(lat, lon)
print 'Done!'