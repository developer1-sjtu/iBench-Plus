/**
 * Copyright (C) 2019-2024 by The Board of Trustees of University of Illinois, Urbana-Champaign
 * 
 * Author: Haoran Qiu
 * Date: March 23, 2020
 *
 * This file is part of iBench-Plus. 
 *    
 * iBench-Plus is free software; you can redistribute it and/or modify it under the terms of the
 * Modified BSD-3 License as published by the Open Source Initiative.
 *    
 * iBench-Plus is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
 * the BSD-3 License for more details.
 *    
 * You should have received a copy of the Modified BSD-3 License along with this program. If not, 
 * see <https://opensource.org/licenses/BSD-3-Clause>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <float.h>

#include <omp.h>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/syscall.h>

#define ARRAY_SIZE 55000000
#define VERSION 2

/* Version II - Using Global Arrays */
double bwData1[ARRAY_SIZE];
double bwData2[ARRAY_SIZE];

unsigned int bwStreamSize = ARRAY_SIZE;

int main (int argc, char **argv) { 
	/* Usage: ./memBw <duration in sec> */
	if (argc < 2) { 
		printf("Usage: ./memBw <duration in sec>\n"); 
		exit(0);
	}
	
	uint32_t maxThreads = omp_get_num_procs();
        printf("Total number of processors (cores): %" PRIu32 "\n", maxThreads);

	int endless = 0;
        if (atoi(argv[1]) == -1) {
                endless = 1;
                printf("Running with %d threads in endless mode\n", maxThreads);
        } else {
		printf("Running with %d threads for %d seconds\n", maxThreads, atoi(argv[1]));
	}

	// get the process id of the master thread
	printf("Master process PID: %d\n", getpid());

	// one array of bwStreamSize double variables
	double mem_size_in_bytes = bwStreamSize * 8;
	double mem_size_in_mib = mem_size_in_bytes / 1048576; // / 1.049e6;

	omp_set_num_threads(maxThreads);

	/* Version I */
	if (VERSION == 1) {
		int master_pid = getpid();
		unsigned int usr_timer = atoi(argv[1]) * maxThreads;
		#pragma omp parallel
		{
			double *bwData1;
			double *bwData2;

			// calloc also initializes the memory (to zero) compared to alloc
			bwData1 = (double*) calloc(bwStreamSize, sizeof(double));
			bwData2 = (double*) calloc(bwStreamSize, sizeof(double));

			int iterations = 0;
			double total_duration = 0;

			double time_spent = 0.0;
			while (time_spent < usr_timer || endless == 1) {
				clock_t begin = clock();

				for (int i = 0; i < bwStreamSize; i++) {
					bwData1[i] = bwData2[i];
				}
			
				clock_t end = clock(); 
				double duration = (double)(end - begin) / CLOCKS_PER_SEC;
				time_spent += duration;

				iterations++;
				total_duration += duration;
				if (iterations == 200 && syscall(SYS_gettid) == master_pid) {
					printf("Approximate Realtime Memory Bandwidth Consumed: %f MiB/s\n", maxThreads*maxThreads*mem_size_in_mib*iterations/total_duration);
					printf("Duration: %fs\n", total_duration/maxThreads);
					iterations = 0;
					total_duration = 0;
					printf("\n");
				}
			}

			free(bwData1);
			free(bwData2);
		}
	}
	/* End of Version I */

	/* Version II */
	if (VERSION == 2) {
		unsigned int usr_timer = atoi(argv[1]);
		double time_spent = 0.0;
		int iterations = 0;
		double total_duration = 0;
		while (time_spent < usr_timer || endless == 1) {
			clock_t begin = clock();

			#pragma omp parallel for
			for (int i = 0; i < bwStreamSize; i++) {
				bwData1[i] = bwData2[i];
			}

			clock_t end = clock();
			double duration = (double)(end - begin) / CLOCKS_PER_SEC;
			time_spent += duration;

			iterations += 1;
			total_duration += duration;

			if (iterations == 100) {
				double real_duration = total_duration/maxThreads;
				printf("Realtime Memory Bandwidth Consumed: %f MiB/s\n", iterations*mem_size_in_mib/real_duration);
				printf("Duration: %f\n", real_duration);
				iterations = 0;
				total_duration = 0;
			}
		}
	}
	/* End of Version II */

	return 0;
}
