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
	int fd, trunk, unlink, unmap, close_fd, data_size = sizeof(share);
	share *ptr;

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

	ptr = (share *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);		// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED) {
		perror("mmap failed!\n");
		return 1;
	}
	
	for (int j = 0; j < 5; j++) {
		ptr->flag = 0;
		for (int i = 0; i < 10; i++) {			// Generate 10 random numbers between 1 and 20 and write them in the shared memory.
        	ptr->array[i] = rand() % 20 + 1;
        	printf("%d-", ptr->array[i]);
    	}
		ptr->flag = 1;
	}
        	
	return 0;
}
