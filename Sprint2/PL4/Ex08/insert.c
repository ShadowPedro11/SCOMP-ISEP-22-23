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
}Record;

int main(){
    char in_name[50], in_address[50];
    int in_number;

    int fd, data_size, trunk;
	Record *shm_ptr;
	
	data_size = sizeof(Record);
	
	fd = shm_open("/shmtest", O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it 
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);										// Truncate the shared memory segment
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}

    shm_ptr = (Record *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (shm_ptr == MAP_FAILED || shm_ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}

    sem_t *mutex1 = sem_open("/sem_mutex1", 0);
	if(mutex1 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *mutex2 = sem_open("/sem_mutex2", 0);
	if(mutex2 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *mutex3 = sem_open("/sem_mutex3", 0);
	if(mutex3 == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *w = sem_open("/sem_w", 0);
	if(w == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}
    sem_t *r = sem_open("/sem_r", 0);
	if(r == SEM_FAILED){
		printf("Error creating the semaphore!\n");
		exit(EXIT_FAILURE);
	}

    printf("Insert Name:\n");
    scanf("%s", in_name);
    

    printf("Insert Address:\n");
    scanf("%s", in_address);


    printf("Insert Number:\n");
    scanf("%d", &in_number);

    sem_wait(mutex2);
        shm_ptr->write_count++;
        if (shm_ptr->write_count == 1) sem_wait(r);
    sem_post(mutex2);

    sem_wait(w);




    int index = shm_ptr->index, exists = 0;

	for(int i = 0; i < index + 1; i++){
		if(in_number == shm_ptr->records[i].number){
			printf("Number already exists in the system.\n");
			exists = 1;
			return EXIT_FAILURE;
		}
	}
	
    if (exists == 0) {
        strcpy(shm_ptr->records[index].name, in_name);
	    strcpy(shm_ptr->records[index].address, in_address);
	    shm_ptr->records[index].number = in_number;
    }

    sem_post(w);

    sem_wait(mutex2);
        shm_ptr->write_count--;
        if (shm_ptr->write_count == 0)sem_post(r);
    sem_post(mutex2);

    sem_unlink("/sem_mutex1");
    sem_unlink("/sem_mutex2");
    sem_unlink("/sem_mutex3");
    sem_unlink("/sem_w");
    sem_unlink("/sem_r");

    if (munmap(shm_ptr, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
		perror("Erro! -  Unmap the shared memory segment\n");	
		exit(1);
	}
	close(fd);

    exit(EXIT_SUCCESS);
}