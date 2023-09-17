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
#include <semaphore.h>

#define STRINGS_NUMBER 50
#define STRING_SIZE 80
#define CHILDS_NUM 10

typedef struct {
    char strings[STRINGS_NUMBER][STRING_SIZE];
}shared;

int main() {
	
	int timeout = 12;
	struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout;
	 
    int fd, trunk, data_size = sizeof(shared);
	shared *ptr;

	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);		// Create a new shared memory segment with read/write permissions, and truncate it to the size of share struct
	if (fd == -1) {
		perror("shm_open failed!\n");
		return 1;
	}

	trunk = ftruncate(fd, data_size);		// Truncate the shared memory segment to the size of share
	if (trunk == -1) {
		perror("ftruncate failed!\n");
		return 1;
	}

	ptr = (shared *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);		// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED) {
		perror("mmap failed!\n");
		return 1;
	}   

    int write_count = 0;
    int w = 1;

    sem_t *mutex1 = sem_open("/sem_mutex1",O_CREAT|O_EXCL, 0644, 1);
	if(mutex1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}    




    pid_t pid_array[CHILDS_NUM];
    int status_array[CHILDS_NUM];

    for (int i = 0; i < CHILDS_NUM; i++) {
        pid_array[i] = fork();

        if (pid_array[i] == -1){
            perror("Fork failed!\n");

            if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
                perror("Error! -  Unmap the shared memory segment");	
                exit(1);
            }

            close(fd);

            if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
                perror("Error! - unlink the shared memory segment"); 
                exit(1);
            }
            return 1;
        }

        if (pid_array[i] == 0){
			
            int j = 0;
            
			int random = 0;
            while (j < STRINGS_NUMBER){									//THe programm will execute until the array is full
				
                int random;												//
                srand(random * getpid() + time(NULL));					// Generate random number between 1 and 100 
                random = rand() % 100 + 1;								//
                
                if (sem_timedwait(mutex1,&ts) == -1) {					// Wait for 12 seconds
					printf("Timed out waiting for semaphore\n");		//If cannot access print this message
					exit(0);
				}else{	
                
					if(random <= 70){									// 70%->Write
					
						if (ptr->strings[j][0] == NULL){
							sprintf(ptr->strings[j], "I'm the Father - with PID %d, on str %d\n", getpid(), j);
							printf("%s", ptr->strings[j]);
							srand(time(NULL) + getpid());
							int sleep_time = rand() % 5 + 1;
							sleep(sleep_time);
							j++;
						} else {	
							j++;                    
						}
			
					}else{												// 30%->Delete
					
						if(j>0){							
						
							if (ptr->strings[j-1][0] != NULL){
						
								ptr->strings[j-1][0] = NULL;
						
								printf("I'm the Father - with PID %d, deleted str %d\n", getpid(), j-1);
						
								srand(time(NULL) + getpid());
								int sleep_time = rand() % 5 + 1;
								sleep(sleep_time);
								j--;
							}
						}
					 
					}
					
                 sem_post(mutex1);   
              
			}
                
                
            }
            
            exit(EXIT_SUCCESS);
        }
    }
    
    for (int i = 0; i < CHILDS_NUM; i++){
        waitpid(pid_array[i], &status_array[i], 0);
    }
    
    sem_unlink("/sem_mutex1");												// unlink semaphore

    if (munmap(ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Error! -  Unmap the shared memory segment");	
		exit(1);
	}

	close(fd);

	if (shm_unlink("/shmtest") < 0) {	// Unlink the shared memory segment (removes the shared memory segment from the file
		perror("Error! - unlink the shared memory segment"); 
		exit(1);
	}


    return 0;
}
