import sys
import re
import pandas as pd
import numpy as np

current_trace_path = str ( sys.argv[1])
experiment_time_log = str ( sys.argv[2])

camera_model_path = str ( sys.argv[3])
camera_timeline_path = str ( sys.argv[4])

def extract_camera_model ( path):
	fp = open ( path, 'r')
	for line in fp:
		m = re.match ( '(\S+)', line)
		camera = float ( m.group ( 1))
	return camera

fp = open ( experiment_time_log, 'r')
for line in fp:
	m = re.match ( '(\S+)\s+(\S+)', line)
	if m != None:
		experiment_start_time = float ( m.group ( 1))
		experiment_end_time = float ( m.group ( 2))
fp.close ()

camera = extract_camera_model ( camera_model_path)

df_current = pd.read_csv ( current_trace_path, delimiter=':', header=None, names=['Time','Current','Voltage','Energy'])
current_times = df_current.loc [ ( df_current['Time'] >= experiment_start_time) & ( df_current['Time'] <= experiment_end_time)]['Time'].to_numpy()

df_camera_timeline = pd.DataFrame ( columns=['Time','Camera'])

for current_times_index in range ( len ( current_times) -1):
	start_time = current_times[current_times_index]
	end_time = current_times[current_times_index+1]
	delta_time = end_time - start_time
	sub_camera_current = camera * ( delta_time)
	sub_camera_current = camera
	df_camera_timeline.loc[df_camera_timeline.shape[0]] = [ end_time, sub_camera_current]

df_camera_timeline.to_csv ( camera_timeline_path, index=False)
