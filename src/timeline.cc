#include <timeline.h>

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>

timeline::timeline ()
{
}

void timeline::add_cpu_frequency ( long time, int cpu_id, unsigned frequency)
{
	cpu_frequency[cpu_id].push_back( std::make_pair( time, frequency));
}

void timeline::add_gpu_frequency ( long time, unsigned frequency)
{
	gpu_frequency.push_back( std::make_pair( time, frequency));
	if ( std::find ( gpu_frequency_vec_list.begin(), gpu_frequency_vec_list.end(), frequency)
		== gpu_frequency_vec_list.end()) {
		gpu_frequency_vec_list.push_back( frequency);
		std::sort ( gpu_frequency_vec_list.begin(), gpu_frequency_vec_list.end());
	}
}

void timeline::init_frequency_utilization (
	state_frequency_utilization_struct& frequency_utilization,
	state_frequency_utilization_struct& gpu_frequency_utilization)
{
	for ( std::vector<unsigned int>::iterator it = cpu_frequency_vec_list.begin(); it != cpu_frequency_vec_list.end(); ++it) {
		for ( int state = 0; state < 2; ++state) {
			frequency_utilization[state][*it]  = 0;
		}
	}
}

void timeline::add_cpu_utilization ( long time, int cpu_id, unsigned state)
{
	cpu_utilization[cpu_id].push_back ( std::make_pair( time, state));
}

void timeline::add_gpu_utilization ( long time, unsigned state)
{
	gpu_utilization.push_back ( std::make_pair( time, state));
}

void timeline::add_equation_core ( long start_time, long end_time, double current, bool header)
{
	equation_node eq;
	eq.start_time = start_time;
	eq.end_time = end_time;
	eq.current = current;
	eq.header = header;
	equations.push_back ( eq);
}

unsigned timeline::get_cpu_frequency ( long time, int cpu_id)
{
	return get_frequency( time, &cpu_frequency[cpu_id]);
}

unsigned timeline::get_gpu_frequency ( long time)
{
	return get_frequency( time, &gpu_frequency);
}

unsigned timeline::get_frequency ( long time, frequency_vec* frequency_source)
{
	frequency_iter it = std::lower_bound( frequency_source->begin(), frequency_source->end(), frequency_struct( time, 0));
	if ( it != frequency_source->begin()) {
		--it;
	}
	std::printf ( "%ld %ld (%ld) %u\n", time, it->first, time-it->first, it->second);
	return it->second;
}

double timeline::get_cpu_utilization ( long start_time, long end_time, int cpu_id)
{
	return get_utilization( start_time, end_time, &cpu_utilization[cpu_id]);
}

double timeline::get_gpu_utilization ( long start_time, long end_time)
{
	return get_utilization( start_time, end_time, &gpu_utilization);
}


double timeline::get_cpu_utilization ( long start_time, long end_time, int cpu_id, state_frequency_utilization_struct& frequency_utilization, bool debug)
{
	if ( cpu_utilization.find(cpu_id) == cpu_utilization.end() || cpu_frequency.find(cpu_id) == cpu_frequency.end()) {
		return 0.0;
	}
	return get_utilization( start_time, end_time, &cpu_frequency[cpu_id], &cpu_utilization[cpu_id], frequency_utilization, debug);
}

double timeline::get_utilization ( long start_time, long end_time, utilization_vec* utilization_source, bool debug)
{
	std::map<unsigned,long> state_map; // state,time
	state_map[0] = 0;
	state_map[1] = 0;
	// Find starting point
	double prev_state = 0.0;
	utilization_iter it = std::lower_bound( utilization_source->begin(), utilization_source->end(), utilization_struct(start_time,1000));
	if (debug) std::cout << "Found: " << it->first << " : " << utilization_source->size() << std::endl;
	if ( it != utilization_source->begin()) {
		prev_state = (it-1)->second;
	}
	if (debug) std::cout << "Start: " << it->first << " [" << start_time << "," << end_time << "]" << std::endl;
	if ( it->first > end_time) {
		return 0.0;
	}
	if ( it == utilization_source->end()) {
		state_map[prev_state] += (end_time-start_time);
		return (double)state_map[1]/(end_time-start_time);
	}

	state_map[prev_state] += (it->first-start_time);
	prev_state = it->second;
	++it;
	for ( ; it != utilization_source->end(); ++it) {
		if ( it->first >= end_time) {
			break;
		}
		state_map[prev_state] += (it->first-(it-1)->first);
		prev_state = it->second;
		if (debug) std::cout << "\t" << prev_state << " : " << state_map[prev_state] << std::endl;
	}
	state_map[prev_state] += (end_time-(it-1)->first);
	if (debug) std::cout << "\t" << state_map[1] << " " << state_map[0] << " : " << (double)state_map[1]/(end_time-start_time) << " / " << (end_time-start_time) << std::endl;
	return (double)state_map[1]/(end_time-start_time);
}

