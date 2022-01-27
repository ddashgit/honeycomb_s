#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include <timeline.h>

#include <string>

class extractor {
	public:
		bool extract ( std::string path);
		extractor ( timeline *_t);
	private:
		// Get probes
		void extract_probes ( std::string path); // Decrepated
		std::string generate_script_parsing_trace (); // Decrepated

		void extract_cpu_frequency ( std::string path);
		void extract_cpu_cluster( std::string path);
		void extract_gpu_frequency ( std::string path);
		void extract_cpu_utilization ( std::string path);
		void extract_gpu_utilization ( std::string path);
		void extract_equations_core ( std::string path);

		timeline *t;
};

#endif /* _EXTRACTOR_H_ */
