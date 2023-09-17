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

#define BUFFER_SIZE 10
#define VALUES 30

typedef struct {
    int buffer[BUFFER_SIZE];
    int produzidos;
    int consumidos;
} CircularBuffer;



int main(int argc, char *argv[]){
	

	
	//shm_unlink("/shmtest");												// Unlink (delete) any existing shared memory segment with the same name
	
	int fd, data_size, trunk;
	CircularBuffer *ptr;
	
	data_size = sizeof(CircularBuffer);
	
	fd = shm_open("/shmtest", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);				// Create a new shared memory segment with read/write permissions, and truncate it to the size of CircularBuffer struct
	if (fd == -1) {
		perror("Erro! - Create a new shared memory segment");
		return 1;
	}

	trunk = ftruncate(fd, data_size);												// Truncate the shared memory segment to the size of CircularBuffer 
	if (trunk == -1) {
		perror("Erro! - Truncate the shared memory segment");
		return 1;
	}
	
	ptr = (CircularBuffer *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);	// Map the shared memory segment to the process's address space
	if (ptr == MAP_FAILED || ptr == 0) {
		perror("Erro! - Map the shared memory segment");
		return 1;
	}
	
	for (int i = 0; i < BUFFER_SIZE; i++) {
        ptr->buffer[i] = 0;
    }
    
	pid_t pid = fork();													// Create new proccess
	
	if(pid < 0){														//Check creation of child process 
		perror("Error creating process!");
		return 1;
	}
	
	if(pid > 0){														//Father-Producer
		
		for(int i = 0; i<30;i++){										//Fori cycle to exchenge 30 values
			
			while(ptr->produzidos - ptr->consumidos == BUFFER_SIZE);	// wait while qnt produzida - ant consumida = 10
			int position = (i%BUFFER_SIZE);								// found position on array to write
			
			while(ptr->buffer[position] != 0);							// check the position to write
			ptr->buffer[position] = i;									// write 
			
			printf("Producer | Item: %d\n",i);							// print the action
			
			ptr->produzidos = ptr->produzidos +1;						// increase the qnt produzida
		}
			
		
	}else{																//Child-Consumer
		
		for(int i = 0; i<30;i++){										//Fori cycle to exchenge 30 values
			while(ptr->produzidos - ptr->consumidos < 0);				//wait while check if have values available to read 
			int position = (i%BUFFER_SIZE);								// found position on array to write
			
			printf("Consumer | Item: %d\n",ptr->buffer[position]);		// print the action
			
			ptr->buffer[position] = 0;									// clear the value readed
			
			ptr->consumidos = ptr->consumidos +1;						// increase the qnt consumida
		}
		
		
		
		
        exit(0);
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