void timeline::get_cpu_utilization ( long start_time, long end_time, state_frequency_utilization_struct& frequency_utilization, bool debug)
{
	// Initialize
	for ( std::vector<unsigned int>::iterator it = cpu_frequency_vec_list.begin(); it != cpu_frequency_vec_list.end(); ++it) {
		for ( int state = 0; state < 2; ++state) {
			frequency_utilization[state][*it]  = 0;
		}
	}
	for ( std::map<int,utilization_vec>::iterator it = cpu_utilization.begin(); it != cpu_utilization.end(); ++it) {
		int cpu_id = it->first;
		get_cpu_utilization( start_time, end_time, cpu_id, frequency_utilization, debug);
	}
}

double timeline::get_gpu_utilization ( long start_time, long end_time, state_frequency_utilization_struct& frequency_utilization, bool debug)
{
	// Initialize the GPU vector
	/*
	for ( std::vector<unsigned int>::iterator it = gpu_frequency_vec_list.begin(); it != gpu_frequency_vec_list.end(); ++it) {
		for ( int state = 0; state < 4; ++state) {
			frequency_utilization[state][*it] = 0;
		}
	}
	*/
	if ( gpu_utilization.size() == 0 || gpu_frequency.size() == 0) {
		return 0.0;
	}
	return get_utilization( start_time, end_time, &gpu_frequency, &gpu_utilization, frequency_utilization, debug);
}

