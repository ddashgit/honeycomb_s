import os
import sys

def generate_config ( config_path, timeline_dir, timeline_snippet_path):
	cpu_frequency_path = os.path.abspath ( os.path.join ( timeline_dir, '__CPU_FREQUENCY_INPUT.TXT'))
	cpu_utilization_path = os.path.abspath ( os.path.join ( timeline_dir, '__CPU_UTILIZATION_INPUT.TXT'))
	gpu_frequency_path = os.path.abspath ( os.path.join ( timeline_dir, '__GPU_FREQUENCY_INPUT.TXT'))
	gpu_utilization_path = os.path.abspath ( os.path.join ( timeline_dir, '__GPU_UTILIZATION_ALL.TXT'))
	timeline_snippet_path = os.path.abspath ( timeline_snippet_path)
	if not ( os.path.exists ( cpu_frequency_path) \
			and os.path.exists ( cpu_utilization_path) \
			and os.path.exists ( gpu_frequency_path) \
			and os.path.exists ( gpu_utilization_path)):
		print ( 'The path %s does not contain the timeline files')
		exit ()
	fpw = open ( config_path, 'w')
	fpw.write ( 'cpu_frequency=%s\n' % ( cpu_frequency_path))
	fpw.write ( 'cpu_utilization=%s\n' % ( cpu_utilization_path))
	fpw.write ( 'gpu_frequency=%s\n' % ( gpu_frequency_path))
	fpw.write ( 'gpu_utilization=%s\n' % ( gpu_utilization_path))
	fpw.write ( 'equations=%s\n' % ( timeline_snippet_path))
	fpw.close ()

timeline_dir = str ( sys.argv[1])
timeline_snippet_path = str ( sys.argv[2])
config_path = str ( sys.argv[3])

generate_config ( config_path, timeline_dir, timeline_snippet_path)
