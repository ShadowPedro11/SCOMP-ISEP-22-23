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

#include "ex4.h"

int main(int argc, char *argv[]){
	int fd, trunk, data_size;
	share *ptr;
	
	data_size = sizeof(share);
	
	fd = shm_open("/shmtest",O_RDWR, S_IRUSR|S_IWUSR);								// Open the shared memory segment with read/write permissions
	if (fd == -1) {
		perror("Erro!");
		return 1;
	}
	
	trunk = ftruncate(fd, data_size);														// Truncate the shared memory segment to the size of share struct
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (share *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro!");
		return 1;
	}

	for (int j = 0; j < 5; j++) {
		while(!ptr->flag);
		printf("\nIteration %d\n", j+1);

		float sum = 0;
		
		for(int i = 0; i<10;i++){											// Read from shared memory and calculate the sum 
			printf("NUM.%d : %d\n",i+1,ptr->array[i]);
			sum = sum + ptr->array[i];
		}
			
		printf("Average : %.2f\n",(sum/10));								// Print average
	}

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
