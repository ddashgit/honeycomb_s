import sys
import re
import pandas as pd
import numpy as np

interval_log = str ( sys.argv[1])
current_file_path = str ( sys.argv[2])
raw_timeline_path = str ( sys.argv[3])

fp = open ( interval_log, 'r')
for line in fp:
	m = re.match ( '(\S+)\s+(\S+)', line)
	if m != None:
		start_time = float ( m.group ( 1))
		end_time = float ( m.group ( 2))
fp.close ()

df = pd.read_csv ( current_file_path, delimiter=':', header=None, names=['Time','Current','Voltage','Energy'])
times = df.loc [ ( df['Time'] >= start_time) & ( df['Time'] <= end_time)]['Time'].to_numpy()
currents = df.loc [ ( df['Time'] >= start_time) & ( df['Time'] <= end_time)]['Current'].to_numpy()

fp = open ( raw_timeline_path, 'w')
fp.write ( ">>> Time %f %f\n" % ( start_time, end_time))
fp.write ( ">> Power Monitor\n")
for index in range ( len ( times)-1):
	start_time = times[index]
	end_time = times[index+1]
	avg_current = -1. * np.mean ( currents[index:index+1])/1000.
	fp.write ( "\t%f : %f : %f\n" % ( start_time, end_time, avg_current))
fp.write ( "<< Power Monitor\n")
fp.close ()