double timeline::get_utilization ( long start_time, long end_time, frequency_vec* frequency_source, utilization_vec* utilization_source, state_frequency_utilization_struct& frequency_utilization_global, bool debug)
{
	state_frequency_utilization_struct frequency_utilization;
	for ( state_frequency_utilization_iter state_it = frequency_utilization_global.begin(); state_it != frequency_utilization_global.end(); ++state_it) {
		for ( frequency_utilization_iter fu_it = state_it->second.begin(); fu_it != state_it->second.end(); ++fu_it)
		{
			frequency_utilization[state_it->first][fu_it->first] = 0;
		}
	}
	frequency_iter it = std::find( frequency_source->begin(), frequency_source->end(), frequency_struct( start_time, 0));
	if ( it == frequency_source->end()) {
		it = std::lower_bound( frequency_source->begin(), frequency_source->end(), frequency_struct( start_time, 0));
		if ( it == frequency_source->end()) {
			it--;
		}
		if ( debug) std::cout << "F: Using the lower bound : " << start_time << " : " << it->first << std::endl;
	}

	utilization_iter uit = std::find( utilization_source->begin(), utilization_source->end(), utilization_struct(start_time,1000));
	if ( uit == utilization_source->end()) {
		uit = std::lower_bound( utilization_source->begin(), utilization_source->end(), utilization_struct(start_time,1000));
		if ( debug) {
			std::cout << "U: Using the lower bound : " << start_time << " : " << uit->first << std::endl;
			for ( utilization_iter u_it = utilization_source->begin(); u_it != utilization_source->end(); ++u_it) {
				std::cout << '\t' << u_it->first << " : " << u_it->second << std::endl;
			}
		}
	}

	unsigned previous_state;
	if ( uit != utilization_source->begin()) {
		previous_state = (uit-1)->second;
	} else {
		previous_state = 0;
		if ( uit != utilization_source->end() && uit->second == 0) {
			previous_state = 1;
		}
	}
	if ( uit == utilization_source->end())
	{
		--uit;
	}

	unsigned previous_frequency;
	if ( it != frequency_source->begin()) {
		previous_frequency = (it-1)->second;
	} else {
		previous_frequency = it->second; // Approx. Can lead to wrong results. Corrected by aux frequency.
	}
	if ( debug)
		std::cout << "Previous Frequency [Find: " << it->second << "(" << it->first << ")" << "] : " << previous_frequency << std::endl;

	if ( debug)
	std::cout << "S=== " << start_time << " : " << end_time << " (" << it->first << "," << uit->first << ") " << std::endl;
	// ----+---------------+-----
	//   ^
	//      u
	if ( it->first < start_time) {
		long frequency_window_start = start_time;
		long frequency_window_end = end_time;
		if ( debug)
		std::cout << "\t1=> " << frequency_window_start << " :-: " << frequency_window_end
			<< " (" << frequency_window_end-frequency_window_start << ") "
			<< previous_frequency << std::endl;
		double duration_active = 0.0;
		long pervious_utilization_time = frequency_window_start;
		while ( uit->first >= frequency_window_start && uit->first <= frequency_window_end && uit != utilization_source->end()) {
			double local_duration = 0.0;
			if ( previous_state == 1) {
				duration_active += (uit->first-pervious_utilization_time);
			}
			local_duration = (uit->first-pervious_utilization_time);
			frequency_utilization[previous_state][previous_frequency] += local_duration;
			if ( debug)
			std::cout << "\t\tP1 : " << pervious_utilization_time << " <=> " << uit->first << " : S:(" << previous_state << "," << uit->second << ") "
				<< " D(" << pervious_utilization_time-frequency_window_start << "," << uit->first-frequency_window_start << ") "
				<< duration_active << " +" << local_duration << " {" << uit->first << "}" << std::endl;
			previous_state = uit->second;
			pervious_utilization_time = uit->first;
			++uit;
		}
		// End u and frequency_window_end
		{
			double local_duration = 0.0;
			if ( previous_state == 1) {
				duration_active += (frequency_window_end-pervious_utilization_time);
			}
			local_duration = (frequency_window_end-pervious_utilization_time);
			frequency_utilization[previous_state][previous_frequency] += local_duration;
			if ( debug)
			std::cout << "\t\tP2 : " << pervious_utilization_time << " <=> "  << frequency_window_end << "*: S:(" << previous_state << "," << uit->second << ") "
				<< " D(" << pervious_utilization_time-frequency_window_start << "," << frequency_window_end-frequency_window_start << ") "
				<< duration_active << " +" << local_duration << " {" << uit->first << "}" << std::endl;
		}
		// frequency_utilization[previous_frequency] += duration_active;
		previous_frequency = it->second;
		++it;
	}

	// ----+---------------+-----
	//        ^
	//      u
	// Check
	if ( it->first > start_time) {
		long frequency_window_start = start_time;
		long frequency_window_end = it->first;
		if ( it->first > end_time)  {
			frequency_window_end = end_time;
		}
		if ( debug)
		std::cout << "\t2=> " << frequency_window_start << " :-: " << frequency_window_end
			<< " (" << frequency_window_end-frequency_window_start << ") "
			<< previous_frequency << std::endl;
		double duration_active = 0.0;
		long pervious_utilization_time = frequency_window_start;
		while ( uit->first >= frequency_window_start && uit->first <= frequency_window_end && uit != utilization_source->end()) {
			double local_duration = 0.0;
			if ( previous_state == 1) {
				duration_active += (uit->first-pervious_utilization_time);
			}
			local_duration = (uit->first-pervious_utilization_time);
			frequency_utilization[previous_state][previous_frequency] += local_duration;
			if ( debug)
			std::cout << "\t\tS1 : " << pervious_utilization_time << " <=> " << uit->first << " : S:(" << previous_state << "," << uit->second << ") "
				<< " D(" << pervious_utilization_time-frequency_window_start << "," << uit->first-frequency_window_start << ") "
				<< duration_active << " +" << local_duration << std::endl;
			previous_state = uit->second;
			pervious_utilization_time = uit->first;
			++uit;
		}
		// End u and frequency_window_end
		{
			double local_duration = 0.0;
			if ( previous_state == 1) {
				duration_active += (frequency_window_end-pervious_utilization_time);
			}
			local_duration = (frequency_window_end-pervious_utilization_time);
			frequency_utilization[previous_state][previous_frequency] += local_duration;
			if ( debug)
			std::cout << "\t\tS2 : " << pervious_utilization_time << " <=> "  << frequency_window_end << "*: S:(" << previous_state << "," << uit->second << ") "
				<< " D(" << pervious_utilization_time-frequency_window_start << "," << frequency_window_end-frequency_window_start << ") "
				<< duration_active << " +" << local_duration << std::endl;
		}
		// frequency_utilization[previous_frequency] += duration_active;
	}
	// ----+---------------+-----
	//       ^   ^    ^
	//        u u  u u
	while ( it->first >= start_time && it->first <= end_time && it != frequency_source->end()) {
		long frequency_window_start = it->first;
		long frequency_window_end;
		previous_frequency = it->second;
		if ( it+1 != frequency_source->end() && (it+1)->first <= end_time)  {
			frequency_window_end = (it+1)->first;
		} else {
			frequency_window_end = end_time;
		}
		if ( debug)
		std::cout << "\t3=> " << frequency_window_start << " :-: " << frequency_window_end
			<< " (" << frequency_window_end-frequency_window_start << ") "
			<< previous_frequency << std::endl;
		double duration_active = 0.0;
		long pervious_utilization_time = frequency_window_start;
		while ( uit->first >= frequency_window_start && uit->first <= frequency_window_end && uit != utilization_source->end()) {
			double local_duration = 0.0;
			if ( previous_state == 1) {
				duration_active += (uit->first-pervious_utilization_time);
			}
			local_duration = (uit->first-pervious_utilization_time);
			frequency_utilization[previous_state][previous_frequency] += local_duration;
			if ( debug)
			std::cout << "\t\tM1 : " << pervious_utilization_time << " <=> " << uit->first << " : S:(" << previous_state << "," << uit->second << ") "
				<< " D(" << pervious_utilization_time-frequency_window_start << "," << uit->first-frequency_window_start << ") "
				<< duration_active << " +" << local_duration << std::endl;
			previous_state = uit->second;
			pervious_utilization_time = uit->first;
			++uit;
		}
		// End u and frequency_window_end
		{
			double local_duration = 0.0;
			if ( previous_state == 1) {
				duration_active += (frequency_window_end-pervious_utilization_time);
			}
			local_duration = (frequency_window_end-pervious_utilization_time);
			frequency_utilization[previous_state][previous_frequency] += local_duration;
			if ( debug)
			std::cout << "\t\tM2 : " << pervious_utilization_time << " <=> "  << frequency_window_end << "*: S:(" << previous_state << "," << uit->second << ") "
				<< " D(" << pervious_utilization_time-frequency_window_start << "," << frequency_window_end-frequency_window_start << ") "
				<< duration_active << " +" << local_duration << std::endl;
		}
		// frequency_utilization[previous_frequency] += duration_active;
		previous_frequency = it->second;
		++it;
	}
	double total = 0.0;
	for ( state_frequency_utilization_iter state_it = frequency_utilization.begin(); state_it != frequency_utilization.end(); ++state_it) {
		for ( frequency_utilization_iter fu_it = state_it->second.begin(); fu_it != state_it->second.end(); ++fu_it)
		{
			frequency_utilization[state_it->first][fu_it->first]  /= (end_time-start_time);
			if ( state_it->first == 1) {
				total += frequency_utilization[state_it->first][fu_it->first];
			}
			frequency_utilization_global[state_it->first][fu_it->first]  += frequency_utilization[state_it->first][fu_it->first];
		 	// if ( debug)
		 	// std::cout << "\t\t\t= (" << state_it->first << ") " << fu_it->first << " " << fu_it->second << std::endl;
		}
	}
	if ( debug)
	std::cout << std::endl;
	for ( state_frequency_utilization_iter state_it = frequency_utilization.begin(); state_it != frequency_utilization.end(); ++state_it) {
		for ( frequency_utilization_iter fu_it = state_it->second.begin(); fu_it != state_it->second.end(); ++fu_it)
		{
		 	if ( debug) {
				std::printf ( "%10u: %6.2f %6.2f: %6.2f %6.2f\n", fu_it->first,
					100*frequency_utilization[0][fu_it->first], 100*frequency_utilization[1][fu_it->first],
					100*frequency_utilization_global[0][fu_it->first], 100*frequency_utilization_global[1][fu_it->first]);
			}
		}
		break;
	}
	if ( debug)
	std::cout << "=== " << start_time << " : " << end_time << std::endl;

	if ( debug)
	printf ( " %6.2f", (100*total));
	return total;
}

