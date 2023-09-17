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
    int index = 
    0;
    char strings[STRINGS_NUMBER][STRING_SIZE];
}shared;

int main() {
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
    int read_count = 0;
    
    sem_t *w = sem_open("/sem_w",O_CREAT|O_EXCL, 0644, 1);
	if(w == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 

    sem_t *r = sem_open("/sem_r",O_CREAT|O_EXCL, 0644, 1);
	if(r == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	} 

    sem_t *mutex1 = sem_open("/sem_mutex1",O_CREAT|O_EXCL, 0644, 1);
	if(mutex1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}    

    sem_t *mutex2 = sem_open("/sem_mutex2",O_CREAT|O_EXCL, 0644, 1);
	if(mutex2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}  

    sem_t *mutex3 = sem_open("/sem_mutex3",O_CREAT|O_EXCL, 0644, 1);
	if(mutex3 == SEM_FAILED){
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
            //Child - Producers
            int j = 0;

            while (j < STRINGS_NUMBER){
                printf("oi3\n");
                sem_wait(mutex2);
                    write_count++;
                    if (write_count == 1) sem_wait(r);
                sem_post(mutex2);
                    
                sem_wait(w);
                    if (ptr->strings[j][0] == 0){
                        sprintf(ptr->strings[j], "I'm the Father - with PID %d, on str %d\n", getpid(), j);
                        printf("Filho %s", ptr->strings[j]);
                        srand(time(NULL) + getpid());
                        int sleep_time = rand() % 5 + 1;
                        sleep(sleep_time);
                        j++;
                        
                    } else {
                        j++;
                        //printf("Entrou else\n");
                    }
                sem_post(w);
                
                printf("oi1\n");
                sem_wait(mutex2);
                printf("oi2\n");
                    write_count--;
                    if (write_count == 0) sem_post(r);
                sem_post(mutex2);
            }
            exit(EXIT_SUCCESS);
        }
    }

    int total_lines = 0;

    while (total_lines < STRINGS_NUMBER) {
        sem_wait(mutex3);
            sem_wait(r);
            printf("pai While \n");
                sem_wait(mutex1);
                    read_count++;
                    if (read_count == 1) sem_wait(w);
                sem_post(mutex1);
            sem_post(r);
        sem_post(mutex3);

        total_lines++;
        printf("%s", ptr->strings[total_lines]);
        printf("Total number of lines: %d\n", total_lines);

        sem_wait(mutex1);
            read_count--;
            if (read_count == 0) sem_post(w);
        sem_post(mutex1);
    }
    
    for (int i = 0; i < CHILDS_NUM; i++)
    {
        waitpid(pid_array[i], &status_array[i], 0);
    }

    sem_unlink("/sem_mutex1");      // unlink semaphore
    sem_unlink("/sem_mutex2");
    sem_unlink("/sem_mutex3");
    sem_unlink("/sem_w");
    sem_unlink("/sem_r");

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