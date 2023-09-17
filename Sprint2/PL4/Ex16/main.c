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
#define ARRAY_SIZE 10000
#define VECTOR_SIZE 1000

typedef struct {
	float array[VECTOR_SIZE];
	float maxValue;
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
	sem_t *semaphore3 = sem_open("/sem3",O_CREAT|O_EXCL, 0644, VECTOR_SIZE);
	if(semaphore3 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}

	
	
	int initArray[ARRAY_SIZE];
	
	for(int i = 0; i < ARRAY_SIZE; i++){		//Initialize array 
		int random;
		srand(i+getpid());
		random = rand() % ARRAY_SIZE;
		initArray[i] = random;
	}
	
	ptr->maxValue = 0;
	
	
	pid_t pid_array[SIZE+1];
	int status_array[SIZE+1];
	
	for(int i = 0; i < SIZE+1;i++){										//fori cycle to create all childs
		
		pid_array[i] = fork();											//create cycle at position i of the pid_array
	
		if(pid_array[i] < 0){											//Check if the process was created
			perror("Error creating Fork");                              //			
			return 1;                                                   //
		}
		
		
		if(pid_array[i] == 0){
			
			if(i==SIZE){						//Procces i will search fro max Value

				while(1){

					if(sem_trywait(semaphore3) == -1){		//check the value of semaphore 3, if semvalue = -1 then all proccess finished
						
						int semValue;
						sem_getvalue(semaphore3,&semValue);
						
						
						if(ptr->array[semValue] > ptr->maxValue){		//get last value writed and check if tis bigger than actual bigger
							
							ptr->maxValue = ptr->array[semValue];
						}
						
						exit(0);
					}else{
						sem_post(semaphore3);
						sem_post(semaphore2);
						
						sem_wait(semaphore1);
						
						int semValue;
						sem_getvalue(semaphore3,&semValue);
						
						
						if(ptr->array[semValue] > ptr->maxValue){		//Check if the value is bigger
							
							ptr->maxValue = ptr->array[semValue];
						}
						
					}
				}
				
			}else{
			
				int start = i*200;
				int end = start + 200;
			
				for(int j = start; j < end;j++){			//Get average value
				
					int sum = 0;
					for(int k = j; k < (j+10); k++){
						sum = sum + initArray[k];
					}
				
					float average = (float)sum/10;
				
					
					sem_wait(semaphore2);			//ensure that only one procces is writing in memory
					
					
					sem_wait(semaphore3);			//
					
					int semValue;
					sem_getvalue(semaphore3,&semValue);
					
					
					ptr->array[semValue] = average;
					
					sem_post(semaphore1);		
				
				}
			
			}
			
			exit(0);
		}
		
	}
	
	
	
	for(int i = 0; i < SIZE+1; i++){
		waitpid(pid_array[i], &status_array[i], 0);						//
	}
    
    for(int i = 0; i < VECTOR_SIZE; i++){
		printf("NUM %d: %.2f\n",i+1,ptr->array[i]);
	}
	
	printf("MAX: %.2f\n",ptr->maxValue);
    

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
