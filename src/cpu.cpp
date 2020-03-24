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

	/*
	// CPU_SET
	cpu_set_t *cpusetp;
        size_t size;
        int num_cpus, cpu;
        num_cpus = maxThreads;
        cpusetp = CPU_ALLOC(num_cpus);
        if (cpusetp == NULL) {
            perror("CPU_ALLOC");
            exit(EXIT_FAILURE);
        }
        size = CPU_ALLOC_SIZE(num_cpus);
        CPU_ZERO_S(size, cpusetp);
	CPU_SET_S(1, size, cpusetp);
	if (sched_setaffinity(0, sizeof(*cpusetp), cpusetp) == -1) {
            perror("sched_setaffinity");
	    exit(EXIT_FAILURE);
	}
        // for (cpu = 0; cpu < num_cpus; cpu += 2)
        //    CPU_SET_S(1, size, cpusetp);
        // printf("CPU_COUNT() of set:    %d\n", CPU_COUNT_S(size, cpusetp));
        // CPU_FREE(cpusetp);
	// exit(EXIT_SUCCESS);
	*/

	uint64_t nsPerRun = NS_PER_S*atoi(argv[1])/maxThreads; // ns

	// gradually add interference level - Haoran
	// for (uint32_t threads = 1; threads <= maxThreads; threads++) {
	        uint32_t threads = 56;
		printf("Running with %d threads\n", threads);
		omp_set_num_threads(threads);
		// uint64_t endNs = getNs() + nsPerRun;
		uint64_t endNs = getNs() + NS_PER_S*atoi(argv[1]);
                #pragma omp parallel
		while (getNs() < endNs);
	//}

        // CPU_FREE(cpusetp);

	return 0;
}
