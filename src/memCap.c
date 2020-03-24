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

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define NS_PER_S (1000000000L)

unsigned long int getNs() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*NS_PER_S + ts.tv_nsec;
}

void remove_all_chars(char* str, char c) {
	char *pr = str, *pw = str;
	while (*pr) {
		*pw = *pr++;
		pw += (*pw != c);
	}
	*pw = '\0';
}

long long int memory_size_byte(void) {
	char line[512], buffer[32];
	long long int column;
	FILE *meminfo;

	if (!(meminfo = fopen("/proc/meminfo", "r"))) {
		perror("/proc/meminfo: fopen");
		return -1;
	}

	while (fgets(line, sizeof(line), meminfo)) {
		if (strstr(line, "MemTotal")) {
			char* colStr;
			colStr = strstr(line, ":");
			remove_all_chars(colStr, ':'); 
			remove_all_chars(colStr, 'k'); 
			remove_all_chars(colStr, 'B');
			remove_all_chars(colStr, ' ');
			column = atoi(colStr);
		        column = 1000*column;	
			fclose(meminfo);
			return column; 
		}
	}
	fclose(meminfo);
	return -1;
}

int main(int argc, char **argv) {
	timespec sleepValue = {0};

	char* volatile block;
	long long int MEM_SIZE = memory_size_byte(); 
	printf("Total Memory Size: %llu bytes\n", MEM_SIZE);

	/*Usage: ./memCap <duration in sec>*/
	if (argc < 2) { 
		printf("Usage: ./cap_mem <duration in sec>\n"); 
		exit(0); 
	}	
	
	// create a mapping in virtual memory
	// MAP_ANONYMOUS - mapping not backed by any file, ignoring file descriptor and offset
	// returns the address of the new mapping
	block = (char*)mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (block == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	int usr_timer = atoi(argv[1]);
	double time_spent = 0.0; 
  	clock_t begin, end;

	while (time_spent < usr_timer) {
  		begin = clock();

		// copy "MEM_SIZE/2" bytes from "block+MEM_SIZE/2" to "block"
		memcpy(block, block+MEM_SIZE/2, MEM_SIZE/2);
		printf("Done for copying MEM_SIZE/2 bytes once");
		
		//sleepValue.tv_nsec = (usr_timer-getNs())/usr_timer;
		//nanosleep(&sleepValue, NULL);
		
		end = clock();
  		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
	}
	return 0;
}
