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

#define SIZE 5



typedef struct {
	int array[10];
	int ax;
	int bx;
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
	
	sem_t *semaphore4 = sem_open("/sem4",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore4 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	sem_t *semaphore5 = sem_open("/sem5",O_CREAT|O_EXCL, 0644, 1);
	if(semaphore5 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
	
	
		
	for (int j = 0; j < 10; j++) {
		ptr->array[j] = 0;
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
			
			if(i%2==0){													//Ax
				
			
				for(int w = 0; w<10;w++){
					
					sem_wait(semaphore1);		//Main Block
					sem_wait(semaphore2);		//Block of Ax, if an Ax pass when Bx are executing they will be caught here
					sem_wait(semaphore4);		//ANother block for Ax 
				
					ptr->ax++;
				
					if(ptr->ax == 1){			//If there is only one proccess of Ax executing
						sem_wait(semaphore3);	//WIll block the Bx
						sem_wait(semaphore5);
						
					}
					sem_post(semaphore1);	//
					sem_post(semaphore4);	// Allowing other proccess of this set to execute
					sem_post(semaphore2);	//
					
			

					printf("Ax -> Ax: %d |Bx: %d =>",ptr->ax,ptr->bx);
					for(int y = 0; y<10;y++){
						ptr->array[y] = ptr->array[y] + y;
						printf("%d: %d |",y+1,ptr->array[y]);
					}
					printf("\n");
			
				
					ptr->ax--;
				
					if(ptr->ax == 0){			//When are no Ax executing
						sem_post(semaphore3);	//Free execution of Bx	
						sem_post(semaphore5);
					}
				
					
				
					sleep(5);
				}
				
			}else{														//Bx
				
				
				for(int w = 0; w<10;w++){
					
					sem_wait(semaphore1);		//Main Block
					
					sem_wait(semaphore5);		//Block of Bx, if an Bx pass when Bx are executing they will be caught here
					
					sem_wait(semaphore3);		////Another block for Bx
					
				
					ptr->bx++;
				
					if(ptr->bx == 1){			//If there is only one proccess of Bx executing
						sem_wait(semaphore4);	//Will block the Ax 
						sem_wait(semaphore2);	//
					}
					
					sem_post(semaphore1);	//
					sem_post(semaphore3);	//Allowing other proccess of this set to execute
					sem_post(semaphore5);	//
			
				
					printf("Bx -> Ax: %d |Bx: %d =>",ptr->ax,ptr->bx);
					for(int y = 0; y<10;y++){
						ptr->array[y] = ptr->array[y] + y;
						printf("%d: %d |",y+1,ptr->array[y]);
					}
					printf("\n");
				
					ptr->bx--;
				
					if(ptr->bx == 0){			//When are no Bx executing
						sem_post(semaphore4);	// Free the execution of Ax 
						sem_post(semaphore2);
					}
				
					
				
					sleep(6);
				}
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
	sem_unlink("/sem4");
	sem_unlink("/sem5");
	
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

void printArray(){
	
	
	
}
