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

#include "ex1.h"

int main(int argc, char *argv[]){
	
	int fd, data_size,trunk = sizeof(student_info);
	student_info *ptr;
	
	fd = shm_open("/shmtest",O_RDWR, S_IRUSR|S_IWUSR);										// Open the shared memory segment with read/write permissions
	if (fd == -1) {
		perror("Erro! - Open the shared memory segment");
		return 1;
	}
	
	trunk = ftruncate(fd, data_size);																// Truncate the shared memory segment to the size of student_info struct
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (student_info *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	printf("Data in shared memory: %s | %d | %s \n", ptr->name, ptr->number, ptr->address);	// Print the data stored in the shared memory segment
		
	
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
