#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <wait.h>
#include <time.h>
#include <string.h>


long convertToMicroSeconds(int time, char* units) {						// Convert time inputed to microSeconds
    long microseconds = 0;
    if (strcmp(units, "us") == 0) {
		printf("us\n");
        microseconds = time;
    } else if (strcmp(units, "ms") == 0) {
		printf("ms\n");
        microseconds = time * 1000;
    } else if (strcmp(units, "s") == 0) {
		printf("s\n");
        microseconds = time * 1000000;
    } else {
        fprintf(stderr, "Error: unsupported time unit %s\n", units);
        exit(1);
    }
    return microseconds;
}


int main(int argc, char** argv) {
	
    if (argc != 4) {
        fprintf(stderr, "Usage: %s n time\n", argv[0]);
        return 1;
    }
    
    int SIZE = atoi(argv[1]);
    int n = atoi(argv[2]);
    char* time = argv[3];

	long time_us = convertToMicroSeconds(n,time);
	
	struct timeval start, end;
    long long elapsed_time;

	pid_t pid_array[SIZE];
	int status_array[SIZE];
	
	gettimeofday(&start, NULL);											// Get actual time
	
	for(int i = 0; i < SIZE;i++){										//fori cycle to create all childs
		
		pid_array[i] = fork();											//create cycle at position i of the pid_array
	
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
		
	
		if(pid_array[i] == 0){
			usleep(time_us);											// sleep for time inputed
            exit(0);
		}
		
		
	}
		
	
	for(int i = 0; i < SIZE; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
	
	gettimeofday(&end, NULL);											// Get actual time
	
	elapsed_time = (end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_usec - start.tv_usec); // Calculate difference
	
	printf("Dif: %lld\n",elapsed_time);


    return 0;
}
