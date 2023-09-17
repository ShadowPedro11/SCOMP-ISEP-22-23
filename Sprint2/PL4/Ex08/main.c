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

#define NUMBER_RECORDS 100
#define CHILDS_NUM 100

typedef struct {
    char name[80];
    char address[80];
    int number;
}Personal_Data;

typedef struct {
   Personal_Data records[NUMBER_RECORDS];
   int index;
   int write_count;
   int read_count;
   int servers;
}Record;

int main() {
    int fd, data_size = sizeof(Record), trunk;
    Record *shm_ptr;
	
	fd = shm_open("/shmtest", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of CircularBuffer struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of CircularBuffer 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	shm_ptr = (Record *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (shm_ptr == MAP_FAILED || shm_ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}

    sem_t *mutex1 = sem_open("/sem_mutex1",O_CREAT, 0644, 1);
	if(mutex1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *mutex2 = sem_open("/sem_mutex2",O_CREAT, 0644, 1);
	if(mutex2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *mutex3 = sem_open("/sem_mutex3",O_CREAT, 0644, 1);
	if(mutex3 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *w = sem_open("/sem_w",O_CREAT, 0644, 1);
	if(w == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *r = sem_open("/sem_r",O_CREAT, 0644, 1);
	if(r == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *s = sem_open("/sem_s",O_CREAT, 0644, 1);
	if(s == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}

    sem_wait(s);
        shm_ptr->servers++;
        if (shm_ptr->servers == 0) {
           for (int i = 0; i < NUMBER_RECORDS; i++) {
            shm_ptr->records[i].number = 0;
           }

            shm_ptr->index = 0;
            shm_ptr->write_count = 0;
            shm_ptr->read_count = 0;
        }
    sem_post(s);

    int option;
    do
    {
        printf("Choose an option\n1. Insert \n2. Consult \n3. Consult all\n");
		scanf("%d", &option);
		
		switch(option){
			case 0:
				break;
			case 1:
				execlp("./insert.o","./insert.o",(char *)NULL);
				break;
		
			case 2:
				execlp("./consult.o","./consult.o",(char *)NULL);
				break;
		
			case 3:
				execlp("./consultall.o","./consultall.o",(char *)NULL);
				break;
			default:
				printf("Selected option not available.\n");
		}
     
    } while (option != 0);

    sem_unlink("/sem_mutex1");
    sem_unlink("/sem_mutex2");
    sem_unlink("/sem_mutex3");
    sem_unlink("/sem_w");
    sem_unlink("/sem_r");
    sem_unlink("/sem_s");

    if (munmap(shm_ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment\n");	
		exit(1);
	}
	close(fd);

    return 0;
    
}