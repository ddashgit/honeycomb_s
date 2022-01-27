#include <extractor.h>

#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <thread>
#include <iomanip>

extractor::extractor ( timeline *_t) :
	t(_t)
{
}

bool extractor::extract ( std::string path)
{
	std::fstream fp ( path, std::fstream::in);
	if ( !fp) {
		return false;
	}
	std::string line;
	std::string type,path_to;
	std::size_t pos;
	while( std::getline( fp, line)) {
		if ( (pos=line.find('=')) != std::string::npos) {
			type = line.substr(0,pos);
			path_to = line.substr(pos+1);
			std::cout << type << " " << path_to << std::endl;
			if ( type.compare("") == 0) {
			} else if ( type.compare("cpu_frequency")   == 0) { // CPU Frequency
				extract_cpu_frequency ( path_to);
			} else if ( type.compare("cpu_utilization") == 0) { // CPU Utilization
				extract_cpu_utilization ( path_to);
			} else if ( type.compare("gpu_frequency")   == 0) { // GPU Frequency
				extract_gpu_frequency ( path_to);
			} else if ( type.compare("gpu_utilization") == 0) { // GPU Utilization
				extract_gpu_utilization ( path_to);
			} else if ( type.compare("equations")       == 0) { // Equations
				extract_equations_core ( path_to);
			}

		}
	}
	fp.close();
	return true;
}

void extractor::extract_cpu_frequency ( std::string path)
{
	std::fstream fp ( path, std::fstream::in);
	std::string line;
	long time;
	int cpu;
	unsigned long frequency;
	while( std::getline( fp, line)) {
		if( std::sscanf ( line.c_str(), "%ld:%d,%lu", &time, &cpu, &frequency))
		{
			t->add_cpu_frequency ( time, cpu, frequency);
		}
	}
	fp.close();
}

void extractor::extract_gpu_frequency ( std::string path)
{
	std::fstream fp ( path, std::fstream::in);
	std::string line;
	long time;
	unsigned frequency;
	while( std::getline( fp, line)) {
		if( std::sscanf ( line.c_str(), "%ld:%u", &time, &frequency))
		{
			t->add_gpu_frequency ( time, frequency);
		}
	}
	fp.close();
}

void extractor::extract_cpu_utilization ( std::string path)
{
	std::fstream fp ( path, std::fstream::in);
	std::string line;
	long time;
	int cpu;
	unsigned state;
	while( std::getline( fp, line)) {
		if( std::sscanf ( line.c_str(), "%ld:%d,%u", &time, &cpu, &state))
		{
			t->add_cpu_utilization ( time, cpu, state);
		}
	}
	fp.close();
}

void extractor::extract_gpu_utilization ( std::string path)
{
	std::fstream fp ( path, std::fstream::in);
	std::string line;
	long time;
	unsigned state;
	while( std::getline( fp, line)) {
		if( std::sscanf ( line.c_str(), "%ld:%u", &time, &state))
		{
			t->add_gpu_utilization ( time, state);
		}
	}
	fp.close();
}

void extractor::extract_equations_core ( std::string path)
{
	std::fstream fp ( path, std::fstream::in);
	std::string line;
	double start_time;
	double end_time;
	double current;
	while( std::getline( fp, line)) {
		if( std::sscanf ( line.c_str(), "%lf : %lf : %lf", &start_time, &end_time, &current))
		{
			t->add_equation_core ( long ( 1000000*start_time), long ( 1000000*end_time), current, false);
		}
		if( std::sscanf ( line.c_str(), ">>> Time %lf : %lf", &start_time, &end_time))
		{
			t->add_equation_core ( long ( 1000000*start_time), long ( 1000000*end_time), -1.0, true);
		}
	}
	fp.close();
}
