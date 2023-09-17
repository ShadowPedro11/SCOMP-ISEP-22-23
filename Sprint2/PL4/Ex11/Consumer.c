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

#define SIZE 4

#define TICKETS 10

typedef struct {
	int ticket;
} ticketLine;

int main(){
	
	int fd, data_size, trunk;
	ticketLine *ptr;
	
	data_size = sizeof(ticketLine);
	
	fd = shm_open("/shmtest", O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of barrier struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of barrier 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (ticketLine *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	sem_t *semaphore1 = sem_open("/sem1", 0);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore2 = sem_open("/sem2", 0);
	if(semaphore2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore3 = sem_open("/sem3", 0);
	if(semaphore3 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore4 = sem_open("/sem4", 0);
	if(semaphore4 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	

	
	
	
	pid_t pid_array[SIZE];
	int status_array[SIZE];
	
	for(int i = 0; i < SIZE;i++){										//fori cycle to create all childs
		
		pid_array[i] = fork();											//create cycle at position i of the pid_array
	
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
		
		
		if(pid_array[i] == 0){
			sem_wait(semaphore3);						// Controll access, 1 per time
			
			int sval;
			sem_getvalue(semaphore4, &sval);							//Check ticket available
			
			if(sval==0){
				printf("CLOSE: %d No tickets available\n", getpid());
				sem_post(semaphore3);
				exit(0);
			}
			
			sem_post(semaphore2);						//Client arrive
			printf("CLIENT %d come \n",getpid());		//Say it number
			
			sem_wait(semaphore1);						//Waits ticket
			
			int random;
			srand(getpid());
			random = rand() % 10;
			sleep(random);			
			int ticket = ptr->ticket;	
					
			printf("CLIENT %d received ticket %d\n",getpid(),ticket);	// Print ticket
			
			
			sem_post(semaphore3);						// Controll access, 1 per time
			exit(0);
		}
		
	}
	
	
	
	for(int i = 0; i < SIZE; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
    
    

	sem_unlink("/sem1");
	sem_unlink("/sem2");
	sem_unlink("/sem3");
	sem_unlink("/sem4");
	
	if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment");	
		exit(1);
	}
	close(fd);
	
	
	return 0;
}
