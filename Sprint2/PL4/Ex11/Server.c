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


typedef struct {
	int ticket;
} ticketLine;

int main(){
	
	int fd, data_size, trunk;
	ticketLine *ptr;
	
	data_size = sizeof(ticketLine);
	
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
	
	ptr = (ticketLine *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	sem_t *semaphore1 = sem_open("/sem1",O_CREAT|O_EXCL, 0644, 0);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore2 = sem_open("/sem2",O_CREAT|O_EXCL, 0644, 0);
	if(semaphore2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore3 = sem_open("/sem3",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore3 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}

	
	int availablesTickets = 3;
	
	sem_t *semaphore4 = sem_open("/sem4",O_CREAT|O_EXCL, 0644, availablesTickets);
	if(semaphore4 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	

	
	for(int i = 0; i < availablesTickets; i++){
		sem_wait(semaphore2);							// Waits for a client
		ptr->ticket = i+1;								// Give ticket
		sem_wait(semaphore4);							//Decrements the number of tickets
		sem_post(semaphore1);							// Informs that ticket is available
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
	if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		perror("Erro! - unlink the shared memory segment"); 
		exit(1);
	}
	
	return 0;
}