void timeline::generate_equation ( std::string path)
{
	for ( std::vector<equation_node>::iterator it = equations.begin(); it != equations.end(); ++it) {
		if ( !it->header) {
			std::printf ( "\t%.6lf %.6lf %lf", it->start_time/1000000.0, it->end_time/1000000.0, it->current);
			state_frequency_utilization_struct cpu_frequency_utilization, gpu_frequency_utilization;
			get_cpu_utilization ( it->start_time, it->end_time, cpu_frequency_utilization, false);
			get_gpu_utilization ( it->start_time, it->end_time, gpu_frequency_utilization, false);

			std::printf ( " |");
			for ( state_frequency_utilization_iter state_it = cpu_frequency_utilization.begin();
				state_it != cpu_frequency_utilization.end(); ++state_it) {
				for ( frequency_utilization_iter fu_it = state_it->second.begin(); fu_it != state_it->second.end(); ++fu_it)
				{
					std::printf ( " (%d:%.6f),", fu_it->first, cpu_frequency_utilization[1][fu_it->first]);
				}
				break;
			}
			std::printf ( " |");
			for ( state_frequency_utilization_iter state_it = gpu_frequency_utilization.begin();
				state_it != gpu_frequency_utilization.end(); ++state_it) {
				for ( frequency_utilization_iter fu_it = state_it->second.begin(); fu_it != state_it->second.end(); ++fu_it)
				{
					std::printf ( " (%d:%.6f:%.6f),", fu_it->first, gpu_frequency_utilization[1][fu_it->first], gpu_frequency_utilization[0][fu_it->first]);
				}
				break;
			}
			std::printf ( "\n");
		} else {
			std::printf ( ">>> Time %.6lf %.6lf\n", it->start_time/1000000.0, it->end_time/1000000.0);
		}
	}
	std::printf ( ">>> Time %.6lf %.6lf\n", 0.0, 0.0);
}
