#include <stdio.h>
#include <mpi.h>
#include <inttypes.h>
#include <math.h>
#include "results.h"

void write_results(timer* t, char* inputfile, int thread_count, char algorithm, int size, char* destfolder) {	
	#ifdef __linux__ 
		struct timespec spec;
		clock_gettime(CLOCK_REALTIME, &spec);
		long timestamp = floor(spec.tv_sec * 1.0e8 + spec.tv_nsec / 100.0);
	# else
    	long timestamp = 0;
	#endif

	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo = localtime(&rawtime);
	char datetime[80];
	strftime((char*) &datetime, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

	char filename[200];
	if (algorithm == 'S') {
		sprintf((char*) &filename, "%s/%c.txt", destfolder, algorithm);
	} else {
		sprintf((char*) &filename, "%s/%c%i.txt", destfolder, algorithm, thread_count);
	}
	
   	FILE* f = fopen(filename, "a");
	fprintf(f, "%s\t%s\t%li\t%i\t%i\t%.9f\t%.9f\t%i\t%.9f\t\n",
			inputfile,
			datetime,
			timestamp,
			thread_count,
			size,
			t->timeval_diff,
			t->timespec_diff,
			(int) t->clock_diff,
			t->clock_diff_time);

	fclose(f);
}
