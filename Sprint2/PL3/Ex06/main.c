#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>

#define SIZE 1000
#define CHILDS_SIZE 10

typedef struct{
	int array[CHILDS_SIZE];
	int flag;
}nums;


int main(int argc, char *argv[]){
	
	//shm_unlink("/shmtest");												// Unlink (delete) any existing shared memory segment with the same name

	int numbers[SIZE];
	srand(time(NULL));
    
    for (int i = 0; i < SIZE; i++) {									// Create an array of 1000 integers, initializing it with random values between 0 and 1000
       numbers[i] = rand() % 1000+1;
    }
	
	
	int fd, data_size, trunk;
	nums *ptr;
	
	data_size = sizeof(nums);
	
	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of nums struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of nums 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (nums *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}

	
    
    ptr->flag = 0;														// Assign the value of flag to 0
	pid_t pid = fork();													// Create new proccess
	
	if(pid < 0){														//Check creation of child process 
		perror("Error creating process!");
		return 1;
		}
	
	if(pid > 0){
		
		pid_t pid_array[CHILDS_SIZE];									//creation of an array of pid_t to create all child's
		int status_array[CHILDS_SIZE];
		
		for(int i = 0; i < CHILDS_SIZE;i++){
			pid_array[i] = fork();										//create cycle at position i of the pid_array

			if(pid_array[i] < 0){										//Check if the process was created
				perror("Error creating Fork");
			return 1;
			}
			
			if(pid_array[i] == 0){										// If in child process 
				int num = 0;
				int initial = i * 100;
				int end = initial + 100;
				for(int i = initial;i<end;i++){							// seacrh the biggest value
					if(numbers[i]>num){
						num = numbers[i];
					}
				}
				ptr->array[i] = num;									// Write the biggest value found on array at position i
				exit(0);	
			 }
		}
		
		for(int i = 0; i < CHILDS_SIZE; i++){							//Wait for all child process
			waitpid(pid_array[i], &status_array[i], 0);						
		}
		
		ptr->flag = 1;
	
	}else{
		while(!ptr->flag);												// While flag is 0 keep waiting
		
		int max = 0;
		for (int i = 0; i < CHILDS_SIZE; i++) {							// search for the biggest value on the array
			if(ptr->array[i] > max){
				max = ptr->array[i];
			}		
		}
		printf("MAX: %d\n",max);										// Print the max value
		
		
        exit(0);
	}
	
	wait(NULL);


	
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
