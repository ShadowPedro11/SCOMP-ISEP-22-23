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

#define SIZE 10


typedef struct {
    char string[100];
    int nReaders;
    int nWriters;
} sharedMA;

int main(){
	
	int fd, data_size, trunk;
	sharedMA *ptr;
	
	data_size = sizeof(sharedMA);
	
	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of barrier struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of barrier 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (sharedMA *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	sem_t *semaphore1 = sem_open("/sem1",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore2 = sem_open("/sem2",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	sem_t *semaphore3 = sem_open("/sem3",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore3 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	
	

	sprintf(ptr->string, "%d", getpid());
	
	
	pid_t pid_array[SIZE];
	int status_array[SIZE];
	
	for(int i = 0; i < SIZE;i++){										//fori cycle to create all childs
		
		pid_array[i] = fork();											//create cycle at position i of the pid_array
	
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
		
		
		if(pid_array[i] == 0){
			
			if(i%2==0){ 												//Reader
				
				sem_wait(semaphore3);									// enter the critical section
				sem_post(semaphore3);									// release the semaphore3 immediately
				
				sem_wait(semaphore1);									// acquire semaphore1 to modify the shared memory
				ptr->nReaders++;										// increase the number of active readers
				
				if(ptr->nReaders == 1){									// if this is the first active reader, acquire semaphore2 to block Writers
					sem_wait(semaphore2);								// acquire semaphore2
				}
				
				sem_post(semaphore1);									// release semaphore1
				
				
				printf("Reader -> %s |nWriters: %d |nReaders: %d\n",ptr->string,ptr->nWriters,ptr->nReaders);
				
				sem_wait(semaphore1);									// acquire semaphore1 to modify the shared memory
				ptr->nReaders--;										// decrement the number of active readers
				
				if(ptr->nReaders == 0){									// if no Readers are active, release semaphore2
					sem_post(semaphore2);								// release semaphore2
				}
				sleep(i);												// sleep
				
				sem_post(semaphore1);									// release semaphore1				

				
				
			}else{ 														//Writer
				
				sem_wait(semaphore3);									// enter the critical section
				
				sem_wait(semaphore2);									// acquire semaphore2 to block access to the shared memory area
				
				ptr->nWriters++;
				
				printf("Writer -> %d |nWriters: %d |nReaders: %d\n",getpid(),ptr->nWriters,ptr->nReaders);
				sprintf(ptr->string, "%d", getpid());
				
				ptr->nWriters--;
				
				
				sem_post(semaphore3);									// release semaphore3
				
				sem_post(semaphore2);									// release semaphore2
				
			}
			
			exit(0);
		}
		
	}
	
	
	
	for(int i = 0; i < SIZE; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
    
    

	sem_unlink("/sem1");
	sem_unlink("/sem2");
	sem_unlink("/sem3");
	
	if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment");	
		exit(1);
	}
	close(fd);
	if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		perror("Erro! - unlink the shared memory segment"); 
		exit(1);
	}
	
	return 0;
}
