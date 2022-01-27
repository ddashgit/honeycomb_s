#include <iostream>
#include <string>
#include <cstdio>
#include <thread>

#include <timeline.h>
#include <extractor.h>

#include <sstream>
#include <iomanip>
#include <string>

int main ( int argc, char** argv)
{
	timeline *t = new ( timeline);
	extractor *e = new extractor( t);
	e->extract ( argv[1]);
	t->generate_equation ( "./");
	return 0;
}
