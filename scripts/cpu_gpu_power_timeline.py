import sys
import re
import pandas as pd

util_path = str ( sys.argv[1])
cpu_model_path = str ( sys.argv[2])
gpu_model_path = str ( sys.argv[3])
cpu_gpu_timeline_path = str ( sys.argv[4])

cpu_model = {}
fp = open ( cpu_model_path, 'r')
for line in fp:
	m = re.match ( '(\S+),(\S+)', line)
	if m != None:
		freq = int ( m.group ( 1))
		comp = float ( m.group ( 2))

		cpu_model[freq] = comp
		continue
	m = re.match ( '(\S+),$', line)
	if m != None:
		cpu_base = float ( m.group ( 1))

gpu_busy_model = {}
gpu_idle_model = {}
fp = open ( gpu_model_path, 'r')
for line in fp:
	m = re.match ( '(\S+),(\S+),(\S+)', line)
	if m != None:
		freq = int ( m.group ( 1))
		busy = float ( m.group ( 2))
		idle = float ( m.group ( 2))

		gpu_busy_model[freq] = busy
		gpu_idle_model[freq] = idle

df = pd.DataFrame ( columns=['Time','CPU','GPU'])
fp = open ( util_path, 'r')
for line in fp:
	m = re.match ( '\s+(\S+)\s+(\S+)\s+(\S+)\s+\| (.*) \| (.*)', line)
	if m != None:
		start_time = float ( m.group ( 1))
		end_time = float ( m.group ( 2))
		avg_current =  float ( m.group ( 3))

		cpu_current = cpu_base
		cpu_utils = m.group ( 4).split ( ',')[:-1]
		for cpu_util in cpu_utils:
			m_sub = re.match ( '\s*\((\S+):(\S+)\)', cpu_util)
			if m_sub != None:
				freq = int ( m_sub.group ( 1))
				util = float ( m_sub.group ( 2))
				sub_current = cpu_model[freq]*util
				cpu_current += sub_current

		gpu_current = 0
		gpu_utils = m.group ( 5).split ( ',')[:-1]
		for gpu_util in gpu_utils:
			m_sub = re.match ( '\s*\((\S+):(\S+):(\S+)\)', gpu_util)
			if m_sub != None:
				freq = int ( m_sub.group ( 1))
				busy_util = float ( m_sub.group ( 2))
				idle_util = float ( m_sub.group ( 2))
				sub_current = gpu_busy_model[freq]*busy_util + gpu_idle_model[freq]*idle_util
				gpu_current += sub_current

		df.loc[df.shape[0]] = [ end_time, cpu_current, gpu_current]
fp.close ()

df.to_csv ( cpu_gpu_timeline_path, index=False)
