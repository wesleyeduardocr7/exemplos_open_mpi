#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include "timer.h"


timer* start_timer() {
	timer* t = malloc(sizeof(timer));
	gettimeofday(&t->timeval_start, NULL);
	#ifdef __linux__ 
		clock_gettime(CLOCK_REALTIME, &t->timespec_start);
	#endif
	t->clock_start = clock();
	return t;
}

void stop_timer(timer* t, bool print_results) {
	gettimeofday(&t->timeval_end, NULL);
	#ifdef __linux__ 
		clock_gettime(CLOCK_REALTIME, &t->timespec_end);
	#endif
	t->clock_end = clock();
	t->timeval_diff = (t->timeval_end.tv_sec - t->timeval_start.tv_sec) * 1000.0; 
	t->timeval_diff += (t->timeval_end.tv_usec - t->timeval_start.tv_usec) / 1000.0; 
	#ifdef __linux__ 		
		t->timespec_diff = (t->timespec_end.tv_sec - t->timespec_start.tv_sec) * 1000.0; 
		t->timespec_diff += (t->timespec_end.tv_nsec - t->timespec_start.tv_nsec) / 1000000.0; 
	#else
		t->timespec_diff = 0;
	#endif	
	t->clock_diff = t->clock_end - t->clock_start;
	t->clock_diff_time = ((float) t->clock_diff / CLOCKS_PER_SEC * 1000.0);

	if (print_results) {
		printf("[timeval] %.9fms\n", t->timeval_diff);
		#ifdef __linux__ 
			printf("[timespec] %.9fms\n", t->timespec_diff);
		#endif
		printf("[clock] %d ticks -> %.9fms\n", (int) t->clock_diff, t->clock_diff_time);
	}
}

