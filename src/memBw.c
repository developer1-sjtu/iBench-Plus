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

#ifndef N
#define N 2000000
#endif

static double bwData[N];

unsigned int bwStreamSize = N;

int main (int argc, char **argv) { 
	/* Usage: ./memBw <duration in sec> */
	if (argc < 2) { 
		printf("Usage: ./memBw <duration in sec>\n"); 
		exit(0);
	}
	double scalar = 3.0;
	
	uint32_t maxThreads = omp_get_num_procs();
        printf("Total number of processors (cores): %" PRIu32 "\n", maxThreads);
        printf("Running with %d threads for %d seconds\n", maxThreads, atoi(argv[1]));

	unsigned int usr_timer = atoi(argv[1]) * maxThreads;

	int endless = 0;
        if (atoi(argv[1]) == -1) {
                endless = 1;
                printf("Endless Mode\n");
        }

	omp_set_num_threads(maxThreads);
	#pragma omp parallel
	{
		double time_spent = 0.0;
		while (time_spent < usr_timer || endless == 1) {
			double *mid = bwData + (bwStreamSize/2);
			clock_t begin = clock();

			for (int i = 0; i < bwStreamSize/2; i++) {
				bwData[i] = scalar*mid[i];
			}
		
			for (int i = 0; i < bwStreamSize/2; i++) {
				mid[i] = scalar*bwData[i];
			}

			clock_t end = clock(); 
  			time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		}
	}
	return 0;
}
