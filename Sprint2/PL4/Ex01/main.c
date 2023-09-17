#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wait.h>
#include <time.h>
#include <string.h>


int main(){
	
	int size = 8;

	sem_t *semaphore = sem_open("/sem_ex1",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 
	
	sem_t *semaphore2 = sem_open("/sem_ex2",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 
    
	
	
																	
	pid_t pid_array[size];												//creation of an array of pid_t to create all child's
	int status_array[size];
	
	for(int i = 0; i < size;i++){										//fori cycle to create all childs
		
		sem_wait(semaphore2);											// decerement semaphore2
		pid_array[i] = fork();											//create cycle at position i of the pid_array
	
		
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
	
		if(pid_array[i] == 0){											//In the child process
			
			sem_wait(semaphore);										// decerement semaphore
			
			int NUM_COUNT = 200;
			FILE *input_file, *output_file;
			int numbers[NUM_COUNT];
			int i;
			
			input_file = fopen("Numbers.txt", "r");						// Open the input file for reading
			if (input_file == NULL) {
				printf("Error opening input file!\n");
				return 1;
			}
			
			
			for (i = 0; i < NUM_COUNT; i++) {  							// Read in the numbers from the input file
				fscanf(input_file, "%d", &numbers[i]);
			}
    
			fclose(input_file);											// Close the input file
			
			output_file = fopen("Output.txt", "a");
			if (output_file == NULL) {
				printf("Error opening output file!\n");
				return 1;
			}
    
    
			for (i = 0; i < NUM_COUNT; i++) {							// Write the numbers to the output file
				fprintf(output_file, "[%d] %d\n", getpid(),numbers[i]);
			}
    
    
			fclose(output_file);  										// Close the output 
			
			
					
			sem_post(semaphore);										//increment semaphore
			sem_post(semaphore2);										//increment semaphore2
			exit(0);
		}
	}
	
	for(int i = 0; i < size; i++){										// Wait for all process
		waitpid(pid_array[i], &status_array[i], 0);						
	}
	
	
	sem_unlink("/sem_ex1");												// unlink semaphore
	sem_unlink("/sem_ex2");												// unlink semaphore
	
	
	
    return 0;
}
