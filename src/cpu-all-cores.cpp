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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// for usage of PRIu32, PRIu64
#include <inttypes.h>

#define NS_PER_S (1000000000L)

uint64_t getNs() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*NS_PER_S + ts.tv_nsec;
}

int main(int argc, const char** argv) {
	// Usage: "./cpu-all-cores <duration in sec>"
	if (argc < 2) { 
		printf("Usage: ./cpu-all-cores <duration in sec>\n"); 
		exit(0);
	}
	
	uint32_t maxThreads = omp_get_num_procs();
	printf("Total number of processors (cores): %" PRIu32 "\n", maxThreads);

	uint64_t nsTotal = NS_PER_S*atoi(argv[1]);

	printf("Running with %d threads for %d seconds\n", maxThreads, atoi(argv[1]));
	omp_set_num_threads(maxThreads);
	uint64_t endNs = getNs() + nsTotal;
        
        #pragma omp parallel
	while (getNs() < endNs);

	return 0;
}
