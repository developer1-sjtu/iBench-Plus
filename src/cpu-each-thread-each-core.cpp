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

// getpid()
#include <unistd.h>
// gettid()
#include <sys/types.h>
#include <sys/syscall.h>

// for usage of PRIu32, PRIu64
#include <inttypes.h>

// for usage of cpu_set
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>

#define NS_PER_S (1000000000L)

uint64_t getNs() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*NS_PER_S + ts.tv_nsec;
}

int main(int argc, const char** argv) {
	// Usage: "./cpu <duration in sec>"
	if (argc < 2) { 
		printf("Usage: ./cpu <duration in sec>\n"); 
		exit(0);
	}
	
	uint32_t maxThreads = omp_get_num_procs();
	printf("Total number of processors (cores): %" PRIu32 "\n", maxThreads);

	uint64_t nsTotal = NS_PER_S*atoi(argv[1]); // ns

	printf("Running with %d threads for %d seconds\n", maxThreads, atoi(argv[1]));
	omp_set_num_threads(maxThreads);
	uint64_t endNs = getNs() + nsTotal;
	int endless = 0;
	if (atoi(argv[1]) == -1) {
		endless = 1;
		printf("Endless Mode\n");
	}
                
	#pragma omp parallel for
	for (int i = 0; i < maxThreads; i++)
	{
		printf("Core #%d, TID: %ld\n", i, syscall(SYS_gettid));
		
		// bind to a particular core
		cpu_set_t *cpusetp;
		cpusetp = CPU_ALLOC(maxThreads);
		if (cpusetp == NULL) {
			perror("CPU_ALLOC");
			exit(EXIT_FAILURE);
		}
		CPU_ZERO_S(maxThreads, cpusetp);
		CPU_SET_S(i, maxThreads, cpusetp);
		if (sched_setaffinity(syscall(SYS_gettid), sizeof(*cpusetp), cpusetp) == -1) {
			perror("sched_setaffinity");
			exit(EXIT_FAILURE);
		}
		CPU_FREE(cpusetp);

		// empty while-loop
		if (endless == 0)
			while (getNs() < endNs);
		else
			while (1);
	}

	return 0;
}
