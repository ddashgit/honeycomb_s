import sys
import re
import pandas as pd
import numpy as np

screen_trace_path = str ( sys.argv[1])
current_trace_path = str ( sys.argv[2])
experiment_time_log = str ( sys.argv[3])
screen_timeline_path = str ( sys.argv[4])

fp = open ( experiment_time_log, 'r')
for line in fp:
	m = re.match ( '(\S+)\s+(\S+)', line)
	if m != None:
		experiment_start_time = float ( m.group ( 1))
		experiment_end_time = float ( m.group ( 2))
fp.close ()

def get_screen_current ( start_time, end_time):
	df_time = df_screen.loc [ ( df_screen['Time'] >= start_time) & ( df_screen['Time'] <= end_time)]
	
	screen_energy = 0.
	for index in df_time.index[:-1]:
		sub_start_time = times[index]
		sub_end_time = times[index+1]
		sub_start_current = darkScreens[index] + screens[index]
		sub_end_current = darkScreens[index+1] + screens[index+1]
		delta_time = sub_end_time - sub_start_time
		delta_energy = delta_time* ( sub_start_current + sub_end_current)/2.
		screen_energy += delta_energy
	
	if ( len ( df_time.index) > 0):
		prev_start_index = df_time.index[0]-1
		if prev_start_index >= 0:
			prev_start_time = times[prev_start_index]
			sub_end_time = times[prev_start_index+1]
			if sub_end_time > start_time:
				delta_time = sub_end_time - start_time
				prev_start_current = darkScreens[prev_start_index] + screens[prev_start_index]
				sub_end_current = darkScreens[prev_start_index+1] + screens[prev_start_index+1]
				slope = ( sub_end_current - prev_start_current)/( sub_end_time - prev_start_time)
				sub_start_current = prev_start_current + slope * ( start_time - prev_start_time)
				delta_energy = delta_time* ( sub_start_current + sub_end_current)/2.
				screen_energy += delta_energy
		
		post_end_index = df_time.index[-1]+1
		if post_end_index <= indices[-1]:
			post_end_time = times[post_end_index]
			sub_start_time = times[post_end_index-1]
			if end_time > sub_start_time:
				delta_time = end_time - sub_start_time
				sub_start_current = darkScreens[post_end_index-1] + screens[post_end_index-1]
				post_end_current = darkScreens[post_end_index] + screens[post_end_index]
				slope = ( post_end_current - sub_start_current)/( post_end_time - sub_start_time)
				sub_end_current = sub_start_current + slope * ( end_time - sub_start_time)
				delta_energy = delta_time* ( sub_start_current + sub_end_current)/2.
				screen_energy += delta_energy

	return ( screen_energy/(end_time-start_time))
	
df_screen = pd.read_csv ( screen_trace_path, delimiter=',',
		header=None, names=['Time','DarkScreen','NormalizedScreen','Screen'])
times		= df_screen['Time'].to_numpy()
darkScreens	= df_screen['DarkScreen'].to_numpy()
screens		= df_screen['Screen'].to_numpy()
indices		= df_screen.index

df_current = pd.read_csv ( current_trace_path, delimiter=':', header=None, names=['Time','Current','Voltage','Energy'])
current_times = df_current.loc [ ( df_current['Time'] >= experiment_start_time) & ( df_current['Time'] <= experiment_end_time)]['Time'].to_numpy()

df_screen_timeline = pd.DataFrame ( columns=['Time','Screen'])

for current_times_index in range ( len ( current_times) -1):
	start_time = current_times[current_times_index]
	end_time = current_times[current_times_index+1]
	sub_screen_current = get_screen_current ( start_time, end_time)
	df_screen_timeline.loc[df_screen_timeline.shape[0]] = [ end_time, sub_screen_current]

df_screen_timeline.to_csv ( screen_timeline_path, index=False)
