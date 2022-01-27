#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <vector>
#include <map>
#include <string>

typedef std::map<unsigned,double> frequency_utilization_struct;
typedef std::map<unsigned,double>::iterator frequency_utilization_iter;
typedef std::map<unsigned,std::map<unsigned,double>> state_frequency_utilization_struct;
typedef std::map<unsigned,std::map<unsigned,double>>::iterator state_frequency_utilization_iter;
typedef std::vector<unsigned int> frequency_vec_list;
typedef std::map<unsigned,double> net_state_struct;
typedef std::map<unsigned,double>::iterator net_state_iter;

class timeline {
	public:
		timeline ();
		// Add CPU & GPU Frequency and Utilization
		void add_cpu_frequency ( long time, int cpu_id, unsigned frequency);
		void add_gpu_frequency ( long time, unsigned frequency);
		void add_cpu_utilization ( long time, int cpu_id, unsigned state);
		void add_gpu_utilization ( long time, unsigned state);
		void add_equation_core ( long start_time, long end_time, double current, bool header = false);
		// Get CPU & GPU Frequency and Utilization
		unsigned get_cpu_frequency ( long time, int cpu_id);
		unsigned get_gpu_frequency ( long time);
		double get_cpu_utilization ( long start_time, long end_time, int cpu_id);
		double get_gpu_utilization ( long start_time, long end_time);
		void get_cpu_utilization ( long start_time, long end_time,
			state_frequency_utilization_struct& frequency_utilization, bool debug = false);
		double get_cpu_utilization ( long start_time, long end_time, int cpu_id,
			state_frequency_utilization_struct& frequency_utilization, bool debug = false);

		double get_gpu_utilization ( long start_time, long end_time,
			state_frequency_utilization_struct& frequency_utilization, bool debug = false);

		void init_frequency_utilization (
			state_frequency_utilization_struct& frequency_utilization,
			state_frequency_utilization_struct& gpu_frequency_utilization);

		void generate_equation ( std::string path);
	private:
		std::vector<long> time_line;
		struct equation_node {
			long start_time;
			long end_time;
			double current;
			bool header;
		};
		std::vector<equation_node> equations;

		typedef std::pair<long,unsigned> frequency_struct;
		typedef std::vector<frequency_struct> frequency_vec;
		typedef frequency_vec::iterator frequency_iter;

		std::map<int,frequency_vec> cpu_frequency;
		frequency_vec gpu_frequency;

		frequency_vec_list cpu_frequency_vec_list;
		frequency_vec_list gpu_frequency_vec_list;

		typedef std::pair<long,unsigned> utilization_struct;
		typedef std::vector<utilization_struct> utilization_vec;
		typedef utilization_vec::iterator utilization_iter;

		std::map<int,utilization_vec> cpu_utilization;
		utilization_vec gpu_utilization;

		std::map<unsigned int,unsigned int> cpu_cluster;

		unsigned get_frequency ( long time, frequency_vec* frequency_source);
		double get_utilization ( long start_time, long end_time, utilization_vec* utilization_source, bool debug = false);
		double get_utilization ( long start_time, long end_time,
			frequency_vec* frequency_source, utilization_vec* utilization_source,
			state_frequency_utilization_struct& frequency_utilization, bool debug = false);
};

#endif /* _TIMELINE_H_ */
