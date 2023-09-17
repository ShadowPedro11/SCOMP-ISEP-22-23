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

#define SIZE 3

int main(){
	
	sem_t *semaphore1 = sem_open("/sem1",O_CREAT|O_EXCL, 0644, 0);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	sem_t *semaphore2 = sem_open("/sem2",O_CREAT|O_EXCL, 0644, 0);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	sem_t *semaphore3 = sem_open("/sem3",O_CREAT|O_EXCL, 0644, 0);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 
	
	
	
	pid_t pid_array[SIZE];
	int status_array[SIZE];
	
	for(int i = 0; i < SIZE;i++){										//fori cycle to create all childs
		
	pid_array[i] = fork();												//create cycle at position i of the pid_array
	
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
		
		if(i == 0){
			if(pid_array[i] == 0){	
				printf("Sistemas ");
				fflush(stdout);
				sem_post(semaphore2);									//"activate" semaphore2
				sem_wait(semaphore1);									//"deactivate" semaphore1
				printf("a ");
				sem_post(semaphore2);									//"activate" sempahore2
				fflush(stdout);
				exit(0);
			}
		}
			
		if(i == 1){
			if(pid_array[i] == 0){		
				sem_wait(semaphore2);									//"deactivate" semaphore1
				printf("de ");
				fflush(stdout);
				sem_post(semaphore3);									//"activate" sempahore3
				sem_wait(semaphore2);									//"deactivate" semaphore2
				printf("melhor ");
				fflush(stdout);
				sem_post(semaphore3);									//"activate" sempahore3
				exit(0);
			}
		}
		
		if(i == 2){
			if(pid_array[i] == 0){
				sem_wait(semaphore3);									//"deactivate" semaphore3							
				printf("Computadores - ");
				fflush(stdout);
				sem_post(semaphore1);									//"activate" sempahore1
				sem_wait(semaphore3);									//"deactivate" semaphore3
				printf("disciplina\n");
				fflush(stdout);
				exit(0);
			}
		}
		
	}
	
	for(int i = 0; i < SIZE; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
    
    

	sem_unlink("/sem1");
	sem_unlink("/sem2");
	sem_unlink("/sem3");
	
	return 0;
}
