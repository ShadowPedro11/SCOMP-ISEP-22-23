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

#define SIZE 6

typedef struct {
	int nBarrier;
} barrier;


void buy_chips(){
	printf("Process %d -> buy chips\n",getpid());
}
void buy_beer(){
	printf("Process %d -> buy beer\n",getpid());
}
void eat_and_drink(){
	printf("Process %d -> eat and drink\n",getpid());
}

int main(){
	int fd, data_size, trunk;
	barrier *ptr;
	
	data_size = sizeof(barrier);
	
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
	
	ptr = (barrier *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	sem_t *semaphore1 = sem_open("/sem1",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	sem_t *semaphore2 = sem_open("/sem2",O_CREAT|O_EXCL, 0644, 0);
	if(semaphore1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}


	
	
	
	pid_t pid_array[SIZE];
	int status_array[SIZE];
	
	for(int i = 0; i < SIZE;i++){											//fori cycle to create all childs
		
	pid_array[i] = fork();												//create cycle at position i of the pid_array
	
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
		
	
		if(pid_array[i] == 0){
			int random;
			srand(getpid());
			random = rand() % 10;
			printf("Process %d get %d\n",getpid(),random);
			
			if(random %2 == 0){		
				int sleepTime;
				srand(getpid());
				sleepTime = rand()%5 + 1;
				printf("Process %d sleeping %d\n",getpid(),sleepTime);
				sleep(sleepTime);
			
				buy_chips();
				
				sem_wait(semaphore1);
				ptr->nBarrier++;
				sem_post(semaphore1);
				/* checks if all processes have reached the barrier */
				if (ptr->nBarrier == SIZE)sem_post(semaphore2);
				/* waits in barrier */
				sem_wait(semaphore2);
				/* allows last process to also continue */
				sem_post(semaphore2);


				eat_and_drink();
			
				exit(0);
			}else{
				int sleepTime;
				srand(getpid());
				sleepTime = rand()%5 + 1;
				printf("Process %d sleeping %d\n",getpid(),sleepTime);
				sleep(sleepTime);	
			
				buy_beer();
				
				sem_wait(semaphore1);
				ptr->nBarrier++;
				sem_post(semaphore1);
				/* checks if all processes have reached the barrier */
				if (ptr->nBarrier == SIZE)sem_post(semaphore2);
				/* waits in barrier */
				sem_wait(semaphore2);
				/* allows last process to also continue */
				sem_post(semaphore2);

				eat_and_drink();
				exit(0);
			}
		}
		
			
		
		
	}
	
	for(int i = 0; i < SIZE; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
    
    

	sem_unlink("/sem1");
	sem_unlink("/sem2");
	
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
