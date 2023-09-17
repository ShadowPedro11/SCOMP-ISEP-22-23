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

#define ROWS 20
#define COLUMNS 10

typedef struct {
    char board[ROWS][COLUMNS][50];
}sharedBoard;

int main(){

	shm_unlink("/shmtest");

	char sem_name[20];
	for (int i = 0; i < (ROWS * COLUMNS); i++)
	{
		sprintf(sem_name, "/semaphore%d", i);
		sem_unlink(sem_name);
	}
	

	int fd, data_size, trunk;
	sharedBoard *board;
	
	data_size = sizeof(sharedBoard);
	
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
	
	board = (sharedBoard *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (board == MAP_FAILED || board == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	
	
	sem_t *semaphores[ROWS*COLUMNS];
	
	for (int i = 0; i < (ROWS*COLUMNS); i++) {
		
		sprintf(sem_name, "/semaphore%d", i);
		semaphores[i] = sem_open(sem_name, O_CREAT|O_EXCL, 0644, 1);
		
		if(semaphores[i] == SEM_FAILED){
			perror("Sem fail");
			exit(EXIT_FAILURE);
		}
	
	}
	
	for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            strcpy(board->board[i][j], "EMPTY");
        }
    }
	


 	sem_unlink("board_exit");
	sem_t *exit_board = sem_open("board_exit", O_CREAT|O_EXCL, 0644, 0);

 
	//Waits until any client shuts down sared board
	sem_wait(exit_board);

	
    
    
	for (int i = 0; i < (ROWS*COLUMNS); i++) {
		
		sprintf(sem_name, "/semaphore%d", i);
		sem_unlink(sem_name);
	
	}
	
	
	if (munmap(board, data_size) < 0) {	// Unmap the shared memory segment to the process's address space
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
